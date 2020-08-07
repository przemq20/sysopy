#include <stdlib.h>
#include "library.h"
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <sys/times.h>
#include <dlfcn.h>
#include <ctype.h>
#include <time.h>

FILE * resultFile;

void error(char * msg){
    printf("%s", msg);
    exit(0);
}

double calculate_time(clock_t start, clock_t end)
{
    return (double)(end - start) / (double)sysconf(_SC_CLK_TCK);
}

int main(int argc, char **argv) {

    struct tms **program_time = calloc(2, sizeof(struct tms *));
    clock_t real_time_program[2];
    
    for (int j = 0; j < 2; j++)
    {
        program_time[j] = (struct tms *)calloc(1, sizeof(struct tms *));
    }
    
    real_time_program[0] = times(program_time[0]);
    
    struct tms **tms_time = calloc(2, sizeof(struct tms *));
    clock_t real_time[2];
    
    for (int j = 0; j < 2; j++)
    {
        tms_time[j] = (struct tms *)calloc(1, sizeof(struct tms *));
    }
    
    char file_name[] = "raport.txt";
    FILE *report_file = fopen(file_name, "a");
    
    if (argc < 2) {
        error("Bad argument");
    }
    struct blockArray* main_tab;
    int i = 1;
    
    while(i<argc){
        char* command = argv[i];
        real_time[0] = times(tms_time[0]);
        
        if(strcmp(argv[i], "create_table") == 0){
            printf("Create table size %s:\n", argv[i+1]);
            main_tab = createArray(atoi(argv[i+1]));
            i += 2;
        } else if(strcmp(argv[i], "compare_pairs") == 0){
            i++;
            char* file1;
            char* file2;
            
            while(strcmp(argv[i], "create_table") != 0 && strcmp(argv[i], "compare_pairs") != 0 && strcmp(argv[i], "remove_block") != 0 && strcmp(argv[i], "remove_operation") != 0){
                file1 = strtok(argv[i], ":");
                file2 = strtok(NULL, ":");
                int x = createBlock(main_tab, file1, file2);
                printf("Comparing files %s and %s, index: %d", file1, file2, x);
                i++;
                
                if(i==argc) break;
            }
        }
        else if(strcmp(argv[i], "remove_block") == 0){
            printf("Remove block %s:\n", argv[i+1]);
            deleteBlock(atoi(argv[i+1]), main_tab);
            i += 2;
        } else if(strcmp(argv[i], "remove_operation") == 0){
            printf("Remove operation block %s in block %s\n", argv[i+2], argv[i+1]);
            deleteOperation(atoi(argv[i+2]), atoi(argv[i+1]), main_tab);
            i += 3;
        }
        else {
            error("Bad argument");
        }
        real_time[1] = times(tms_time[1]);
        printf("[REAL_TIME] Executing action %s took %fs\n", command, calculate_time(real_time[0], real_time[1]));
        printf("[USER_TIME] Executing action %s took %fs\n", command, calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime));
        printf("[SYSTEM_TIME] Executing action %s took %fs\n", command, calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));
        fprintf(report_file, "[REAL_TIME] Executing action %s took %fs\n", command, calculate_time(real_time[0], real_time[1]));
        fprintf(report_file,"[USER_TIME] Executing action %s took %fs\n", command, calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime));
        fprintf(report_file, "[SYSTEM_TIME] Executing action %s took %fs\n", command, calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));
    }
    
    real_time_program[1] = times(program_time[1]);
    fprintf(report_file, "[REAL_TIME] Executing main.c took %fs\n", calculate_time(real_time_program[0], real_time[1]));
    fprintf(report_file, "[USER_TIME] Executing main.c took %fs\n", calculate_time(program_time[0]->tms_utime, program_time[1]->tms_utime));
    fprintf(report_file, "[SYSTEM_TIME] Executing main.c took %fs\n", calculate_time(program_time[0]->tms_stime, program_time[1]->tms_stime));

    return 0;
}