/*
 * gws2.c
 *
 *  Created on: Nov 7, 2017
 *  Updated on: Nov 8, 2017
 *      Author: Qige <qigezhao@gmail.com>
 */

#include <stdio.h>
#include <string.h>

#include "sys.h"
#include "gws2.h"
#include "GWSABB.h"
#include "GWSRadio5K.h"
#include "GWSRadio3K.h"

// instant of all queries
static struct gws2 SG_GWS;

static int gws2HwArch(void);
static int (*gws2RadioUpdate)(void *radio);

// sync ABB, Radio right away, and save to "SGWS"
void gws2Sync(void)
{
	// TODO: query from libiwinfo
	struct gws2Abb abb;
	memset(&abb, 0x0, sizeof(abb));
	if (GWSABBUpdate(&abb)) {
		printf("error> failed to update abb\n");
	} else {
		memcpy(&(SG_GWS.abb), &abb, sizeof(struct gws2Abb));
	}

	// TODO: query from command line
	const char *ipaddr = sysCommand("uci get network.lan.ipaddr");
	memset(SG_GWS.sys.ipaddr, 0x0, sizeof(SG_GWS.sys.ipaddr));
	memcpy(SG_GWS.sys.ipaddr, ipaddr, strlen(ipaddr));

	// TODO: update from command line "rfinfo"
	struct gws2Radio radio;
	memset(&radio, 0x0, sizeof(radio));
	SG_GWS.radio.hwArch = gws2HwArch();
	if (SG_GWS.radio.hwArch == GWS_5000_V2) {
		gws2RadioUpdate = gws5kRadioUpdate;
	} else if (SG_GWS.radio.hwArch == GWS_5000) {
		gws2RadioUpdate = gws5kRadioUpdate;
	} else if (SG_GWS.radio.hwArch == GWS_4000) {
		gws2RadioUpdate = gws3kRadioUpdate;
	} else if (SG_GWS.radio.hwArch == GWS_3000) {
		gws2RadioUpdate = gws3kRadioUpdate;
	} else if (SG_GWS.radio.hwArch == GWS_WIFI) {
		gws2RadioUpdate = gws3kRadioUpdate;
	} else if (SG_GWS.radio.hwArch == GWS_GENERIC) {
		gws2RadioUpdate = gws3kRadioUpdate;
	}

	if ((*gws2RadioUpdate)(&radio)) {
		printf("error> failed to update radio\n");
	} else {
		printf("------> radio params udpated\n");
		SG_GWS.radio.tvws.txPwr = radio.tvws.txPwr;
		SG_GWS.radio.tvws.rxGain = radio.tvws.rxGain;
		SG_GWS.radio.tvws.region = radio.tvws.region;
		SG_GWS.radio.tvws.channel = radio.tvws.channel;
		SG_GWS.radio.tvws.chanBw = radio.tvws.chanBw;
	}
}

// judge hw arch by CPU & /dev/*
static int gws2HwArch(void)
{
	// TODO: add judge hw arch code
	return GWS_5000_V2;
}

// free memory requested by malloc()
void gws2SyncClean(void) {}

// System related
char *gws2SysIpAddr(void)
{
	return SG_GWS.sys.ipaddr;
}
char *gws2SysHwArch(void)
{
	if (SG_GWS.radio.hwArch == GWS_5000_V2) {
		return "GWS5Kv2";
	} else if (SG_GWS.radio.hwArch == GWS_5000) {
		return "GWS5K";
	} else if (SG_GWS.radio.hwArch == GWS_4000) {
		return "GWS4K";
	} else if (SG_GWS.radio.hwArch == GWS_3000) {
		return "GWS3K";
	} else if (SG_GWS.radio.hwArch == GWS_WIFI) {
		return "GWSWiFi";
	} else if (SG_GWS.radio.hwArch == GWS_GENERIC) {
		return "GWSGeneric";
	} else {
		return "GWSUnknown";
	}
}

// Analog Base Band related
int gws2AbbSignal(void)
{
	return SG_GWS.abb.signal;
}
int gws2AbbNoise(void)
{
	return SG_GWS.abb.noise;
}
int gws2AbbMode(void)
{
	return SG_GWS.abb.modeNum;
}
int gws2AbbPeerSignal(const int index)
{
	int signal, noise = SG_GWS.abb.noise;
	if (index >= 0 && index < GWS_ABB_DEFAULT_PEER_QTY) {
		signal = SG_GWS.abb.peers[index-1].pSignal;
	} else {
		signal = noise;
	}

	if (signal < noise) {
		signal = noise;
	}

	return signal;
}
int gws2AbbChanBw(void)
{
	return SG_GWS.abb.chanBw;
}

// Radio related
int gws2RadioTxPwr(void)
{
	return SG_GWS.radio.tvws.txPwr;
}
int gws2RadioRxGain(void)
{
	return SG_GWS.radio.tvws.rxGain;
}
int gws2RadioRgn(void)
{
	return SG_GWS.radio.tvws.region;
}
int gws2RadioChan(void)
{
	return SG_GWS.radio.tvws.channel;
}
int gws2RadioChanBw(void)
{
	// return min(Abb chanbw, Radio chanbw)
	return SG_GWS.radio.tvws.chanBw;
}

// System related
void gws2CmdSysReboot(void)
{
	sysExecute("reboot");
}
void gws2CmdSysFailsafe(void)
{
	sysExecute("ifconfig eth0 192.168.1.1/24 up");
}

// ABB related
void gws2CmdAbbReset(void)
{
	sysExecute("wifi");
}
void gws2CmdAbbModeMesh(void)
{
	sysExecute("arn -M mesh");
}
void gws2CmdAbbModeSta(void)
{
	sysExecute("arn -M ear");
}
void gws2CmdAbbModeAp(void)
{
	sysExecute("arn -M car");
}
void gws2CmdAbbChanBw(const int val)
{
	char cmd[GWS_DEFAULT_CMD_LENGTH];
	memset(cmd, 0x0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),
			"uci set wireless.@wifi-iface[0].chanbw=%d; "\
			"uci commit wireless; "\
			"wifi\n", val
	);
	sysExecute(cmd);
}

// Radio related
void gws2CmdRadioPerfMin(void)
{
	sysExecute("arn -P 17");
}
void gws2CmdRadioPerfMax(void)
{
	sysExecute("arn -P 33");
}
void gws2CmdRadioChan(const int val)
{
	char cmd[GWS_DEFAULT_CMD_LENGTH];
	memset(cmd, 0x0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "arn -C %d\n", val);
	sysExecute(cmd);
}
void gws2CmdRadioChanBw(const int val)
{
	char cmd[GWS_DEFAULT_CMD_LENGTH];
	memset(cmd, 0x0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "arn -B %d\n", val);
	sysExecute(cmd);
}
void gws2CmdRadioTxPwr(const int val)
{
	char cmd[GWS_DEFAULT_CMD_LENGTH];
	memset(cmd, 0x0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "arn -P %d\n", val);
	sysExecute(cmd);
}
void gws2CmdRadioRegion(const int val)
{
	char cmd[GWS_DEFAULT_CMD_LENGTH];
	memset(cmd, 0x0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "arn -R %d\n", val);
	sysExecute(cmd);
}
