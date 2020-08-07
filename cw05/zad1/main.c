#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

char** parse(char* line) {
    static char* e[11];

    int i = 0;
    while (line != NULL && i < 10) {
        char* arg = strsep(&line, " \n");
        if (*arg) e[i++] = arg;
    }

    while (i < 10)
        e[i++] = NULL;

    return e;
}

int main(int argc, char** argv) {
    if (argc != 2) return 1;
    FILE* commands = fopen(argv[1], "r");

    int current[2], previous[2] = { -1, -1 };
    char* line = NULL, * process_info;
    size_t line_len = 0;

    while (getline(&line, &line_len, commands) > 0) {
        while ((process_info = strsep(&line, "|")) != NULL) {
            pipe(current);
            if (fork() == 0){

                for (char** __x = parse(process_info);; (execvp(__x[0], __x) != -1) || (exit(1), 1)) {
                    dup2((close(previous[1]), previous[0]), STDIN_FILENO);
                    if (!(line == NULL)) dup2((close(current[0]), current[1]), STDOUT_FILENO);
                }
            }
            previous[0] = (close(current[1]), current[0]);
            previous[1] = current[1];
        }
        while (wait(NULL) > 0);
    }
}