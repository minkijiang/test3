
#include <stdio.h>
#include <stdlib.h>

#define MAXLENGTH 1024
#define NOTHING -1

typedef struct GRAPHINFO {
	int samplesize;
	int startheight;
	char top[MAXLENGTH];
	char bottem[MAXLENGTH];

	float* values;

}GRAPHINFO;


#define MICROSEC_TO_SEC 1000000
#define ROWAMOUNT 10

#define NEU '-'
#define ASC '/'
#define DESC '\\'
#define PLOT '#'

#define SIMPLEDRAW 1
#define DYNAMICDRAW 2

/*

waits for tdelay microseconds

*/

void delay(int tdelay);

/*

precondition: samplesize, startheight >= 1

return: GRAPHINFO struct with all the nessesary information

*/

GRAPHINFO* createGraphInfo(int samplesize, int startheight, char top[MAXLENGTH], char bottem[MAXLENGTH]);

/*

precondition: 'values' field is dynamically allocated

frees all dynamically aloocated fields

return: nothing

*/

void freeGraph(GRAPHINFO* graphinfo);

/*

change header display of graph to string of 'header'

return: nothing

*/



void updateHeader(GRAPHINFO* graphinfo, char header[MAXLENGTH]);

/*

print initial empty graph and put header 'header' on top

return: nothing

*/

void printInitialGraph(GRAPHINFO* graphinfo, char header[MAXLENGTH]);

/*

precondition: 1<= currentsample <= samplesize
			  0 <= value <= maxvalue

plot to graph

return: nothing

*/

void plotgraph(GRAPHINFO* graphinfo, int currentsample, float value, float maxvalue);

