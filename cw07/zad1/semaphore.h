#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>

#define SEMAPHORE_KEY ftok(getenv("HOME"), 'S')

int makeSemaphore();
int accessSemaphore();
void deleteSemaphore(int semaphoreId);
void increment(int semaphoreId);
void decrement(int semaphoreId);