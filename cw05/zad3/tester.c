#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    char *pipe_name = "fifo";
    pid_t pids[6];
    mkfifo(pipe_name,S_IRUSR | S_IWUSR);

    char *const cons1[5] = {"./consumer","fifo","./tests/results","10",NULL};
    char *const prod1[5] = {"./producer","fifo","./tests/test1","10",NULL};
    char *const prod2[5] = {"./producer","fifo","./tests/test2","10",NULL};
    char *const prod3[5] = {"./producer","fifo","./tests/test3","10",NULL};
    char *const prod4[5] = {"./producer","fifo","./tests/test4","15",NULL};
    char *const prod5[5] = {"./producer","fifo","./tests/test5","7",NULL};

    if ((pids[5]=fork()) == 0) 
        execvp("./consumer",cons1);

    if ((pids[0]=fork()) == 0)
        execvp("./producer",prod1);

    if ((pids[1]=fork()) == 0)
        execvp("./producer",prod2);
    
    if ((pids[2]=fork()) == 0)
        execvp("./producer",prod3);

    if ((pids[3]=fork()) == 0)
        execvp("./producer",prod4);
    
    if ((pids[4]=fork()) == 0)
        execvp("./producer",prod5);
    
    for (int i=0; i<6; i++)
        waitpid(pids[i],NULL,0);

    return 0;
}