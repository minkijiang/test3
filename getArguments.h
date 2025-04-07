#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct MONITORINFO {

	int tdelay;
	int samplesize;

	bool showMem;
	bool showCpu;
	bool showCores;


} MONITORINFO;

#define DEFAULTTDELAY 500000
#define DEFAULTSAMPLE 20
#define MAXLENGTH 1024
#define NOTHING -1

MONITORINFO* createMonitorInfo();

void processArguments(MONITORINFO* monitorinfo, int argc, char **argv);