#include "getArguments.h"

#include <string.h>
#include <math.h>

MONITORINFO* createMonitorInfo() {
	MONITORINFO* monitorinfo = malloc(sizeof(MONITORINFO));

	if (monitorinfo == NULL) {
		perror("failed to malloc");
		exit(1);
	}

	monitorinfo->tdelay = DEFAULTTDELAY;
	monitorinfo->samplesize = DEFAULTSAMPLE;

	monitorinfo->showMem = false;
	monitorinfo->showCpu = false;
	monitorinfo->showCores = false;

	return monitorinfo;
}

int getSampleSize(char* arg) {

	char SAMPLE[MAXLENGTH] = "--samples=";

	char arg2[MAXLENGTH];
	char sample[MAXLENGTH];
	strcpy(arg2, arg);

	if (strlen(arg2) > 10) {
		strcpy(sample, arg+10);
		arg2[10] = '\0';
	}
	else {
		return NOTHING;
	}

	if (strcmp(arg2, SAMPLE) != 0) {
		return NOTHING;
	}

	return strtol(sample, NULL, 10);
}

int getTdelay(char* arg) {

	char TDELAY[MAXLENGTH] = "--tdelay=";

	char arg2[MAXLENGTH];
	char tdelay[MAXLENGTH];
	strcpy(arg2, arg);

	if (strlen(arg2) > 9) {
		strcpy(tdelay, arg+9);
		arg2[9] = '\0';
	}
	else {
		return NOTHING;
	}

	if (strcmp(arg2, TDELAY) != 0) {
		return NOTHING;
	}

	return strtol(tdelay, NULL, 10);
}


void processArguments(MONITORINFO* monitorinfo, int argc, char **argv) {

	bool invalid_syntax = false;
	
	bool show_mem = false;
	bool show_cpu = false;
	bool show_cores = false;

	bool sample_size = DEFAULTSAMPLE;
	bool tdelay = DEFAULTTDELAY;


	if (argc == 1) {
		show_mem = true;
		show_cpu = true;
		show_cores = true;
	}
	else if (argc == 2 && strtol(argv[1], NULL, 10) != 0) {
		show_mem = true;
		show_cpu = true;
		show_cores = true;
		sample_size = strtol(argv[1], NULL, 10);
	}
	else if (argc == 3 && strtol(argv[1], NULL, 10) != 0 && strtol(argv[2], NULL, 10) != 0) {
		show_mem = true;
		show_cpu = true;
		show_cores = true;
		sample_size = strtol(argv[1], NULL, 10);
		tdelay = strtol(argv[2], NULL, 10);
	}
	else {
		
		int i0 = 1;
		if (argc  >= 2 && strtol(argv[1], NULL, 10) != 0) {
			sample_size = strtol(argv[1], NULL, 10);
			i0++;
		}
		if (argc >= 3 && strtol(argv[2], NULL, 10) != 0) {
			tdelay = strtol(argv[2], NULL, 10);
			i0++;
		}

		for (int i = i0; i < argc; i++) {
			if (strcmp(argv[i], "--memory") == 0) {
				show_mem = true;
			}

			else if (strcmp(argv[i], "--cpu") == 0) {
				show_cpu = true;
			}
			else if (strcmp(argv[i], "--cores") == 0) {
				show_cores = true;
			}
			
			else if (getSampleSize(argv[i]) != NOTHING) {
				sample_size = getSampleSize(argv[i]);
			}
			else if (getTdelay(argv[i]) != NOTHING) {
				tdelay = getTdelay(argv[i]);
			}
			else {
				invalid_syntax = true;
			}

		}


	}

	if (invalid_syntax) {
		fprintf(stderr, "Error: invalid command line arguments");
		exit(1);
	}

	monitorinfo->showMem = show_mem;
	monitorinfo->showCpu = show_cpu;
	monitorinfo->showCores = show_cores;
	monitorinfo->samplesize = sample_size;
	monitorinfo->tdelay = tdelay;
	monitorinfo->samplesize = sample_size;
	monitorinfo->tdelay = tdelay;
}