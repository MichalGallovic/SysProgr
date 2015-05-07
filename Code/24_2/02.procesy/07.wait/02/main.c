#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

int child() {
	long int i;

	for ( i = 0; i < 1000000000; i++ ) {
		fprintf(stderr,"child %012ld\n", i);
		usleep(130000);
	}
	return 0;
}

int main() {
	pid_t pid;
	int status;

	pid = fork();
	switch (pid) {
	case -1:
		perror("fork");
		exit(-1);
	case 0:
		child();
		fprintf(stderr,"child skoncil normalne\n");
		exit(0);
	default:
		break;
	}

	fprintf(stderr, "parent pocka zo 1-2 sekunde...\n");
	sleep(2);
	fprintf(stderr, "parent (po)zastavi dieta ;) a pocka zo 2 sekunde...\n");
	kill(pid, SIGSTOP);
	sleep(2);
	fprintf(stderr, "parent pusti dieta dalej... pocka zo 2 sekunde...\n");
	kill(pid, SIGCONT);
	sleep(2);
	fprintf(stderr, "rodica to prestalo bavit... potrati dieta terminalnym signalom (SIGTERM)\n");
	kill(pid, SIGTERM);

	wait(&status);
	if (WIFEXITED(status))
		fprintf(stderr,"dieta skoncilo normalne s kodom %d\n", WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		fprintf(stderr,"dieta skoncilo NE-NOR-MAL-NE signalom %d\n", WTERMSIG(status));
	else
		fprintf(stderr,"dieta skoncilo NE-NOR-MAL-NE\n");

	return 0;
}
