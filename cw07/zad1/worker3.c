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
  /* setting SIGUSR1 handler and accessing shared memory objects and semaphore
   */
  signal(SIGUSR1, handler);

  arrayId = accessArray();
  counterId = accessCounter();
  semaphoreId = accessSemaphore();

  shopOrders = getArray(arrayId);
  counter = getCounter(counterId);

  while (1 == 1) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(lrint(tv.tv_usec));

    usleep(1000 * (rand() % 1000 + 1000));

    decrement(semaphoreId);

    if (counter->packed_orders > 0) {
      arrayIndex = firstPacked(arrayIndex, shopOrders);

      shopOrders[arrayIndex].order_number *= 3;
      shopOrders[arrayIndex].is_packed = 0;

      counter->packed_orders--;

      char* time = get_time();

      printf(
          "(%d %s) Wysłałem zamówienie o wielkości %d, Liczba "
          "zamówień do przygotowania: %d, Liczba zamównień do wysłania: "
          "%d\n",
          getpid(), time, shopOrders[arrayIndex].order_number,
          counter->unpacked_orders, counter->packed_orders);

      arrayIndex = (arrayIndex + 1) % MAX_ARRAY_SIZE;
    }

    increment(semaphoreId);
  }
}