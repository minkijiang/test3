#include "setsignal.h"
#include "signalhandler.h"
#include "draw.h"
#include <stdbool.h>
#include <unistd.h>

#define HEIGHT 46

void reset_input() {
	fseek(stdin,0,SEEK_END);
}

void terminate(int signo) {
	signal(SIGINT, SIG_IGN);
	
	kill(-getpgrp(), SIGINT);

	wait(NULL);

	clear(1, HEIGHT);
	reposition(1, HEIGHT+1);
	printf("Process Terminated");

	exit(0);
}

void stop(int signo) {
	signal(SIGTSTP, SIG_IGN);

	kill(-getpgrp(), SIGTSTP);

	signal(SIGTSTP, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	setSignalHandlers(stop, terminate);

	reposition(1, HEIGHT);
	printf("Process Stopped");

	reposition(1, HEIGHT+2);
	printf("Type \'y\' to continue, type \'n\' to exit \n\n");

	reset_input();

	bool isContinue = true;
	while (isContinue) {

		char input;
		scanf("%c", &input);

		if (input == 'y') {

			clear(1, HEIGHT);

			kill(-getpgrp(), SIGCONT);

			isContinue = false;
			return;
		}
		else if (input == 'n') {

			clear(1, HEIGHT);

			kill(-getpgrp(), SIGINT);
			wait(NULL);

			terminate(SIGINT);

		}
		else {
			clear(1, HEIGHT+2);
			reposition(1, HEIGHT+2);

			printf("Invalid input \n");
			printf("Type \'y\' to continue, type \'n\' to exit \n\n");
		}

	}

}

void setSignals() {
	setSignalHandlers(terminate, stop);
}
