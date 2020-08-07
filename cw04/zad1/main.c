#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>

bool stopped = false;

void sigintHandle() {
    printf("Odebrano sygnal SIGINT\n");
    exit(0);
}

void sigtstpHandle() {
    if (stopped) {
        stopped = false;
    }
    else {
        printf("Oczekuje na CTRL+Z - kontynuacja albo CTRL+C - zakonczenie programu\n");
        stopped = true;
    }
}

int main() {
    struct sigaction act;
    act.sa_handler = sigtstpHandle;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    while (true) {
        signal(SIGINT, sigintHandle);
        sigaction(SIGTSTP, &act, NULL); 
        if (!stopped) {
            system("ls");
            sleep(1);
        }
        else {
            pause();
        }
    }
    return 0;
}
