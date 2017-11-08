/*
 * sys.c
 *
 *  Created on: Nov 7, 2017
 *      Author: Qige <qigezhao@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sysExecute(const char *cmd)
{
	//printf(">>>> sysExecute(%s)\n", cmd);
	system(cmd);
}

char *sysCommand(const char *cmd)
{
	FILE *fp;
	unsigned int bufferLength;
	static char buffer[2048];

	fp = popen(cmd, "r");
	memset(buffer, 0x00, sizeof(buffer));
	fread(buffer, sizeof(buffer)-1, sizeof(char), fp);
	bufferLength = strlen(buffer);
	if (bufferLength > 0) {
		if (buffer[bufferLength-1] == '\n') buffer[bufferLength-1] = '\0';
	}
	buffer[bufferLength] = '\0';
	pclose(fp);

	return buffer;
}
