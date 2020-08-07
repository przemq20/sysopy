#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>

char *pipe_path;
char *result_path; 
int n;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Wrong number of arguments!\n");
        return -1;
    }
    pipe_path = argv[1];
    result_path = argv[2];
    n = atoi(argv[3])+1;

    FILE *p = fopen(pipe_path,"r");
    FILE *f = fopen(result_path,"w");
    char chunk[n];
    if(f != NULL) {
        while (fgets(chunk,n,p) != NULL) {
            fprintf(f,"%s",chunk);
        }
    }
    fclose(p);
    fclose(f);

    return 0;
}