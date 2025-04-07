#include "setsignal.h"

void setSignalHandlers(void (*func_stop)(int), void (*func_terminate)(int)) {
	if (func_stop != NULL && signal(SIGTSTP, func_stop) == SIG_ERR) {
		perror("failed to set signal");
		exit(1);
	}

	if (func_terminate != NULL && signal(SIGINT, func_terminate) == SIG_ERR) {
		perror("failed to set signal");
		exit(1);
	}
}