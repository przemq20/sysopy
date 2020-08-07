#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int maxChairs;
int currClientID = -1;
int clients = 0;

int takenChairs = 0;
pthread_mutex_t chairs_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t chairs_cond = PTHREAD_COND_INITIALIZER;

int isBarberSleeping = 0;
pthread_mutex_t sleeping_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sleeping_cond = PTHREAD_COND_INITIALIZER;

int barberIsFree = 1;
pthread_mutex_t free_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t free_cond = PTHREAD_COND_INITIALIZER;

int res;

void error(char *msg) {
  perror(msg);
  exit(1);
}

void *barber(void *args) {
  int chairsNum1;

  while (1) {
    res = pthread_mutex_lock(&chairs_mutex);
    if (res != 0) {
      error("mutex lock");
    }
    if (takenChairs == 0) {
      chairsNum1 = takenChairs;
      res = pthread_mutex_unlock(&chairs_mutex);
      if (res != 0) {
        error("mutex unlock");
      }
      res = pthread_mutex_lock(&sleeping_mutex);
      if (res != 0) {
        error("mutex lock");
      }
      printf("Golibroda: idę spać\n");
      isBarberSleeping = 1;

      while (isBarberSleeping) {
        pthread_cond_wait(&sleeping_cond, &sleeping_mutex);
      }

      res = pthread_mutex_unlock(&sleeping_mutex);
      if (res != 0) {
        error("mutex unlock");
      }
    } else {
      takenChairs--;
      chairsNum1 = takenChairs;
      res = pthread_mutex_unlock(&chairs_mutex);
      if (res != 0) {
        error("mutex unlock");
      }
    }

    printf("Golibroda: czeka %d klientów, gole klienta %d\n", chairsNum1,
           currClientID);
    sleep(rand() % 5 + 1);

    res = pthread_mutex_lock(&free_mutex);
    if (res != 0) {
      error("mutex lock");
    }
    barberIsFree = 1;
    pthread_cond_signal(&free_cond);

    res = pthread_mutex_unlock(&free_mutex);
    if (res != 0) {
      error("mutex unlock");
    }
  }

  return NULL;
}

void *client(void *arg) {
  int myID = *((int *)arg);
  int isBarberAwake = 0;

  res = pthread_mutex_lock(&sleeping_mutex);
  if (res != 0) {
    error("mutex lock");
  }
  if (isBarberSleeping) {
    printf("%d: budzę golibrodę\n", myID);
    isBarberAwake = 1;

    isBarberSleeping = 0;
    currClientID = myID;
    pthread_cond_signal(&sleeping_cond);
  }

  res = pthread_mutex_unlock(&sleeping_mutex);
  if (res != 0) {
    error("mutex unlock");
  }
  if (!isBarberAwake) {
    while (1) {
      res = pthread_mutex_lock(&chairs_mutex);
      if (res != 0) {
        error("mutex lock");
      }
      if (takenChairs == maxChairs) {
        res = pthread_mutex_unlock(&chairs_mutex);
        if (res != 0) {
          error("mutex unlock");
        }
        printf("%d: Zajęte\n", myID);
        sleep(rand() % 3 + 1);
      } else {
        takenChairs++;
        printf("%d: Poczekalnia, wolne miejsca - %d\n", myID,
               (maxChairs - takenChairs));
        res = pthread_mutex_unlock(&chairs_mutex);
        if (res != 0) {
          error("mutex unlock");
        }
        break;
      }
    }

    res = pthread_mutex_lock(&free_mutex);
    if (res != 0) {
      error("mutex lock");
    }
    while (!barberIsFree) {
      pthread_cond_wait(&free_cond, &free_mutex);
    }
    barberIsFree = 0;

    currClientID = myID;

    res = pthread_mutex_unlock(&free_mutex);
    if (res != 0) {
      error("mutex unlock");
    }
  }

  return NULL;
}
int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Wrong number of argumnets.\n");
    return 1;
  }

  maxChairs = atoi(argv[1]);
  int clients = atoi(argv[2]);

  pthread_t barberID;
  res = pthread_create(&barberID, NULL, barber, NULL);
  if (res != 0) {
    error("pthread create");
  }
  pthread_t clientsID[clients];

  for (int i = 0; i < clients; i++) {
    res = pthread_create(&clientsID[i], NULL, client, (void *)&i);
    if (res != 0) {
      error("pthread create");
    }
    sleep(rand() % 2 + 1);
  }

  for (int i = 0; i < clients; i++) {
    res = pthread_join(clientsID[i], NULL);
    if (res != 0) {
      error("pthread join");
    }
  }

  sleep(5);
  res = pthread_cancel(barberID);
  if (res != 0) {
    error("pthread cancel");
  }
  return 0;
}