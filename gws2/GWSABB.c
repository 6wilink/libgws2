/*
 * GWSABB.c
 *
 *  Created on: Nov 8, 2017
 *      Author: Qige <qigezhao@gmail.com>
 */

#include "gws2.h"
#include "./iwinfo/iwinfo.h"


static int abbChanBw(void);
static int abbSignal(const struct iwinfo_ops *iw);
static int abbNoise(const struct iwinfo_ops *iw);
static int abbModeNum(const struct iwinfo_ops *iw);
static int abbPeerUpdate(const struct iwinfo_ops *iw,
		struct gws2AbbPeer *pPeer,
		const int max);


int GWSABBUpdate(void *pAbb)
{
	struct gws2AbbPeer gws2AbbPeer;
	struct gws2Abb *abb = (struct gws2Abb *) pAbb;

	const struct iwinfo_ops *iw;
	iw = iwinfo_backend(GWS_ABB_DEFAULT_IFNAME);
	if (! iw) {
		printf("error> abb device not exists\n");
		return -1;
	}

	abb->noise = abbNoise(iw);
	abb->signal = abbSignal(iw);
	abb->modeNum = abbModeNum(iw);
	abb->chanBw = abbChanBw();

	if (abb->signal < abb->noise) {
		abb->signal = abb->noise;
	}

	// TODO: update all
	memset(&gws2AbbPeer, 0x0, sizeof(gws2AbbPeer));
	abbPeerUpdate(iw, &gws2AbbPeer, 5);
	abb->peersSignal[0] = abb->signal;
	abb->peersSignal[1] = gws2AbbPeer.pSignal;

	return 0;
}

static int abbPeerUpdate(const struct iwinfo_ops *iw, struct gws2AbbPeer *pPeer, const int max)
{
	int i;
	int dataLength;
	char data[IWINFO_BUFSIZE];

	//struct iwinfo_rate_entry *br;
	struct iwinfo_assoclist_entry *e;

	if (iw->assoclist(GWS_ABB_DEFAULT_IFNAME, data, &dataLength)) {
		printf("error> abb has no peers\n");
		return -1;
	} else if (dataLength <= 0) {
		printf("--------> no station connected\n");
		return -2;
	}

	for (i = 0; i < dataLength; i += sizeof(struct iwinfo_assoclist_entry)) {
		e = (struct iwinfo_assoclist_entry *) &data[i];
		pPeer->pInactive = e->inactive;
		pPeer->pSignal = e->signal;
	}

	return 0;
}

static int abbChanBw(void)
{
	return 8;
}
static int abbSignal(const struct iwinfo_ops *iw)
{
	int signal;
	if (iw && iw->signal(GWS_ABB_DEFAULT_IFNAME, &signal)) {
		signal = GWS_BAR_VAL;
	}
	return signal;
}
static int abbNoise(const struct iwinfo_ops *iw)
{
	int noise;
	if (iw && iw->noise(GWS_ABB_DEFAULT_IFNAME, &noise)) {
		noise = GWS_BAR_VAL;
	}
	return noise;
}
static int abbModeNum(const struct iwinfo_ops *iw)
{
	int mode, wifiMode;
	if (iw && iw->mode(GWS_ABB_DEFAULT_IFNAME, &wifiMode)) {
		mode = GWS_DEFAULT;
	} else {
		switch(wifiMode) {
		case IWINFO_OPMODE_MASTER:
			mode = GWS_CAR_AP;
			break;
		case IWINFO_OPMODE_CLIENT:
			mode = GWS_EAR_STA;
			break;
		case IWINFO_OPMODE_MESHPOINT:
			mode = GWS_MESH_POINT;
			break;
		default:
			mode = GWS_DEFAULT;
			break;
		}
	}
	return mode;
}
