#include "semaphore.h"
#include "sharedArray.h"
#include "sharedCounter.h"

int semaphoreId, counterId, arrayId;
ShopOrder* shopOrders;
OrderCounter* counter;
int arrayIndex = 0;

void handler(int signum) {
  closeArray(shopOrders);
  closeCounter(counter);
  exit(0);
}

int main() {
  signal(SIGUSR1, handler);

  arrayId = accessArray();
  counterId = accessCounter();
  semaphoreId = accessSemaphore();

  shopOrders = getArray(arrayId);
  counter = getCounter(counterId);

  while (1) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(lrint(tv.tv_usec));

    usleep(1000 * (rand() % 1000 + 1000));

    decrement(semaphoreId);

    int free =
        MAX_ARRAY_SIZE - counter->packed_orders - counter->unpacked_orders;

    if (free > 0) {
      arrayIndex = firstEmpty(arrayIndex, shopOrders);

      shopOrders[arrayIndex].order_number = 1 + (rand() % 10);
      shopOrders[arrayIndex].is_waiting = 1;
      shopOrders[arrayIndex].is_packed = 0;

      counter->unpacked_orders++;

      char* time = get_time();

      printf(
          "(%d %s) Dodałem liczbę: %d, Liczba zamównień do "
          "przygotowania: %d, Liczba zamównień do wysłania: %d\n",
          getpid(), time, shopOrders[arrayIndex].order_number,
          counter->unpacked_orders, counter->packed_orders);

      arrayIndex = (arrayIndex + 1) % MAX_ARRAY_SIZE;
    }

    increment(semaphoreId);
  }
}