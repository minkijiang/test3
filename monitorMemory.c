
#include "graph.h"
#include "signalhandler.h"

#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>



#define KB_TO_GB 1000000

typedef struct MEMINFO {
	float max_mem;
	float mem_usage;
	float avg_usage;

	GRAPHINFO* memgraphinfo;

} MEMINFO;

#define STARTHEIGHT 7


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
	//meminfo->max_mem = (float)((int)(meminfo->max_mem * 100)) / 100;

	meminfo->mem_usage = 0;
	meminfo->avg_usage = 0;

	char memtop[MAXLENGTH];
	char membottem[MAXLENGTH];
	sprintf(memtop, "%.2f GB", meminfo->max_mem); 
	strcpy(membottem, "0 GB");
	meminfo->memgraphinfo = createGraphInfo(samplesize, STARTHEIGHT, memtop, membottem);

	return meminfo;
}

void freememinfo(MEMINFO* meminfo) {
	freeGraph(meminfo->memgraphinfo);
	free(meminfo);
};

void retrieveMemoryInfo(MEMINFO* meminfo, int currentsample, int fd) {
	float memusage;
	if (read(fd, &memusage, sizeof(float)) < 0) {
		perror("failed to read to pipe");
		exit(1);
	}

	meminfo->mem_usage = memusage;
	meminfo->memgraphinfo->values[currentsample-1] = memusage;
	meminfo->avg_usage = ((meminfo->avg_usage * (currentsample-1)) + memusage) / currentsample;
}

void plotMemoryGraph(MEMINFO* meminfo, int currentsample) {
	char header[MAXLENGTH];
	sprintf(header, "Memory: %.2f GB", meminfo->avg_usage);
	updateHeader(meminfo->memgraphinfo, header);

	plotgraph(meminfo->memgraphinfo, currentsample, meminfo->mem_usage, meminfo->max_mem);

}

void printInitialMemoryGraph(MEMINFO* meminfo) {
	char header[MAXLENGTH];
	sprintf(header, "Memory: %.2f GB", meminfo->avg_usage);

	printInitialGraph(meminfo->memgraphinfo, header);
}

/*

void test(MEMINFO* meminfo) {

	printf("\x1b[%d;%df", 1, 1);
	printf("Memory: %.2f GB", meminfo->avg_usage);
	printInitialMemoryGraph(meminfo);
	for (int i = 0; meminfo->memgraphinfo->values[i] != NOTHING; i++) {
		plotgraph(meminfo->memgraphinfo, i+1, meminfo->memgraphinfo->values[i], meminfo->max_mem);
	}
}

*/

void waitforchild() {
	int exitstatus;
	if (wait(&exitstatus) < 0) {
		perror("failed to wait for child");
		exit(1);
	}
	if (WIFEXITED(exitstatus) && WEXITSTATUS(exitstatus) != 0) {
		perror("failed to retrieve core information");
		exit(1);
	}
}



int main(int argc, char** argv) {

	//argc >= 4

	setSignals();

	int samplesize = strtol(argv[1],NULL, 10);
	int tdelay = strtol(argv[2],NULL, 10);
	int mempipe = strtol(argv[3],NULL, 10);

	int pipefile[2];

	if (pipe(pipefile) != 0) {
		perror("failed to pipe");
		exit(1);
	}

	MEMINFO* meminfo = createMeminfo(samplesize);

	int pid = fork();

	if (pid == 0) {

		if (close(pipefile[0]) != 0) {
			perror("failed to close pipe");
			exit(1);
		}

		printInitialMemoryGraph(meminfo);
		for (int sample = 0; sample < samplesize; sample++) {
			usleep(tdelay-1000);
			float memusage = getMemoryUsage();
			if (write(pipefile[1], &memusage, sizeof(float)) < 0) {
				perror("failed to write to pipe");
				exit(1);
			}

		}

		if (close(pipefile[1]) != 0) {
			perror("failed to close pipe");
			exit(1);
		}

	}
	else if (pid > 0) {

		setpgid(pid, getpgrp());


		if (close(pipefile[1]) != 0) {
			perror("failed to close pipe");
			exit(1);
		}

		for (int sample = 1; sample < samplesize+1; sample++) {
			retrieveMemoryInfo(meminfo, sample, pipefile[0]);
			plotMemoryGraph(meminfo, sample);

		}

		waitforchild();

		if (close(pipefile[0]) != 0) {
			perror("failed to close pipe");
			exit(1);
		}

		

		if (write(mempipe, &(meminfo->avg_usage), sizeof(float)) < 0) {
			perror("failed to write to pipe");
			exit(1);
		}
		
		if (close(mempipe) != 0) {
			perror("failed to close pipe");
			exit(1);
		}


		

		
	}
	else {
		perror("failed to fork");
		exit(1);
	}

	freememinfo(meminfo);


	return 0;
}

//.  gcc draw.c graph.c childsignalhandler.c setsignal.c monitorMemory.c 






