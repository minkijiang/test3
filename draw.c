#include "draw.h"

#define ENDHEIGHT 56

void reloaddisplay() {
	printf("\x1b[%d;%df\n", ENDHEIGHT, 1);
}


void reposition(int x, int y) {
	//x is row y is height
	printf("\x1b[%d;%df", y, x);
}

void printchar(int x, int y, char c) {
	//x is row y is height
	printf("\x1b[%d;%df%c", y, x, c);
}

void removechar(int x, int y) {
	printf("\x1b[%d;%df%c", y, x, EMPTY);
}

void clear(int x, int y) {
	//x is row y is height
	printf("\x1b[%d;%df\033[J", y, x); // reposition: \x1b[%d;%df clearscreen: \033[J
}