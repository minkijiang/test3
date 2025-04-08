#include "getArguments.h"
#include "signalhandler.h"
#include "draw.h"
#include <sys/wait.h>

#include <unistd.h>

#define STARTHEIGHT 3
#define ENDHEIGHT 46

#define MICROSEC_TO_SEC 1000000

void waitforchild() {
	int exitstatus;
	for (int i = 0; i < 2; i++) {    //there should be exactly 3 child processes
		if (wait(&exitstatus) < 0) {
			perror("failed to wait for child");
			exit(1);
		}
		if (WIFEXITED(exitstatus) && WEXITSTATUS(exitstatus) != 0) {
			perror("failed to retrieve system information");
			exit(1);
		}
	}
}

void printHeader(int samples, int tdelay) {
	reposition(1, STARTHEIGHT);
	printf("Number of samples: %d  --  every %d microSecs ( %.3f secs )", samples, tdelay, ((double)(tdelay)/MICROSEC_TO_SEC));
}

int getmax(int x, int y, int z) {
	int max = 0;
	if (x > max) {
		max = x;
	}
	if (y > max) {
		max = y;
	}
	if (z > max) {
		max = z;
	}

	return max;

}

//extra stuff i might not use


void printsummary(int mempipe, int cpupipe, int corepipe) {
	fd_set readfds;
	FD_ZERO(&readfds);

    int maxfd = getmax(mempipe, cpupipe, corepipe); 
    FD_SET(mempipe, &readfds);
	FD_SET(cpupipe, &readfds);
	FD_SET(corepipe, &readfds);

    float memavg;
    float cpuavg;
    float coreamount;
    float maxghz;

    while (mempipe != NOTHING || cpupipe != NOTHING || corepipe != NOTHING) {
	    if (select(maxfd, &readfds, NULL, NULL, NULL) == -1) {
	    	perror("failed to select");
	    	exit(1);
	    }
	    if (mempipe != NOTHING && FD_ISSET(mempipe, &readfds)) {
	    	if (read(mempipe, &memavg, sizeof(float)) < 0 ) { //assume only write to pipe once
	    		perror("failed to read pipe");
	    		exit(1);
	    	}
	    	waitforchild(); //assume child exits after write to pipe
	    	mempipe = NOTHING;
	    }
	    if (cpupipe != NOTHING && FD_ISSET(cpupipe, &readfds)) {
	    	if (read(cpupipe, &cpuavg, sizeof(float)) < 0 ) { //assume only write to pipe once
	    		perror("failed to read pipe");
	    		exit(1);
	    	}
	    	waitforchild(); //assume child exits after write to pipe
	    	cpupipe = NOTHING;
	    }
	    if (corepipe != NOTHING && FD_ISSET(corepipe, &readfds)) {
	    	int values[2];
	    	if (read(corepipe, values, 2*sizeof(float)) < 0 ) { //assume only write to pipe once
	    		perror("failed to read pipe");
	    		exit(1);
	    	}
	    	coreamount = values[0];
	    	maxghz = values[1];

	    	waitforchild(); //assume child exits after write to pipe
	    	corepipe = NOTHING;
	    }
	}

	reposition(1, ENDHEIGHT);
	printf("Summary: \n\n Memory Usage: %.2f\n Cpu Usage: %.2f\n Core Amount: %.2f\n Maximum GHZ: %.2f\n ", memavg, cpuavg, coreamount, maxghz);
}



int main(int argc, char** argv) {

	clear(1, 1);

	int pid = fork();

	MONITORINFO* monitorinfo = createMonitorInfo();
	processArguments(monitorinfo, argc, argv);

	if (pid == 0) {
		execl("monitorCores", "monitorCores");
	}

	/*

	clear(1, 1); //clear entire screen

	//char stdoutfile[MAXLENGTH];
	//readlink("/proc/self/fd/1", stdoutfile, sizeof(path) - 1);

	MONITORINFO* monitorinfo = createMonitorInfo();
	processArguments(monitorinfo, argc, argv);

	int mempipe[2];
	int cpupipe[2];
	int corepipe[2];

	if (pipe(mempipe) != 0 || pipe(cpupipe) != 0 || pipe(corepipe) != 0) {
		perror("failed to pipe");
		exit(1);
	}

	int pids[3];

	if (monitorinfo->showMem) {

		pids[0] = fork();

		if (pids[0] == 0) {
			if (close(mempipe[0]) != 0 ) {
				perror("failed to close pipe");
				exit(1);
			}

			if (close(cpupipe[0]) != 0 || close(cpupipe[1]) != 0 || close(corepipe[0]) != 0 || close(corepipe[1]) != 0) {
				perror("failed to close pipe");
				exit(1);
			}

			execl("monitorMemory", "monitorMemory", monitorinfo->samplesize, monitorinfo->tdelay, mempipe[1]);
		}
		else if (pids[0] < 0) {
			perror("failed to fork");
			exit(1);
		}
		
	}
	else {
		pids[0] = NOTHING;
	}

	if (monitorinfo->showCpu) {

		pids[1] = fork();

		if (pids[1] == 0) {
			if (close(cpupipe[0]) != 0 ) {
				perror("failed to close pipe");
				exit(1);
			}

			if (close(mempipe[0]) != 0 || close(mempipe[1]) != 0 || close(corepipe[0]) != 0 || close(corepipe[1]) != 0) {
				perror("failed to close pipe");
				exit(1);
			}

			execl("monitorCpu", "monitorCpu", monitorinfo->samplesize, monitorinfo->tdelay, cpupipe[1]);
		}
		else if (pids[1] < 0) {
			perror("failed to fork");
			exit(1);
		}
	}
	else {
		pids[1] = NOTHING;
	}

	if (monitorinfo->showCores) {
	
		pids[2] = fork();

		if (pids[2] == 0) {
			if (close(corepipe[0]) != 0 ) {
				perror("failed to close pipe");
				exit(1);
			}

			if (close(mempipe[0]) != 0 || close(mempipe[1]) != 0 || close(cpupipe[0]) != 0 || close(cpupipe[1]) != 0) {
				perror("failed to close pipe");
				exit(1);
			}

			execl("monitorCores", "monitorCores", corepipe[1]);
		}
		else if (pids[2] < 0) {
			perror("failed to fork");
			exit(1);
		}
	}
	else {
		pids[2] = NOTHING;
	}

	if (close(corepipe[1]) != 0 || close(cpupipe[1]) != 0 || close(mempipe[1]) != 0 ) {
		perror("failed to close pipe");
		exit(1);
	}

	setSignals();
	setpgid(pids[0], getpgrp());
	setpgid(pids[1], getpgrp());
	setpgid(pids[2], getpgrp());

	printsummary(mempipe[0], cpupipe[0], corepipe[0]);

	free(monitorinfo);

	*/

	return 0;
}



//. gcc testmem.c /Users/minkijiang/Documents/UTSC_y2_proj/CSCB09/A3/a3/graph.c


// cd /cmshome/jiangm95/cscb09w25_space/projects

//.  cd /cmshome/jiangm95/cscb09w25_space/projects

//. gcc -Wall -Werror --std=c99 stuff.c

//.  ssh jiangm95@it-ia3170-03.utsc-labs.utoronto.ca 

// gcc a2.c -lm -fsanitize=address









