float getMaxMemory() {


	FILE* memfile = fopen("/proc/meminfo", "r");

	if (memfile == NULL) {
		perror("failed to open /proc/meminfo");
		exit(1);
	}

	char line[MAXLENGTH];

	char word[MAXLENGTH];
	int maxmemory;

	while (fgets(line, (MAXLENGTH-1)*sizeof(char), memfile) != NULL) {
		sscanf(line, "%s %d" ,word, &maxmemory);
		if (strcmp(word, "MemTotal:") == 0) {
			break;
		}
	}

	int isClosed = fclose(memfile);
	if (isClosed != 0) {
		perror("failed to close /proc/meminfo");
		exit(1);
	}

	return (float)maxmemory/KB_TO_GB ;

	

	return 30;
}

float getMemoryUsage() {

	FILE* memfile = fopen("/proc/meminfo", "r");

	if (memfile == NULL) {
		perror("failed to open /proc/meminfo");
		exit(1);
	}

	char line[MAXLENGTH];

	char word[MAXLENGTH];
	int freememory;

	while (fgets(line, (MAXLENGTH-1)*sizeof(char), memfile) != NULL) {
		sscanf(line, "%s %d" ,word, &freememory);
		if (strcmp(word, "MemFree:") == 0) {
			break;
		}
	}

	int isClosed = fclose(memfile);
	if (isClosed != 0) {
		perror("failed to close /proc/meminfo");
		exit(1);
	}

	float maxmemory = getMaxMemory();

	return maxmemory - ((float)freememory/KB_TO_GB);

}

int main() {
	getMaxMemory();
	return 0;
}








