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

    if (counter->unpacked_orders > 0) {
      arrayIndex = firstUnpacked(arrayIndex, shopOrders);

      shopOrders[arrayIndex].order_number *= 2;
      shopOrders[arrayIndex].is_waiting = 0;
      shopOrders[arrayIndex].is_packed = 1;

      counter->unpacked_orders--;
      counter->packed_orders++;

      char* time = get_time();

      printf(
          "(%d %s) Przygotowałem zamówienie o wielkości: %d, "
          "Liczba zamównień do przygotowania: %d, Liczba zamównień do "
          "wysłania: %d\n",
          getpid(), time, shopOrders[arrayIndex].order_number,
          counter->unpacked_orders, counter->packed_orders);

      arrayIndex = (arrayIndex + 1) % MAX_ARRAY_SIZE;
    }
    increment(semaphoreId);
  }
}