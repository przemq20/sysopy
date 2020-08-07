#define _POSIX_C_SOURCE 199309L
#define SIG1(X) (X == SIGRT ? SIGRTMIN : SIGUSR1)
#define SIG2(X) (X == SIGRT ? SIGRTMIN + 1 : SIGUSR2)
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>

typedef enum { KILL, SIGQUEUE, SIGRT } mode_t;

mode_t get_mode(char* encoded_mode) {
	if (strcmp(encoded_mode, "KILL") == 0) return KILL;
	if (strcmp(encoded_mode, "SIGQUEUE") == 0) return SIGQUEUE;
	return SIGRT;
}



int got_last_reply = 0;
int replies_count = 0;

void sigusr1(int signum, siginfo_t* info, void* context) {
	// ignore warning about unused parameter
	(void)signum;
	(void)info;
	(void)context;

	replies_count++;
}

void sigusr1_sigqueue(int signum, siginfo_t* info, void* context) {
	// ignore warning about unused parameter
	(void)signum;
	(void)context;

	printf("Got signal no. %d\n", info->si_value.sival_int);
	replies_count++;
}

void sigusr2(int signum, siginfo_t* info, void* context) {
	// ignore warning about unused parameter
	(void)signum;
	(void)info;
	(void)context;

	got_last_reply = 1;
}

int main(int argc, char* argv[]) {
	if (argc != 4) {
		printf("Wrong amount of parameters\n");
		return 1;
	}

	int pid = atoi(argv[1]);
	int count = atoi(argv[2]);
	mode_t mode = get_mode(argv[3]);

	sigset_t block_mask;
	sigfillset(&block_mask);
	sigdelset(&block_mask, SIG1(mode));
	sigdelset(&block_mask, SIG2(mode));
	sigprocmask(SIG_SETMASK, &block_mask, NULL);

	struct sigaction sigusr1_action;
	sigusr1_action.sa_sigaction = mode == SIGQUEUE ? sigusr1_sigqueue : sigusr1;
	sigusr1_action.sa_flags = SA_SIGINFO;
	sigemptyset(&sigusr1_action.sa_mask);
	sigaction(SIG1(mode), &sigusr1_action, NULL);

	struct sigaction sigusr2_action;
	sigusr2_action.sa_sigaction = sigusr2;
	sigusr2_action.sa_flags = SA_SIGINFO;
	sigemptyset(&sigusr2_action.sa_mask);
	sigaction(SIG2(mode), &sigusr2_action, NULL);

	for (int i = 0; i < count; i++) {
		kill(pid, SIG1(mode));
	}
	kill(pid, SIG2(mode));

	while (!got_last_reply) {
	}

	printf("Sent %d signals, got back %d\n", count, replies_count);
}