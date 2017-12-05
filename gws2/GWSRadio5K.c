/*
 * GWSRadio5K.c
 *
 *  Created on: Nov 7, 2017
 *      Author: Qige <qigezhao@gmail.com>
 */

#include <stdlib.h>

#include "gws2.h"
#include "sys.h"


static void gws5kRadioRfinfo(void);
static int gws5kFetch(const char *cmd);

static int gws5kRadioTxPwr(void);
static int gws5kRadioRxGain(void);
static int gws5kRadioRegion(void);
static int gws5kRadioChannel(void);
static int gws5kRadioChanBw(void);


int gws5kRadioUpdate(void *pRadio)
{
	struct gws2Radio *radio = (struct gws2Radio *) pRadio;

	gws5kRadioRfinfo();

	radio->tvws.txPwr = gws5kRadioTxPwr();
	radio->tvws.rxGain = gws5kRadioRxGain();
	radio->tvws.region = gws5kRadioRegion();
	radio->tvws.channel = gws5kRadioChannel();
	radio->tvws.chanBw = gws5kRadioChanBw();

	return 0;
}

static void gws5kRadioRfinfo(void)
{
	sysExecute("rfinfo > /tmp/.WB.rfinfo");
}

static int gws5kRadioTxPwr(void)
{
	const char *cmd = "cat /tmp/.WB.rfinfo | grep Tx | grep Power | awk '{print $3}'";
	return gws5kFetch(cmd);
}
static int gws5kRadioRxGain(void)
{
	const char *cmd = "cat /tmp/.WB.rfinfo | grep Rx | grep Gain | awk '{print $3}'";
	return gws5kFetch(cmd);
}

static int gws5kRadioRegion(void)
{
	const char *cmd = "cat /tmp/.WB.rfinfo | grep Region | awk '{print $2}'";
	return gws5kFetch(cmd);
}

static int gws5kRadioChannel(void)
{
	const char *cmd = "cat /tmp/.WB.rfinfo | grep Chan: | awk '{print $2}'";
	return gws5kFetch(cmd);
}

static int gws5kRadioChanBw(void)
{
	const char *cmd = "cat /tmp/.WB.rfinfo | grep Chan | grep BW | awk '{print $3}'";
	return gws5kFetch(cmd);
}


static int gws5kFetch(const char *cmd)
{
	const char *val = sysCommand(cmd);
	if (val) {
		return atoi(val);
	}
	return -1;
}
