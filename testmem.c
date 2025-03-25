#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "graph.h"

#define KB_TO_GB 1000000

typedef struct MEMINFO {
	float max_mem;
	//float mem_usage;
	float* mem_usage_arr;

	float avg_usage;

	char** memgraph;

} MEMINFO;


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


MEMINFO* createMeminfo(int samplesize) {
	MEMINFO* meminfo = malloc(sizeof(MEMINFO));

	if (meminfo == NULL) {
		perror("failed to malloc");
		exit(1);
	}

	meminfo->max_mem = getMaxMemory();
	//meminfo->mem_usage = 0;
	meminfo->mem_usage_arr = malloc(samplesize*sizeof(float));
	meminfo->avg_usage = 0;

	return meminfo;
}

void freememinfo(MEMINFO* meminfo, int samplesize) {
	free(meminfo->mem_usage_arr);
	freeGraph(meminfo->memgraph);
	free(meminfo);
}

void updateMemoryGraph(MEMINFO* meminfo, int currentsample, int fd) {
	float memusage;

	read(fd, &memusage, sizeof(float));

	//meminfo->avg_usage = ((meminfo->avg_usage * sample) + memusage) / (currentsample+1);

	updateGraph(meminfo->memgraph, currentsample, memusage, meminfo->max_mem);

	meminfo->mem_usage_arr[currentsample] = memusage;

	float sum = 0;
	for (int i = 0; i < currentsample + 1; i++) {
		sum += meminfo->mem_usage_arr[i];
	}
	meminfo->avg_usage = sum/(currentsample+1);

}

void displayMemory(MEMINFO* meminfo) {

	printf("Memory: %.2f GB", meminfo->avg_usage);

	for (int i = 0; i < ROWAMOUNT+1; i++) {
		if (i == 0) {
			printf("\n   %.2f GB |", meminfo->max_mem);
		}
		else if (i == ROWAMOUNT){
			printf("\n      0 GB |");
		}
		else {
			printf("\n           |");
		}

		printf("%s", meminfo->memgraph[i]);
	}
}

void writeMemoryGraph(MEMINFO* meminfo, int fd) {

	char line[MAXLENGTH];
	sprintf(line, "Memory: %.2f GB", meminfo->avg_usage);
	write(fd, line, MAXLENGTH*sizeof(char));

	for (int i = 0; i < ROWAMOUNT+1; i++) {
		if (i == 0) {
			sprintf(line, "\n   %.2f GB |", meminfo->max_mem);
		}
		else if (i == ROWAMOUNT) {
			sprintf(line, "\n      0 GB |");
		}
		else {
			sprintf(line, "\n           |");
		}

		strcat(line, meminfo->memgraph[i]);
		write(fd, line, MAXLENGTH*sizeof(char));
	}

}

int main(int argc, char** argv) {

	int samplesize = strtol(argv[1],NULL, 10);
	int tdelay = strtol(argv[2],NULL, 10);


	MEMINFO* meminfo = createMeminfo(samplesize);

	int pipefile[2];

	int isPiped = pipe(pipefile);
	if (isPiped != 0) {
		perror("failed to pipe");
		exit(1);
	}

	int pid = fork();

	if (pid == 0) {	
		int isClosed = close(pipefile[1]);
		if (isClosed != 0) {
			perror("failed to close pipe");
			exit(1);
		}

		for (int sample = 0; sample < samplesize; sample++) {
			updateMemoryGraph(meminfo, sample, pipefile[0]);
			displayMemory(meminfo);

		}

		isClosed = close(pipefile[0]);
		if (isClosed != 0) {
			perror("failed to close pipe");
			exit(1);
		}

	}
	else if (pid > 0) {

		int isClosed = close(pipefile[0]);
		if (isClosed != 0) {
			perror("failed to close pipe");
			exit(1);
		}

		for (int sample = 0; sample < samplesize; sample++) {
			delay(tdelay);
			float memusage = getMemoryUsage();
			write(pipefile[1], &memusage, sizeof(float));

		}

		isClosed = close(pipefile[1]);
		if (isClosed != 0) {
			perror("failed to close pipe");
			exit(1);
		}
	}
	else {
		perror("failed to fork");
		exit(1);
	}

	freememinfo(meminfo, samplesize);


	return 0;
}