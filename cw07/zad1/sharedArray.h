#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define ARRAY_KEY ftok(getenv("HOME"), 'A')
#define MAX_ARRAY_SIZE 10

struct ShopOrder {
  int order_number;
  int is_waiting;
  int is_packed;
} typedef ShopOrder;

int makeArray();
int accessArray();
ShopOrder* getArray(int arrayId);
void closeArray(ShopOrder* shopOrders);
void deleteArray(int arrayId);

int firstEmpty(int i, ShopOrder* shopOrders);
int firstUnpacked(int i, ShopOrder* shopOrders);
int firstPacked(int i, ShopOrder* shopOrders);

char* get_time();