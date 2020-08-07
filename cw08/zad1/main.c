#include "common.h"

int main(int argc, char** argv) {
  if (argc != 5) {
    fprintf(stderr, "Wrong number of arguments!\n");
    return 1;
  }

  int threads_num = atoi(argv[1]);
  char* mode = argv[2];
  char* input_file_name = argv[3];
  char* result_file_name = argv[4];

  int** picture = read_file(input_file_name);

  struct timespec start;
  clock_gettime(CLOCK_REALTIME, &start);

  thread_info** infos =
      (thread_info**)calloc(threads_num, sizeof(thread_info*));

  pthread_t* threads = (pthread_t*)calloc(threads_num, sizeof(pthread_t));

  if (strcmp(mode, "block") == 0) {
    int step = width / threads_num;

    for (int i = 0; i < threads_num; i++) {
      infos[i] = (thread_info*)calloc(1, sizeof(thread_info));
      infos[i]->start_idx = i * step;
      infos[i]->end_idx = (i + 1) * step;
      infos[i]->picture = picture;
    }

    for (int i = 0; i < threads_num; i++) {
      if (pthread_create(&threads[i], NULL, thread_block, (void*)infos[i]) != 0)
        error("pthread_create");
    }
  } else if (strcmp(mode, "interleaved") == 0) {
    for (int i = 0; i < threads_num; i++) {
      infos[i] = (thread_info*)calloc(1, sizeof(thread_info));
      infos[i]->start_idx = i;
      infos[i]->step = threads_num;
      infos[i]->picture = picture;
    }

    for (int i = 0; i < threads_num; i++) {
      if (pthread_create(&threads[i], NULL, thread_interleaved,
                         (void*)infos[i]) != 0) {
        error("pthread_create");
      }
    }
  } else if (strcmp(mode, "sign") == 0) {
    int range = MAX / threads_num;

    for (int i = 0; i < threads_num; i++) {
      infos[i] = (thread_info*)calloc(1, sizeof(thread_info));
      infos[i]->min = i * range;
      infos[i]->max = (i + 1) * range;
      infos[i]->picture = picture;
    }

    for (int i = 0; i < threads_num; i++) {
      if (pthread_create(&threads[i], NULL, thread_sign, (void*)infos[i]) !=
          0) {
        error("pthread_create");
      }
    }
  } else {
    fprintf(stderr, "No such mode\n");
  }

  struct timespec** times =
      (struct timespec**)calloc(threads_num, sizeof(struct timespec*));

  void* retval;

  for (int i = 0; i < threads_num; i++) {
    pthread_join(threads[i], &retval);

    struct timespec* tm = (struct timespec*)retval;
    printf("%ld: %lds %ldms\n", threads[i], tm->tv_sec, tm->tv_nsec / 1000);

    for (int j = 0; j < MAX; j++) {
      histogram[j] += infos[i]->histogram[j];
    }

    times[i] = tm;
    free(infos[i]);
  }

  struct timespec end;
  clock_gettime(CLOCK_REALTIME, &end);

  struct timespec* total_time = time_diff(start, end);

  log_to_file("Times.txt", mode, threads_num, total_time, threads, times);

  save_histogram(result_file_name, histogram);

  for (int i = 0; i < threads_num; i++) {
    free(times[i]);
  }
  free(times);

  free(total_time);

  for (int i = 0; i < height; i++) {
    free(picture[i]);
  }
  free(picture);

  free(threads);
  free(infos);

  return 0;
}