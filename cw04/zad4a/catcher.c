#define _POSIX_C_SOURCE 199309L
#define SIG1(X) (X == SIGRT ? SIGRTMIN : SIGUSR1)
#define SIG2(X) (X == SIGRT ? SIGRTMIN + 1 : SIGUSR2)
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef enum { KILL, SIGQUEUE, SIGRT } mode_t;

mode_t get_mode(char* encoded_mode) {
	if (strcmp(encoded_mode, "KILL") == 0) return KILL;
	if (strcmp(encoded_mode, "SIGQUEUE") == 0) return SIGQUEUE;
	return SIGRT;
}	

int sender_pid = 0;
int sigusr1_count = 0;

void sigusr1(int signum) {
	(void)signum;
	sigusr1_count++;
}

void sigusr2(int signum, siginfo_t* info, void* context) {
	(void)signum;
	(void)context;
	sender_pid = info->si_pid;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Wrong amount of parameters\n");
		return 1;
	}

	mode_t mode = get_mode(argv[1]);

	sigset_t block_mask;
	sigfillset(&block_mask);
	sigdelset(&block_mask, SIG1(mode));
	sigdelset(&block_mask, SIG2(mode));
	sigprocmask(SIG_SETMASK, &block_mask, NULL);

	struct sigaction sigusr1_action;
	sigusr1_action.sa_handler = sigusr1;
	sigusr1_action.sa_flags = 0;
	sigemptyset(&sigusr1_action.sa_mask);
	sigaction(SIG1(mode), &sigusr1_action, NULL);

	struct sigaction sigusr2_action;
	sigusr2_action.sa_sigaction = sigusr2;
	sigusr2_action.sa_flags = SA_SIGINFO;
	sigemptyset(&sigusr2_action.sa_mask);
	sigaction(SIG2(mode), &sigusr2_action, NULL);

	printf("%d\n", getpid());

	while (sender_pid == 0) {
	}

	printf("received %d signals\n", sigusr1_count);

	for (int i = 0; i < sigusr1_count; i++) {
		if (mode == SIGQUEUE) {
			sigqueue(sender_pid, SIG1(mode), (union sigval) { .sival_int = i });
		}
		else {
			kill(sender_pid, SIG1(mode));
		}
	}
	kill(sender_pid, SIG2(mode));
}