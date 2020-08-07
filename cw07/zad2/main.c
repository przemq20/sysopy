#include "semaphore.h"
#include "sharedArray.h"
#include "sharedCounter.h"

int WORKERS;
pid_t* pids;
sem_t* semaphoreId;
int counterId, arrayId;
ShopOrder* shopOrders;
OrderCounter* counter;
int arrayIndex = 0;

void handler(int signum) {
  for (int i = 0; i < WORKERS; i++)
    kill(pids[i], SIGUSR1);
}

int main(int argc, char* argv[]) {
  if (argc != 4) {
    printf("Wrong number of arguments!\n");
    return -1;
  }

  struct timeval tv;
  gettimeofday(&tv, NULL);
  srand(lrint(tv.tv_usec));

  arrayId = makeArray();
  counterId = makeCounter();
  semaphoreId = makeSemaphore();

  counter = getCounter(counterId);
  shopOrders = getArray(arrayId);

  counter->packed_orders = counter->unpacked_orders = 0;

  for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
    shopOrders[i].order_number = 0;
    shopOrders[i].is_packed = 0;
    shopOrders[i].is_waiting = 0;
  }

  int workers[] = {atoi(argv[1]), atoi(argv[2]), atoi(argv[3])};
  WORKERS = workers[0] + workers[1] + workers[2];
  pids = (pid_t*)calloc(WORKERS, sizeof(pid_t));

  int worker_num = 0;

  for (int i = 0; i < workers[0]; i++) {
    if ((pids[worker_num] = fork()) == 0) {
      worker_num++;
      execl("./worker1", "./worker1", NULL);
    }
  }

  for (int i = 0; i < workers[1]; i++) {
    if ((pids[worker_num] = fork()) == 0) {
      worker_num++;
      execl("./worker2", "./worker2", NULL);
    }
  }

  for (int i = 0; i < workers[2]; i++) {
    if ((pids[worker_num] = fork()) == 0) {
      worker_num++;
      execl("./worker3", "./worker3", NULL);
    }
  }

  signal(SIGINT, handler);

  for (int i = 0; i < WORKERS; i++) {
    waitpid(pids[i], NULL, 0);
  }

  closeArray(shopOrders);
  closeCounter(counter);
  closeSemaphore(semaphoreId);
  deleteArray();
  deleteCounter();
  deleteSemaphore();
  free(pids);

  return 0;
}