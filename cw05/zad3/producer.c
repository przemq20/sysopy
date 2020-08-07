#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>

char *pipe_path;
char *input_path;
int n;

int main(int argc, char *argv[]) {
    srand(time(NULL));
    if (argc != 4) {
        printf("Wrong number of arguments!\n");
        return -1;
    }
    pipe_path = argv[1];
    input_path = argv[2];
    n = atoi(argv[3])+1;

    int p = open(pipe_path,O_WRONLY);
    FILE *f = fopen(input_path,"r");
    char chunk[n];
    if(f != NULL) {
        while (fgets(chunk,n,f) != NULL) {
            char msg[10+n];
            sprintf(msg,"#%d#%s\n",getpid(),chunk);
            write(p,msg,strlen(msg));
            sleep(2);
        }
    }
    close(p);
    fclose(f);
    
    return 0;
}