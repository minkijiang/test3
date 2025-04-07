
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define KB_TO_GB 1000000
#define MAXLENGTH 1024
#define NOTHING -1

float getMaxMemory() {


	FILE* memfile = fopen("/proc/meminfo", "r");

	if (memfile == NULL) {
		perror("failed to open /proc/meminfo");
		exit(1);
	}

	char line[MAXLENGTH];

	char word[MAXLENGTH];
	int maxmemory;

	while (fgets(line, (MAXLENGTH-1)*sizeof(char), memfile) != NULL) {
		sscanf(line, "%s %d" ,word, &maxmemory);
		if (strcmp(word, "MemTotal:") == 0) {
			break;
		}
	}

	int isClosed = fclose(memfile);
	if (isClosed != 0) {
		perror("failed to close /proc/meminfo");
		exit(1);
	}

	return (float)maxmemory/KB_TO_GB ;

	

	return 30;
}

float getMemoryUsage() {

	FILE* memfile = fopen("/proc/meminfo", "r");

	if (memfile == NULL) {
		perror("failed to open /proc/meminfo");
		exit(1);
	}

	char line[MAXLENGTH];

	char word[MAXLENGTH];
	int freememory;

	while (fgets(line, (MAXLENGTH-1)*sizeof(char), memfile) != NULL) {
		sscanf(line, "%s %d" ,word, &freememory);
		if (strcmp(word, "MemFree:") == 0) {
			break;
		}
	}

	int isClosed = fclose(memfile);
	if (isClosed != 0) {
		perror("failed to close /proc/meminfo");
		exit(1);
	}

	float maxmemory = getMaxMemory();

	return maxmemory - ((float)freememory/KB_TO_GB);

}

typedef struct CORES {
	int phy_id;
	int core_id;
	struct CORES* next;

} CORES;

typedef struct COREINFO {
	float maxghz;
	int coreamount;
} COREINFO;

void freeCores(CORES* cores) {
	for (CORES* core = cores; core != NULL; core = core->next) {
		free(core);
	}
}

CORES* addCore(CORES* cores, int phy_id, int core_id) {
	CORES* newcore = malloc(sizeof(CORES));
	newcore->phy_id = phy_id;
	newcore->core_id = core_id;
	newcore->next = cores;

	return cores;
}

bool coreExists(CORES* cores, int phy_id, int core_id) {
	for (CORES* core = cores; core != NULL; core = core->next) {
		if (core->phy_id == phy_id && core->core_id == core_id) {
			return true;
		}
	}
	return false;
}

int* getCore(FILE* corefile) {
	int value;

	int phy_id = NOTHING;
	int core_id = NOTHING;

	char word[MAXLENGTH];
	char line[MAXLENGTH];

	printf("hi\n");

	while (phy_id == NOTHING || core_id == NOTHING ) {
		if (fgets(line, (MAXLENGTH-1)*sizeof(char), corefile) == NULL) {
			return NULL;
		}
		sscanf(line, "%s: %d", word, &value);
		if (strcmp(word, "physical id") == 0) {
			phy_id = value;
		}
		else if (strcmp(word, "core id") == 0) {
			core_id = value;
		}
	}

	int* values = malloc(2*sizeof(int));
	values[0] = phy_id;
	values[1] = core_id;

	return values;
}


int getCoreAmount() {
	
	FILE* corefile = fopen("/proc/cpuinfo", "r");

	if (corefile == NULL) {
		perror("failed to open /proc/cpuinfo");
		exit(1);
	}

	CORES* cores = NULL;

	int* values = getCore(corefile);

	while (values != NULL) {
		int phy_id = values[0];
		int core_id = values[1];
		if (!coreExists(cores, phy_id, core_id)) {
			cores = addCore(cores, phy_id, core_id);
		}
		free(values);
		values = getCore(corefile);
	}

	int coreamount = 0;
	for (CORES* core = cores; core != NULL; core = core->next) {
		coreamount++;
	}

	int isClosed = fclose(corefile);
	if (isClosed != 0) {
		perror("failed to close /proc/cpuinfo");
		exit(1);
	}

	freeCores(cores);

	return coreamount;
}



int main() {
	printf("\n%d\n", getCoreAmount());

	/*

	FILE* corefile = fopen("/proc/cpuinfo", "r");

	int* v = getCore(corefile);
	printf("%d", v[0]);

	*/

	return 0;
}








