#include "graph.h"
#include "signalhandler.h"

#include <unistd.h>
#include <string.h>


typedef struct CPUINFO {
	float cpu_usage;
	float avg_usage;

	/*

	cpuUsageInfo[0] = previous total usage time
	cpuUsageInfo[1] = previous total idle time
	cpuUsageInfo[2] = current total usage time
	cpuUsageInfo[3] = current total idle time

	*/

	long long int cpuUsageInfo[4];

	GRAPHINFO* cpugraphinfo;

} CPUINFO;

#define STARTHEIGHT 20

long long int* getTotalCpuUsageInfo() {
	long long int* cpu_info = malloc(2*sizeof(long long int));

	long long int user; long long int nice; long long int sys; long long int idle; 
	long long int IOwait; long long int irq; long long int softirq;


	FILE* cpufile = fopen("/proc/stat", "r");

	if (cpufile == NULL) {
		perror("failed to open /proc/stat");
		exit(1);
	}

	char word[MAXLENGTH];
	char line[MAXLENGTH];

	while (fgets(line, (MAXLENGTH-1)*sizeof(char), cpufile) != NULL) {
		sscanf(line, "%s %lld %lld %lld %lld %lld %lld %lld", word, 
		&user, &nice, &sys, &idle, &IOwait, &irq, &softirq);
		if (strcmp(line, "cpu")==0) {
			break;
		}
	}

	cpu_info[0] = user+nice+sys+idle+IOwait+irq+softirq;
	cpu_info[1] = idle;

	int isClosed = fclose(cpufile);
	if (isClosed != 0) {
		perror("failed to close /proc/stat");
		exit(1);
	}

	return cpu_info;

}

CPUINFO* createCpuinfo(int samplesize) {
	CPUINFO* cpuinfo = malloc(sizeof(CPUINFO));

	if (cpuinfo == NULL) {
		perror("failed to malloc");
		exit(1);
	}

	cpuinfo->cpu_usage = 0;
	cpuinfo->avg_usage = 0;

	long long int* cinfo = getTotalCpuUsageInfo();
	cpuinfo->cpuUsageInfo[0] = cinfo[0];
	cpuinfo->cpuUsageInfo[1] = cinfo[1];
	cpuinfo->cpuUsageInfo[2] = cinfo[0];
	cpuinfo->cpuUsageInfo[3] = cinfo[1];

	char memtop[MAXLENGTH];
	char membottem[MAXLENGTH];
	strcpy(memtop, "100%%");
	strcpy(membottem, "0%%");
	cpuinfo->cpugraphinfo = createGraphInfo(samplesize, STARTHEIGHT, memtop, membottem);

	return cpuinfo;

}

void freecpuinfo(CPUINFO* cpuinfo) {
	freeGraph(cpuinfo->cpugraphinfo);
	free(cpuinfo->cpuUsageInfo);
	free(cpuinfo);
}

void retrieveCpuInfo(CPUINFO* cpuinfo, int currentsample, int fd) {
	cpuinfo->cpuUsageInfo[0] = cpuinfo->cpuUsageInfo[2];
	cpuinfo->cpuUsageInfo[1] = cpuinfo->cpuUsageInfo[3];

	long long int cpuinfos[2];
	if (read(fd, cpuinfos, 2*sizeof(long long int)) < 0) {
		perror("failed to read pipe");
		exit(1);
	}

	cpuinfo->cpuUsageInfo[2] = cpuinfos[0];
	cpuinfo->cpuUsageInfo[3] = cpuinfos[1];

	free(cpuinfo);

	float runtime = (float) (cpuinfo->cpuUsageInfo[2] - cpuinfo->cpuUsageInfo[0]);
	float idletime = (float) (cpuinfo->cpuUsageInfo[3] - cpuinfo->cpuUsageInfo[1]);
	float percent = ((runtime-idletime) / runtime)*100;

	cpuinfo->cpu_usage = percent;
	cpuinfo->cpugraphinfo->values[currentsample-1] = percent;
	cpuinfo->avg_usage = ((cpuinfo->avg_usage * (currentsample-1)) + percent) / currentsample;
}


void plotCpuGraph(CPUINFO* cpuinfo, int currentsample) {
	char header[MAXLENGTH];
	sprintf(header, "CPU %.2f %%", cpuinfo->avg_usage);
	updateHeader(cpuinfo->cpugraphinfo, header);

	plotgraph(cpuinfo->cpugraphinfo, currentsample, cpuinfo->cpu_usage, 100);
}

void printInitialCpuGraph(CPUINFO* cpuinfo) {
	char header[MAXLENGTH];
	sprintf(header, "CPU %.2f %%", cpuinfo->avg_usage);

	printInitialGraph(cpuinfo->cpugraphinfo, header);
}

/*

void test(CPUINFO* cpuinfo) {

	printf("\x1b[%d;%df", 14, 1);
	printf("CPU %.2f %%", cpuinfo->avg_usage);
	printInitialCpuGraph(cpuinfo);
	for (int i = 0; cpuinfo->cpugraphinfo->values[i] != NOTHING; i++) {
		plotgraph(cpuinfo->cpugraphinfo, i+1, cpuinfo->cpugraphinfo->values[i], 100);
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
		perror("failed to retrieve cpu information");
		exit(1);
	}
}



int main(int argc, char** argv) {

	//argc >= 4

	setSignals();

	int samplesize = strtol(argv[1],NULL, 10);
	int tdelay = strtol(argv[2],NULL, 10);
	//int cpupipe = strtol(argv[3],NULL, 10);

	int pipefile[2];

	if (pipe(pipefile) != 0) {
		perror("failed to pipe");
		exit(1);
	}

	CPUINFO* cpuinfo = createCpuinfo(samplesize);

	int pid = fork();

	if (pid == 0) {
		if (close(pipefile[0]) != 0) {
			perror("failed to close pipe");
			exit(1);
		}

		printInitialCpuGraph(cpuinfo);

		for (int sample = 0; sample < samplesize; sample++) {
			//delay(tdelay);
			usleep(tdelay-1000);
			long long int* cpuUsageInfo = getTotalCpuUsageInfo();
			if (write(pipefile[1], cpuUsageInfo, 2*sizeof(long long int)) < 0) {
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
		
		if (close(pipefile[1]) != 0) {
			perror("failed to close pipe");
			exit(1);
		}

		for (int sample = 1; sample < samplesize+1; sample++) {
			retrieveCpuInfo(cpuinfo, sample, pipefile[0]);
			plotCpuGraph(cpuinfo, sample);

		}

		waitforchild();

		if (close(pipefile[0]) != 0) {
			perror("failed to close pipe");
			exit(1);
		}

		/*

		if (write(cpupipe, &(cpuinfo->avg_usage), sizeof(float)) < 0) {
			perror("failed to write to pipe");
			exit(1);
		}
		if (close(cpupipe) != 0) {
			perror("failed to close pipe");
			exit(1);
		}

		*/
		
	}
	else {
		perror("failed to fork");
		exit(1);
	}

	freecpuinfo(cpuinfo);


	return 0;
}