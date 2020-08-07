#include "library.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct blockArray* createArray(int size) {
	struct blockArray* result = calloc(1, sizeof(struct blockArray));
	result->blocks = calloc(size, sizeof(struct block*));
	result->index = 0;
	result->size = size;
	result->free_places = size;
	return result;
};


void doDiff(char* file1, char* file2) {
	system("touch tmp");
	char tmp[13 + strlen(file1) + strlen(file2)];
	strcpy(tmp, "diff ");
	strcat(tmp, file1);
	strcat(tmp, " ");
	strcat(tmp, file2);
	strcat(tmp, " > tmp");
	system(tmp);
};

int createBlock(struct blockArray* main_tab, char* file1, char* file2) {
	if (main_tab->free_places == 0) {
		puts("No free places in main array");
		return -1;
	}
	doDiff(file1, file2);
	FILE* f = fopen("tmp", "r");
	if (f == NULL)
	{
		perror("Cannot open file");
		return -1;
	}
	int i = -1;
	char** tmp = (char**)calloc(100000, sizeof(char*));
	char* line = NULL;
	size_t len = 0;
	while (getline(&line, &len, f) != -1) {
		if (strncmp(line, ">", 1) != 0 && strncmp(line, "<", 1) != 0 && strncmp(line, "-", 1) != 0) {
			i++;
			tmp[i] = calloc(100000, sizeof(char));
			strcpy(tmp[i], line);
		}
		else {
			strcat(tmp[i], line);
		}
	}

	struct block* new_block = calloc(1, sizeof(struct block));
	new_block->numberOperations = i + 1;
	new_block->operations = tmp;
	if (main_tab->index < main_tab->size) {
		main_tab->blocks[main_tab->index] = new_block;
		main_tab->index++;
		main_tab->free_places--;
		return main_tab->index - 1;
	}
	else {
		int index = 0;
		while (main_tab->blocks[index] != NULL) i++;
		main_tab->blocks[index] = new_block;
		main_tab->free_places--;
		return index;
	}

};

void deleteBlock(int index, struct blockArray* main_tab) {
	if (index > main_tab->index) {
		printf("delete element is forbidden at index %d from array", index);
	}
	else {
		free(main_tab->blocks[index]);
		main_tab->blocks[index] = NULL;
		main_tab->free_places++;
	}
}

void deleteOperation(int op_index, int bl_index, struct blockArray* main_tab) {
	if (bl_index > main_tab->index) {
		printf("delete element is forbidden at index %d from array", bl_index);
	}
	else {
		struct block* bl = main_tab->blocks[bl_index];
		if (bl == NULL) {
			puts("block doesn't exists");
			return;
		}
		if (op_index >= bl->numberOperations) {
			printf("delete element at index %d is forbidden from array of size %d", op_index, bl->numberOperations);
		}
		else {
			free(bl->operations[op_index]);
			bl->numberOperations--;
			bl->operations[op_index] = NULL;
		}
	}
}

int getNumberOperations(struct block* bl) {
	return bl->numberOperations;
}

