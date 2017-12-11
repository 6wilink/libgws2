/*
 * gws2.h
 *
 *  Created on: Nov 7, 2017
 *  Updated on: Nov 8, 2017
 *      Author: Qige <qigezhao@gmail.com>
 */

#ifndef GWS2_H_
#define GWS2_H_

#define GWS_DEFAULT_CMD_LENGTH 		16

#define GWS_ABB_DEFAULT_PEER_QTY 	10
#define GWS_ABB_DEFAULT_IFNAME 		"wlan0"
#define GWS_ABB_BAR_INACTIVE 		3000

#define GWS_MAC_LENGTH 				18
#define GWS_SSID_LENGTH 			16

#define GWS_BAR_VAL 				-110

// hardware architecture
enum GWS_HWARCH
{
	GWS_UNKNOWN = 0,
	GWS_GENERIC = 1,
	GWS_WIFI = 2,
	GWS_3000 = 3,
	GWS_4000 = 4,
	GWS_5000 = 5,
	GWS_5000_V2 = 6
};

enum GWS_ABB_MODE
{
	GWS_DEFAULT = 1,
	GWS_MESH_POINT = 0,
	GWS_EAR_STA = 1,
	GWS_CAR_AP = 2,
	GWS_AD_HOC = 3
};

// analog base band peer(s)
struct gws2AbbPeer
{
	//struct gws2AbbPeer *next, *prev;
	int mIndex, index;
	char pMac[GWS_MAC_LENGTH];
	int pSignal, pInactive;
	struct {
		float br;
		int mcs;
		unsigned long long packets;
	} rx;
	struct {
		float br;
		int mcs;
		unsigned long long packets;
	} tx;
};

struct gws2Abb
{
	//struct gws2Abb *next, *prev;
	int index;
	char wmac[GWS_MAC_LENGTH], bssid[GWS_MAC_LENGTH];
	char ssid[GWS_SSID_LENGTH];
	int signal, noise;
	int chanBw;
	enum GWS_ABB_MODE modeNum;
	struct {
		float br;
		int mcs;
	} rx;
	struct {
		float br;
		int mcs;
		unsigned long long bytes;
	} tx;
	struct gws2AbbPeer peers[GWS_ABB_DEFAULT_PEER_QTY];
};

struct gws2Radio
{
	struct gws2Radio *next, *prev;
	int index;
	enum GWS_HWARCH hwArch;
	struct {
		unsigned int freqMHz, chanBw;
		unsigned int region, channel;
		int tx, txPwr;
		int rx, rxGain;
	} tvws;
};

struct gws2System
{
	char ipaddr[16];
};

struct gws2
{
	unsigned long long ts;
	struct gws2Abb abb, *pAbb;
	struct gws2Radio radio, *pRadio;
	struct gws2System sys;
};


// sync with abb+radio
void gws2Sync(void);
void gws2SyncClean(void);

// which hw arch, which platform
char *gws2SysIpAddr(void);
char *gws2SysHwArch(void);

// all call gws2sync() before return
int gws2AbbSignal(void);
int gws2AbbNoise(void);

int gws2AbbMode(void);
int gws2AbbPeerSignal(const int index);

int gws2RadioTxPwr(void);
int gws2RadioRxGain(void);
int gws2RadioRgn(void);
int gws2RadioChan(void);
int gws2RadioChanBw(void);

void gws2CmdSysReboot(void);
void gws2CmdSysFailsafe(void);

void gws2CmdAbbReset(void);
void gws2CmdAbbModeMesh(void);
void gws2CmdAbbModeSta(void);
void gws2CmdAbbModeAp(void);
void gws2CmdAbbModeAdHoc(void);

void gws2CmdAbbChanBw(const int val);

void gws2CmdRadioPerfMin(void);
void gws2CmdRadioPerfMax(void);
void gws2CmdRadioChan(const int val);
void gws2CmdRadioChanBw(const int val);
void gws2CmdRadioTxPwr(const int val);
void gws2CmdRadioRegion(const int val);

#endif /* GWS2_H_ */
