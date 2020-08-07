#ifndef SYSOPY_LAB1_LIB_H
#define SYSOPY_LAB1_LIB_H

struct block {
	int numberOperations;
	char** operations;
};

struct blockArray {
	struct block** blocks;
	int index;
	int size;
	int free_places;
};

struct blockArray* createArray(int size);

void doDiff(char* file1, char* file2);

int createBlock(struct blockArray* main_tab, char* file1, char* file2);

void deleteBlock(int index, struct blockArray* main_tab);

void deleteOperation(int operation_index, int block_index, struct blockArray* main_tab);

int getNumberOperations(struct block* bl);


#endif