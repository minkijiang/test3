#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define MICROSEC_TO_SEC 1000000

#define MAXLENGTH 1024
#define NOTHING -1
#define ROWAMOUNT 10
#define GRAPHCHAR '#'

void delay(int tdelay);

void freeGraph(char** graph);

char** initialize_graph(int samplesize);

void updateGraph(char** graph, int currentsample, float value, float maxvalue);