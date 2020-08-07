#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

int random_int(int min, int max) {
    return rand() % (max - min + 1) + min;
}

int main(int argc, char** argv) {
    srand(time(NULL));

    int n = atoi(argv[1]), min = atoi(argv[2]), max = atoi(argv[3]), minNum= atoi(argv[4]), maxNum= atoi(argv[5]);

    char* file_name = (char*) calloc(100, sizeof(char));

    FILE* tests_list = fopen("tests_list", "w+");

    for(int i = 0; i < n; i++) {
        char str[3];
        sprintf(str, "%d", i);

        strcpy(file_name, "tests/");
        strcat(file_name, "test");
        strcat(file_name, str);
        strcat(file_name, "_A");
        FILE* fileA = fopen(file_name, "w+");
        fprintf(tests_list, "%s ", file_name);

        strcpy(file_name, "tests/");
        strcat(file_name, "test");
        strcat(file_name, str);
        strcat(file_name, "_B");
        FILE* fileB = fopen(file_name, "w+");
        fprintf(tests_list, "%s ", file_name);

        strcpy(file_name, "tests/");
        strcat(file_name, "test");
        strcat(file_name, str);
        strcat(file_name, "_C");
        fprintf(tests_list, "%s\n", file_name);

        int height = random_int(min, max);
        int width = random_int(min, max);

        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
                fprintf(fileA, "%d", random_int(minNum, maxNum));
                fprintf(fileA, " ");
            }
            fprintf(fileA, "\n");
        }

        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                fprintf(fileB, "%d", random_int(minNum, maxNum));
                fprintf(fileB, " ");
            }
            fprintf(fileB, "\n");
        }

        fclose(fileA);
        fclose(fileB);
    }

    return 0;
}