
#include "graph.h"

void delay(int tdelay) {
	clock_t start_time = clock();
  	while ( ( (clock() - start_time) / CLOCKS_PER_SEC ) < ((float)tdelay/MICROSEC_TO_SEC) );
}

void freeGraph(char** graph) {
	for (int i = 0; i < ROWAMOUNT; i++) {
		free(graph[i]);
	}
	free(graph);
}

char** initialize_graph(int samplesize) {
	char** datagraph = (char**) malloc((ROWAMOUNT+1)*sizeof(char*));

	for (int i = 0; i < ROWAMOUNT+1; i++) {
		*(datagraph+i) = malloc((samplesize)*sizeof(char));
	}

	for (int i = 0; i < ROWAMOUNT; i++) {
		for (int k = 0; k < samplesize; k++) {
			*(*(datagraph+i)+k) = ' ';
		}
	}
	for (int k = 0; k < samplesize; k++) {
			*(*(datagraph+ROWAMOUNT)+k) = '_';
		}
	return datagraph;


}

void updateGraph(char** graph, int currentsample, float value, float maxvalue) {
	int column = ceil((value/maxvalue)*ROWAMOUNT);
	graph[column][currentsample] = GRAPHCHAR;
}