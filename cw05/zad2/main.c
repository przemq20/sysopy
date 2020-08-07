#include <stdio.h>
#include <stdlib.h>

#define MAX 100

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Wrong amount of arguments\n");
		return 1;
	}
	FILE* fp = fopen(argv[1], "r");
	if (fp == NULL) {
		perror("Cannot open file");
		return 1;
	}
	FILE* sortInput = popen("sort", "w");
	char buff[MAX];

	while (fgets(buff, MAX, fp) != NULL) {
		fputs(buff, sortInput);
	}
	pclose(sortInput);
	return 0;
}