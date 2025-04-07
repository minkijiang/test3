#include <string.h>
#include <math.h>
#include <time.h>

#include "graph.h"
#include "draw.h"

#define DRAWMODE DYNAMICDRAW

void delay(int tdelay) {
	clock_t start_time = clock();
  	while ( ( (clock() - start_time) / CLOCKS_PER_SEC ) < ((float)tdelay/MICROSEC_TO_SEC) );
}

GRAPHINFO* createGraphInfo(int samplesize, int startheight, char top[MAXLENGTH], char bottem[MAXLENGTH]) {
	GRAPHINFO* graphinfo = malloc(sizeof(GRAPHINFO));

	graphinfo->samplesize = samplesize;
	graphinfo->startheight = startheight;
	strcpy(graphinfo->top, top);
	strcpy(graphinfo->bottem, bottem);

	graphinfo->values = malloc(samplesize*sizeof(float));
	for (int i = 0; i < samplesize; i++) {
		graphinfo->values[i] = NOTHING;
	}

	return graphinfo;
}

void freeGraph(GRAPHINFO* graphinfo) {
	free(graphinfo->values);
	free(graphinfo);
}

void updateHeader(GRAPHINFO* graphinfo, char header[MAXLENGTH]) {
	reposition(1, graphinfo->startheight-2);
	printf("%s", header);
	reloaddisplay();
}

void printInitialGraph(GRAPHINFO* graphinfo, char header[MAXLENGTH]) {
	reposition(1, graphinfo->startheight-2);
	printf("%s", header);

	int leftsidelength = strlen(graphinfo->top)+1;
	if (strlen(graphinfo->bottem)+1 > leftsidelength) {
		leftsidelength = strlen(graphinfo->bottem)+1;
	}

	reposition(1, graphinfo->startheight);

	for (int i = 0; i < abs(leftsidelength-((int)strlen(graphinfo->top)+1)); i++ ) {
		printf(" ");
	}
	printf("%s |\n", graphinfo->top);

	for (int i = 0; i < ROWAMOUNT-1; i++) {
		for (int k = 0; k < leftsidelength; k++) {
			printf(" ");
		}
		printf("|");
		printf("\n");
	}

	for (int i = 0; i < abs(leftsidelength-((int)strlen(graphinfo->bottem)+1)); i++ ) {
		printf(" ");
	}
	printf("%s |", graphinfo->bottem);
	for (int i = 0; i < graphinfo->samplesize; i++ ) {
		printf("=");
	}

	reloaddisplay();


}

void plotgraph(GRAPHINFO* graphinfo, int currentsample, float value, float maxvalue) {

	// samplesize >= currentsample >= 1

	graphinfo->values[currentsample-1] = value;

	int y = graphinfo->startheight;
	y += (ROWAMOUNT - ceil((value/maxvalue)*ROWAMOUNT));

	int x = strlen(graphinfo->top)+2;
	if (strlen(graphinfo->bottem)+2 > x) {
		x = strlen(graphinfo->bottem)+2;
	}
	x += currentsample;


	if (DRAWMODE == DYNAMICDRAW) {
		printchar(x, y, NEU);

		if (currentsample > 1) {
			int y0 = graphinfo->startheight;
			y0 += (ROWAMOUNT - ceil(( graphinfo->values[currentsample-2] / maxvalue)*maxvalue));

			if (graphinfo->values[currentsample-2] > value) {
				removechar(x-1, y0);
				printchar(x-1, y0+1, DESC);
			}
			else if (graphinfo->values[currentsample-2] < value) {
				printf("%c", ASC);
				printchar(x-1, y0, ASC);
			}
		}
	}
	else if (DRAWMODE == SIMPLEDRAW) {
		printchar(x, y, PLOT);
	}

	//printchar(1, graphinfo->startheight+ROWAMOUNT+1, '\n');

	reloaddisplay();


}

// top htop (ps oux)