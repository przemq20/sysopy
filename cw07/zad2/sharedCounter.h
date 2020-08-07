#define _XOPEN_SOURCE 500
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define COUNTER_NAME "/counter"

struct OrderCounter {
  int packed_orders;
  int unpacked_orders;
} typedef OrderCounter;

int makeCounter();
int accessCounter();
OrderCounter* getCounter(int counterId);
void closeCounter(OrderCounter* counter);
void deleteCounter();