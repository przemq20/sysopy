#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define SEMAPHORE_NAME "/sem"

sem_t* makeSemaphore();
sem_t* accessSemaphore();
void closeSemaphore(sem_t* semaphoreId);
void deleteSemaphore();
void increment(sem_t* semaphoreId);
void decrement(sem_t* semaphoreId);