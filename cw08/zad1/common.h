#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX 256
#define BUFF_SIZE 70

int histogram[MAX];
int width, height, max_val;

typedef struct thread_info {
  int** picture;
  int histogram[MAX];
  int start_idx;
  int end_idx;
  int step;
  int min;
  int max;

} thread_info;

void error(char* msg) {
  perror(msg);
  exit(1);
}

int** read_file(char* file_name) {
  char ch;
  int val;

  FILE* fp = fopen(file_name, "r");
  if (fp == NULL)
    error("Can't open file");

  ch = getc(fp);
  if (ch != 'P')
    error("Wrong format of file");

  ch = getc(fp);
  if (ch != '2')
    error("Wrong format of file");

  while (getc(fp) != '\n')
    ;

  while (getc(fp) == '#') {
    while (getc(fp) != '\n')
      ;
  }

  fseek(fp, -1, SEEK_CUR);

  fscanf(fp, "%d %d", &width, &height);
  fscanf(fp, "%d", &max_val);

  printf("%d  %d\n", width, height);

  int** picture = (int**)calloc(height, sizeof(int*));
  for (int i = 0; i < height; i++) {
    picture[i] = (int*)calloc(width, sizeof(int));
  }

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      fscanf(fp, "%d", &val);
      picture[row][col] = val;
    }
  }

  fclose(fp);

  return picture;
}

struct timespec* time_diff(struct timespec start, struct timespec end) {
  struct timespec* diff = (struct timespec*)calloc(1, sizeof(struct timespec));
  diff->tv_sec = end.tv_sec - start.tv_sec;
  diff->tv_nsec = end.tv_nsec - start.tv_nsec;

  if (start.tv_nsec > end.tv_nsec) {
    diff->tv_sec--;
    diff->tv_nsec = -diff->tv_nsec;
  }

  return diff;
}

void* thread_block(void* info) {
  thread_info* t_info = (thread_info*)info;

  struct timespec start;
  clock_gettime(CLOCK_REALTIME, &start);

  for (int row = 0; row < height; row++) {
    for (int col = t_info->start_idx; col < t_info->end_idx; col++) {
      t_info->histogram[t_info->picture[row][col]]++;
    }
  }

  struct timespec end;
  clock_gettime(CLOCK_REALTIME, &end);

  struct timespec* tm = time_diff(start, end);

  pthread_exit((void*)tm);
}

void* thread_interleaved(void* info) {
  thread_info* t_info = (thread_info*)info;

  struct timespec start;
  clock_gettime(CLOCK_REALTIME, &start);

  for (int row = 0; row < height; row++) {
    for (int col = t_info->start_idx; col < width; col += t_info->step) {
      t_info->histogram[t_info->picture[row][col]]++;
    }
  }

  struct timespec end;
  clock_gettime(CLOCK_REALTIME, &end);

  struct timespec* tm = time_diff(start, end);

  pthread_exit((void*)tm);
}

void* thread_sign(void* info) {
  thread_info* t_info = (thread_info*)info;

  struct timespec start;
  clock_gettime(CLOCK_REALTIME, &start);

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      if (t_info->picture[row][col] >= t_info->min &&
          t_info->picture[row][col] < t_info->max)
        t_info->histogram[t_info->picture[row][col]]++;
    }
  }

  struct timespec end;
  clock_gettime(CLOCK_REALTIME, &end);

  struct timespec* tm = time_diff(start, end);

  pthread_exit((void*)tm);
}

void log_to_file(char* file_name,
                 char* mode,
                 int threads_num,
                 struct timespec* total_time,
                 pthread_t* threads,
                 struct timespec** times) {
  FILE* fp = fopen(file_name, "a");
  if (fp == NULL)
    error("fopen log file");

  fprintf(fp, "\n####### %s -> %d threads #######\n", mode, threads_num);
  fprintf(fp, "total time: %lds %ldms\n", total_time->tv_sec,
          total_time->tv_nsec / 1000);

  for (int i = 0; i < threads_num; i++) {
    fprintf(fp, "thread %ld: %lds %ldms\n", threads[i], times[i]->tv_sec,
            times[i]->tv_nsec / 1000);
  }

  fclose(fp);
}

void save_histogram(char* file_name, int* histogram) {
  FILE* fp = fopen(file_name, "w");

  for (int i = 0; i < MAX; i++) {
    fprintf(fp, "%d - %d\n", i, histogram[i]);
  }

  fclose(fp);
}