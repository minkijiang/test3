
#include "setsignal.h"
#include "signalhandler.h"
#include <unistd.h>
#include <sys/wait.h>

void terminate(int signo) {
	wait(NULL);
	exit(0);
}

void stop(int signo) {
	int sig = SIGCONT + 1; //set sig != SIGCONT

	while (sig != SIGCONT) {

		sigset_t mask;
		sigemptyset(&mask);
		sigaddset(&mask, SIGCONT);

        sigwait(&mask, &sig);

    }
}

void setSignals() {
	setSignalHandlers(stop, terminate);
}
