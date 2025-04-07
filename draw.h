#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY ' '

/*

flush stdout without using fflush

returns: nothing

*/

void reloaddisplay();

/*

precondition: x, y >= 1

position cursor to row x and height y

returns: nothing

*/

void reposition(int x, int y);

/*

precondition: x, y >= 1

print 'c' to positon (x,y)

returns: nothing

*/

void printchar(int x, int y, char c);

/*

precondition: x, y >= 1

remove a charcter in positon(x,y) if there is any

returns: nothing

*/

void removechar(int x, int y);

/*

precondition: x, y >= 1

clear everything that is below height 'y'

returns: nothing

*/

void clear(int x, int y);


