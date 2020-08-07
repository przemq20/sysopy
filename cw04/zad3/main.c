#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <execinfo.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void sgn_handler(int signum, siginfo_t* info, void* secret) {
	switch (signum)
	{
	case 4:
		printf("\n\nSignal name = SIGILL\n");
		break;
	case 8:
		printf("\n\nSignal name = SIGFPE\n");
		break;
	case 11:
		printf("\n\nSignal name = SIGSEGV\n");
		break;
	case 20:
		printf("\n\nSignal name = SIGCHLD\n");
		break;
	case 29:
		printf("\n\nSignal name = SIGIO\n");
		break;
	default:
		break;
	}
	printf("Signal number = %d\n", info->si_signo);
	printf("Sending PID = %d\n", info->si_pid);
	printf("Signal value %d\n", info->si_value.sival_int);
	printf("Sending process UID = %d\n", info->si_uid);
	printf("Signal code = %d\n", info->si_code);
	if (info->si_code == SI_KERNEL) {
		printf("Signal send by kernel\n");
	}
	else {
		printf("Signal send by user\n");
	}
	printf("Signal error no = %d\n", info->si_errno);

	switch (signum)
	{
	case 4:
		printf("Address of occured fault: %p\n", info->si_addr);
		break;
	case 8:
		printf("Address of occured fault: %p\n", info->si_addr);
		break;
	case 11:
		printf("Address of occured fault: %p\n", info->si_addr);
		break;
	case 20:
		printf("Child exit code = %d\n", info->si_status);
		break;
	case 29:
		printf("Band event = %ld\n", info->si_band);
		break;
	default:
		break;
	}
}

int main(int argc, char* argv[]) {
	int signal = atoi(argv[1]);
	struct sigaction action;
	action.sa_handler = (void*)sgn_handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = SA_SIGINFO;
	sigaction(signal, &action, NULL);

	if (signal == 20) {
		if (fork() == 0) {
			raise(signal);
		}
		wait(NULL);
	}
	else {
		raise(signal);
	}
	return 0;
}
