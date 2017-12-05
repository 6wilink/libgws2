/*
 * GWSRadio3K.c
 *
 *  Created on: Nov 7, 2017
 *      Author: qige
 */

#include "gws2.h"

int gws3kRadioUpdate(void *pRadio)
{
	struct gws2Radio *radio = (struct gws2Radio *) pRadio;

	radio->tvws.txPwr = 23;
	radio->tvws.rxGain = 0;
	radio->tvws.region = 0;
	radio->tvws.channel = 14;
	radio->tvws.chanBw = 5;

	return 0;
}

