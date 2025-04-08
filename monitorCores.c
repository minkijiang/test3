#include "draw.h"
#include "signalhandler.h"

#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

#define STARTHEIGHT 33
#define MAXLENGTH 1024
#define NOTHING -1


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
	CORES* freecore = NULL;

	while (cores != NULL) {
		free(freecore);
		freecore = cores;
		cores = cores->next;
	}

}

CORES* addCore(CORES* cores, int phy_id, int core_id) {
	CORES* newcore = malloc(sizeof(CORES));
	newcore->phy_id = phy_id;
	newcore->core_id = core_id;
	newcore->next = cores;

	return newcore;
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
	char word2[MAXLENGTH];
	char line[MAXLENGTH];

	while (phy_id == NOTHING || core_id == NOTHING ) {
		if (fgets(line, (MAXLENGTH-1)*sizeof(char), corefile) == NULL) {
			return NULL;
		}

		sscanf(line, "%s %s : %d", word, word2, &value);

		if (strcmp(word, "physical") == 0 && strcmp(word2, "id") == 0) {
			phy_id = value;
		}
		else if (strcmp(word, "core") == 0 && strcmp(word2, "id") == 0) {
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

float getMaxGHZ() {

	FILE* ghzfile = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");

	if (ghzfile == NULL) {
		perror("failed to open /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq");
		exit(1);
	}

	int ghz;

	fscanf(ghzfile, "%d", &ghz);   //file content has only a number

	if (fclose(ghzfile) != 0) {
		perror("failed to close /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq");
		exit(1);
	}

	return (float)ghz/1000000;

}

COREINFO* getCoreInfo() {
	COREINFO* coreinfo = malloc(sizeof(COREINFO));
	coreinfo->maxghz = getMaxGHZ();
	coreinfo->coreamount = getCoreAmount();

	return coreinfo;
}

void displayCores(float maxghz, int coreamount) {

	int row_num = ceil(sqrt(coreamount));     // set row number to ceil(sqrt(coreamount)) to square it out

	printf("\n\n Number of Cores: %d @  %.2f GHZ", coreamount, maxghz);

	for (int i = 0; i < row_num; i++) {
		int cores_left = row_num;
		if (row_num > coreamount-(i*row_num)) {
			cores_left = coreamount-(i*row_num);
		}

		printf("%s", "\n ");
		for (int k = 0; k < cores_left; k++) {	
			printf("%s", "+--+ ");
		}
		printf("%s", "\n ");
		for (int k = 0; k < cores_left; k++) {
			printf("%s", "|  | ");	
		}
		printf("%s", "\n ");
		for (int k = 0; k < cores_left; k++) {
			printf("%s", "+--+ ");	

		}
	}

}


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

	// argc >= 2

	int corepipe = strtol(argv[1], NULL, 10);

	setSignals();

	int pipefile[2];
	pipe(pipefile);

	int pid = fork();

	if (pid == 0) {
		if (close(pipefile[0]) != 0) {
			perror("failed to close");
			exit(1);
		}

		COREINFO* coreinfo = getCoreInfo();
		if (write(pipefile[1], coreinfo, sizeof(COREINFO))  < 0) {
			perror("failed to write to pipe");
			exit(1);
		}

		if (close(pipefile[1]) != 0) {
			perror("failed to close");
			exit(1);
		}

		free(coreinfo);


	}
	else if (pid > 0) {
		if (close(pipefile[1]) != 0) {
			perror("failed to close");
			exit(1);
		}

		COREINFO* coreinfo = malloc(sizeof(COREINFO));
		if (read(pipefile[0], coreinfo, sizeof(coreinfo)) < 0) {
			perror("failed to read pipe");
			exit(1);
		}

		waitforchild();

		reposition(1, STARTHEIGHT);
		displayCores(coreinfo->maxghz, coreinfo->coreamount);
		reloaddisplay();


		if (close(pipefile[0]) != 0) {
			perror("failed to close");
			exit(1);
		}


		float values[2];
		values[0] = (float) coreinfo->coreamount;
		values[1] = coreinfo->maxghz;

		

		if (write(corepipe, values, 2*sizeof(float)) < 0) {
			perror("failed to write to pipe");
			exit(1);
		}

		if (close(corepipe) != 0) {
			perror("failed to close pipe");
			exit(1);
		}

		

		

		free(coreinfo);

		

	}
	else {
		perror("failed to fork");
		exit(1);
	}



	return 0;
}


//.  gcc draw.c graph.c childsignalhandler.c setsignal.c monitorCores.c 







