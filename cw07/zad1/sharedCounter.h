#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define COUNTER_KEY ftok(getenv("HOME"), 'C')

struct OrderCounter {
  int packed_orders;
  int unpacked_orders;
} typedef OrderCounter;

int makeCounter();
int accessCounter();
OrderCounter* getCounter(int counterId);
void closeCounter(OrderCounter* counter);
void deleteCounter(int counterId);