#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";


void generate(char* fileName, int linesCount, int bytesNumber) {

	FILE* fp;
	fp = fopen(fileName, "w");

	if (fp == NULL) {
		perror("Cannot open or create file to save generated records");
		exit(1);
	}

	srand(time(0));

	char* line;
	line = (char*)calloc(bytesNumber, sizeof(char));

	for (int i = 0; i < linesCount; i++) {
		for (int j = 0; j < bytesNumber - 1; j++) {
			int randNum = rand() % (int)(sizeof(charset) - 1);
			line[j] = charset[randNum];
		}
		line[bytesNumber - 1] = '\n';

		fwrite(line, sizeof(char), bytesNumber, fp);
		memset(line, 0, bytesNumber);

	}

	free(line);
	fclose(fp);
}

void print_time(struct tms* start) {
	struct tms end;
	times(&end);
	printf("user: %f, sys: %f\n\n",
		(double)(end.tms_utime - start->tms_utime) / sysconf(_SC_CLK_TCK),
		(double)(end.tms_stime - start->tms_stime) / sysconf(_SC_CLK_TCK));
}

void sys_copy(char* src_filename, char* dst_filename, int records_count,
	int buffer_size) {
	int src = open(src_filename, O_RDONLY);
	int dst = open(dst_filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

	char* buffer = calloc(buffer_size, sizeof(char));

	int read_bytes_count = 0, read_records_count = 0;
	do {
		read_bytes_count = read(src, buffer, buffer_size);

		write(dst, buffer, buffer_size);
		read_records_count++;
	} while (read_bytes_count == buffer_size &&
		read_records_count < records_count);

	close(src);
	close(dst);

	free(buffer);
}

void sys_swap(int a, int b, int file, int record_size) {
	char* a_content = calloc(record_size, sizeof(char));
	char* b_content = calloc(record_size, sizeof(char));

	lseek(file, a * record_size, SEEK_SET);
	read(file, a_content, record_size);
	lseek(file, b * record_size, SEEK_SET);
	read(file, b_content, record_size);

	lseek(file, a * record_size, SEEK_SET);
	write(file, b_content, record_size);
	lseek(file, b * record_size, SEEK_SET);
	write(file, a_content, record_size);

	free(a_content);
	free(b_content);
}

int sys_partition(int low, int high, int file, int record_size) {
	int pivot = high;
	char* pivot_content = NULL;

	int i = (low - 1);

	for (int j = low; j <= high - 1; j++) {
		if (pivot_content == NULL) {
			pivot_content = calloc(record_size, sizeof(char));
			lseek(file, pivot * record_size, SEEK_SET);
			read(file, pivot_content, record_size);
		}

		char* j_content = calloc(record_size, sizeof(char));
		lseek(file, j * record_size, SEEK_SET);
		read(file, j_content, record_size);

		if (strcmp(j_content, pivot_content) < 0) {
			free(pivot_content);
			pivot_content = NULL;

			free(j_content);
			j_content = NULL;

			i++;
			sys_swap(i, j, file, record_size);
		}

		if (pivot_content != NULL) {
			free(pivot_content);
			pivot_content = NULL;
		}

		if (j_content != NULL) {
			free(j_content);
			j_content = NULL;
		}
	}
	sys_swap(i + 1, high, file, record_size);
	return i + 1;
}

void sys_quicksort(int low, int high, int file, int record_size) {
	if (low < high) {
		int pi = sys_partition(low, high, file, record_size);

		sys_quicksort(low, pi - 1, file, record_size);
		sys_quicksort(pi + 1, high, file, record_size);
	}
}

void sys_sort(char* filename, int records_count, int record_size) {
	int file = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	sys_quicksort(0, records_count - 1, file, record_size);
	close(file);
}

void lib_copy(char* src_filename, char* dst_filename, int records_count,
	int buffer_size) {
	FILE* src = fopen(src_filename, "r");
	FILE* dst = fopen(dst_filename, "w");

	char* buffer = calloc(buffer_size, sizeof(char));

	int read_bytes_count = 0, read_records_count = 0;
	do {
		read_bytes_count = fread(buffer, 1, buffer_size, src);

		fwrite(buffer, 1, buffer_size, dst);
		read_records_count++;
	} while (read_bytes_count == buffer_size &&
		read_records_count < records_count);

	fclose(src);
	fclose(dst);

	free(buffer);
}

void lib_swap(int a, int b, FILE* file, int record_size) {
	char* a_content = calloc(record_size, sizeof(char));
	char* b_content = calloc(record_size, sizeof(char));

	fseek(file, a * record_size, 0);
	fread(a_content, 1, record_size, file);
	fseek(file, b * record_size, 0);
	fread(b_content, 1, record_size, file);

	fseek(file, a * record_size, 0);
	fwrite(b_content, 1, record_size, file);
	fseek(file, b * record_size, 0);
	fwrite(a_content, 1, record_size, file);

	free(a_content);
	free(b_content);
}

int lib_partition(int low, int high, FILE* file, int bytesNumber) {
	int pivot = high;
	char* pivot_content = NULL;

	int i = (low - 1);

	for (int j = low; j <= high - 1; j++) {
		if (pivot_content == NULL) {
			pivot_content = calloc(bytesNumber, sizeof(char));
			fseek(file, pivot * bytesNumber, 0);
			fread(pivot_content, 1, bytesNumber, file);
		}

		char* j_content = calloc(bytesNumber, sizeof(char));
		fseek(file, j * bytesNumber, 0);
		fread(j_content, 1, bytesNumber, file);

		if (strcmp(j_content, pivot_content) < 0) {
			free(pivot_content);
			pivot_content = NULL;

			free(j_content);
			j_content = NULL;

			i++;
			lib_swap(i, j, file, bytesNumber);
		}

		if (pivot_content != NULL) {
			free(pivot_content);
			pivot_content = NULL;
		}

		if (j_content != NULL) {
			free(j_content);
			j_content = NULL;
		}
	}
	lib_swap(i + 1, high, file, bytesNumber);
	return i + 1;
}

void lib_quicksort(int low, int high, FILE* file, int record_size) {
	if (low < high) {
		int pi = lib_partition(low, high, file, record_size);

		lib_quicksort(low, pi - 1, file, record_size);
		lib_quicksort(pi + 1, high, file, record_size);
	}
}

void lib_sort(char* filename, int linesCount, int record_size) {
	FILE* file = fopen(filename, "rwb+");
	lib_quicksort(0, linesCount - 1, file, record_size);
	fclose(file);
}


int main(int argc, char* args[]) {
	struct tms cmd_start;

	for (int i = 1; i < argc; i++) {
		times(&cmd_start);

		char* cmd = args[i];

		if (strcmp(cmd, "generate") == 0) {
			char* filename = args[i + 1];
			int records_count = atoi(args[i + 2]);
			int record_size = atoi(args[i + 3]);
			i += 3;

			generate(filename, records_count, record_size);
		}
		else if (strcmp(cmd, "sort") == 0) {
			char* filename = args[i + 1];
			int records_count = atoi(args[i + 2]);
			int record_size = atoi(args[i + 3]);
			char* type = args[i + 4];
			i += 4;

			if (strcmp(type, "sys") == 0) {
				sys_sort(filename, records_count, record_size);
			}
			else {
				lib_sort(filename, records_count, record_size);
			}

			printf("sort %s %d %d %s:\n", filename, records_count, record_size,
				type);
			print_time(&cmd_start);
		}
		else if (strcmp(cmd, "copy") == 0) {
			char* src_filename = args[i + 1];
			char* dst_filename = args[i + 2];
			int records_count = atoi(args[i + 3]);
			int record_size = atoi(args[i + 4]);
			char* type = args[i + 5];
			i += 5;

			if (strcmp(type, "sys") == 0) {
				sys_copy(src_filename, dst_filename, records_count,
					record_size);
			}
			else {
				lib_copy(src_filename, dst_filename, records_count,
					record_size);
			}

			printf("copy %s %s %d %d %s:\n", src_filename, dst_filename,
				records_count, record_size, type);
			print_time(&cmd_start);
		}
		else {
			fprintf(stderr, "Invalid command: %s", cmd);
			return 1;
		}
	}

	return 0;
}