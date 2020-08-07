#define _XOPEN_SOURCE 500
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>   
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <ftw.h>
#include <stdint.h>

int mtime_flag = 0;
int atime_flag = 0;
int maxdepth_flag = 0;

int mtime_value = 0;
int mtime_sign = 0;

int atime_value = 0;
int atime_sign = 0;

int maxdepth_value = 0;

void operaten_on_time(char* parametr, int* time_value, int* time_sign) {
	if (parametr[0] == '+') {
		*time_sign = 1;
		char* buffor = (char*)calloc(100, sizeof(char));
		char c = parametr[1];
		int i = 0;
		while (c != '\0') {
			buffor[i] = c;
			c = parametr[i + 2];
			i++;
		}
		sscanf(buffor, "%d", time_value);
		free(buffor);
	}
	else if (parametr[0] == '-') {
		*time_sign = -1;
		char* buffor = (char*)calloc(100, sizeof(char));
		char c = parametr[1];
		int i = 0;
		while (c != '\0') {
			buffor[i] = c;
			c = parametr[i + 2];
			i++;
		}
		sscanf(buffor, "%d", time_value);
		free(buffor);
	}
	else {
		char* buffor = (char*)calloc(100, sizeof(char));
		char c = parametr[0];
		int i = 0;
		while (c != '\0') {
			buffor[i] = c;
			c = parametr[++i];
		}
		sscanf(buffor, "%d", time_value);
		free(buffor);
	}
}

void parse_command(char* command, char* follow_up, int* mtime_flag, int* atime_flag, int* maxdepth_flag, int* mtime_sign, int* atime_sign, int* mtime_value, int* atime_value, int* maxdepth_value) {

	if (strcmp(command, "-mtime") == 0) {
		*mtime_flag = 1;
		operaten_on_time(follow_up, mtime_value, mtime_sign);
	}
	else if (strcmp(command, "-atime") == 0) {
		*atime_flag = 1;
		operaten_on_time(follow_up, atime_value, atime_sign);
	}
	else if (strcmp(command, "-maxdepth") == 0) {
		*maxdepth_flag = 1;
		char* buffor = (char*)calloc(100, sizeof(char));
		char c = follow_up[0];
		int i = 0;
		while (c != '\0') {
			buffor[i] = c;
			c = follow_up[++i];
		}
		sscanf(buffor, "%d", maxdepth_value);
		free(buffor);
	}
}

void print_info(char* path, struct stat* statistics) {
	time_t seconds = time(NULL);
	printf("File path: %s\n", path);
	printf("Number of hard links: %ld\n", statistics->st_nlink);

	if (S_ISREG(statistics->st_mode)) {
		printf("File type: %s\n", "file");
	}
	else if (S_ISDIR(statistics->st_mode)) {
		printf("File type: %s\n", "dir");
	}
	else if (S_ISCHR(statistics->st_mode)) {
		printf("File type: %s\n", "char dev");
	}
	else if (S_ISBLK(statistics->st_mode)) {
		printf("File type: %s\n", "block dev");
	}
	else if (S_ISFIFO(statistics->st_mode)) {
		printf("File type: %s\n", "fifo");
	}
	else if (S_ISLNK(statistics->st_mode)) {
		printf("File type: %s\n", "slink");
	}
	else if (S_ISSOCK(statistics->st_mode)) {
		printf("File type: %s\n", "sock");
	}

	printf("Size: %ld\n", statistics->st_size);
	printf("Last access: %ld:%ld:%ld:%ld:%ld ago\n", (long int)(seconds - statistics->st_atime) / 2592000, (long int)(seconds - statistics->st_atime) / 86400, (long int)(seconds - statistics->st_atime) / 3600, (long int)((seconds - statistics->st_atime) / 60) % 60, (long int)(seconds - statistics->st_atime) % 60);
	printf("Last modification: %ld:%ld:%ld:%ld:%ld ago\n", (long int)(seconds - statistics->st_mtime) / 2592000, (long int)(seconds - statistics->st_mtime) / 86400, (long int)(seconds - statistics->st_mtime) / 3600, (long int)((seconds - statistics->st_mtime) / 60) % 60, (long int)(seconds - statistics->st_mtime) % 60);	printf("\n");
	printf("\n");
}

void seach_directory(char* path, int current_depth, int mtime_flag, int atime_flag, int maxdepth_flag, int mtime_sign, int atime_sign, int mtime_value, int atime_value, int maxdepth_value) {
	time_t seconds = time(NULL);

	DIR* dir;
	struct dirent* entry;

	if ((dir = opendir(path))) {
		while ((entry = readdir(dir)) != NULL) {
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;
			if (current_depth == -1) {
				current_depth++;
				struct stat* tmp = (struct stat*) calloc(1, sizeof(struct stat));
				lstat(path, tmp);
				print_info(path, tmp);
				free(tmp);
				if (maxdepth_flag == 1 && maxdepth_value == 0) {
					return;
				}
			}
			struct stat* tmp = (struct stat*) calloc(1, sizeof(struct stat));
			char* tmp_path = (char*)calloc(1000, sizeof(char));
			strcpy(tmp_path, path);
			strcat(tmp_path, "/");
			strcat(tmp_path, entry->d_name);
			lstat(tmp_path, tmp);

			if (mtime_flag) {

				int mtime = (seconds - tmp->st_mtime) / 86400;
				int days = mtime_value;
				if (mtime_sign == 1 && !(mtime > days)) {
					free(tmp_path);
					free(tmp);                                          
					continue;                                           
				}
				else if (mtime_sign == 0 && !(mtime == days)) {
					free(tmp_path);
					free(tmp);
					continue;
				}
				else if (mtime_sign == -1 && !(mtime < days)) {
					free(tmp_path);
					free(tmp);
					continue;
				}
			}

			if (atime_flag) {
				int atime = (seconds - tmp->st_atime) / 86400;
				int days = atime_value;
				if (atime_sign == 1 && !(atime > days)) {
					free(tmp_path);
					free(tmp);                                          
					continue;                                           
				}
				else if (atime_sign == 0 && !(atime == days)) {
					free(tmp_path);
					free(tmp);
					continue;
				}
				else if (atime_sign == -1 && !(atime < days)) {
					free(tmp_path);
					free(tmp);
					continue;
				}
			}


			char* actualpath = (char*)calloc(10000, sizeof(char));
			realpath(tmp_path, actualpath);

			print_info(actualpath, tmp);
			if ((S_ISDIR(tmp->st_mode) && !S_ISLNK(tmp->st_mode))) {
				if (maxdepth_flag == 1 && (current_depth + 1 < maxdepth_value)) {
					current_depth++;
					seach_directory(actualpath, current_depth, mtime_flag, atime_flag, maxdepth_flag, mtime_sign, atime_sign, mtime_value, atime_value, maxdepth_value);
				}
				else if (maxdepth_flag == 0) {
					seach_directory(actualpath, current_depth, mtime_flag, atime_flag, maxdepth_flag, mtime_sign, atime_sign, mtime_value, atime_value, maxdepth_value);
				}
			}
			free(actualpath);
			free(tmp_path);
			free(tmp);
		}
	}
	else {
		perror("opendir");
		printf("Used path: %s\n\n", path);
	}

	closedir(dir);
}

static int display_info(const char* path, const struct stat* statistics, int tflag, struct FTW* ftwbuf) {
	if (maxdepth_flag == 1 && ftwbuf->level > maxdepth_value) {
		return 0;
	}
	time_t seconds = time(NULL);

	if (mtime_flag) {
		int mtime = (seconds - statistics->st_mtime) / 86400;
		int days = mtime_value;
		if (mtime_sign == 1 && !(mtime > days)) {
			return 0;
		}
		else if (mtime_sign == 0 && !(mtime == days)) {
			return 0;
		}
		else if (mtime_sign == -1 && !(mtime < days)) {
			return 0;
		}
	}

	if (atime_flag) {
		int atime = (seconds - statistics->st_atime) / 86400;
		int days = atime_value;
		if (atime_sign == 1 && !(atime > days)) {
			return 0;
		}
		else if (atime_sign == 0 && !(atime == days)) {
			return 0;
		}
		else if (atime_sign == -1 && !(atime < days)) {
			return 0;
		}
	}

	char* actualpath = (char*)calloc(10000, sizeof(char));
	realpath(path, actualpath);
	printf("File path: %s\n", actualpath);
	free(actualpath);

	printf("Number of hard links: %ld\n", statistics->st_nlink);

	if (S_ISREG(statistics->st_mode)) {
		printf("File type: %s\n", "file");
	}
	else if (S_ISDIR(statistics->st_mode)) {
		printf("File type: %s\n", "dir");
	}
	else if (S_ISCHR(statistics->st_mode)) {
		printf("File type: %s\n", "char dev");
	}
	else if (S_ISBLK(statistics->st_mode)) {
		printf("File type: %s\n", "block dev");
	}
	else if (S_ISFIFO(statistics->st_mode)) {
		printf("File type: %s\n", "fifo");
	}
	else if (S_ISLNK(statistics->st_mode)) {
		printf("File type: %s\n", "slink");
	}
	else if (S_ISSOCK(statistics->st_mode)) {
		printf("File type: %s\n", "sock");
	}

	printf("Size: %ld\n", statistics->st_size);
	printf("Last access: %ld:%ld:%ld:%ld:%ld ago\n", (long int)(seconds - statistics->st_atime) / 2592000, (long int)(seconds - statistics->st_atime) / 86400, (long int)(seconds - statistics->st_atime) / 3600, (long int)((seconds - statistics->st_atime) / 60)%60, (long int)(seconds - statistics->st_atime) % 60);
	printf("Last modification: %ld:%ld:%ld:%ld:%ld ago\n", (long int)(seconds - statistics->st_mtime) / 2592000, (long int)(seconds - statistics->st_mtime) / 86400, (long int)(seconds - statistics->st_mtime) / 3600, (long int)((seconds - statistics->st_mtime) / 60)%60, (long int)(seconds - statistics->st_mtime) % 60);
	printf("\n");

	return 0;
}

int main(int argc, char** argv) {

	printf("\n\n");

	char* path;
	char* mode;

	if (argc == 3) {
		path = argv[1];
	}
	else if (argc == 5) {
		path = argv[1];
		parse_command(argv[2], argv[3], &mtime_flag, &atime_flag, &maxdepth_flag, &mtime_sign, &atime_sign, &mtime_value, &atime_value, &maxdepth_value);
		mode = argv[4];
	}
	else if (argc == 7) {
		path = argv[1];
		parse_command(argv[2], argv[3], &mtime_flag, &atime_flag, &maxdepth_flag, &mtime_sign, &atime_sign, &mtime_value, &atime_value, &maxdepth_value);
		parse_command(argv[4], argv[5], &mtime_flag, &atime_flag, &maxdepth_flag, &mtime_sign, &atime_sign, &mtime_value, &atime_value, &maxdepth_value);
		mode = argv[6];
	}
	else if (argc == 9) {
		path = argv[1];
		parse_command(argv[2], argv[3], &mtime_flag, &atime_flag, &maxdepth_flag, &mtime_sign, &atime_sign, &mtime_value, &atime_value, &maxdepth_value);
		parse_command(argv[4], argv[5], &mtime_flag, &atime_flag, &maxdepth_flag, &mtime_sign, &atime_sign, &mtime_value, &atime_value, &maxdepth_value);
		parse_command(argv[6], argv[7], &mtime_flag, &atime_flag, &maxdepth_flag, &mtime_sign, &atime_sign, &mtime_value, &atime_value, &maxdepth_value);
		mode = argv[8];
	}
	else {
		puts("Wrong amount of arguments, remember to add last if you want nftw or sys");
		path = NULL;
		return 0;
	}

	if (strcmp(mode, "nftw") == 0) {
		int flags = 0;
		flags |= FTW_PHYS;
		if (nftw(path, display_info, 20, flags) == -1) {
			perror("nftw");
			exit(EXIT_FAILURE);
		}
	}
	else if (strcmp(mode, "sys") == 0) {
		seach_directory(path, -1, mtime_flag, atime_flag, maxdepth_flag, mtime_sign, atime_sign, mtime_value, atime_value, maxdepth_value);
	}

	printf("\n\n");
}