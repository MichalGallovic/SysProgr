#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int run( char *program, char **args ) {
	pid_t pid;

	pid = fork();
	switch (pid) {
	case -1:
		perror("fork");
		return -1;
	case 0:
		execvp( program, args );
		perror("exec");
		return -1;
	default:
		break;
	}
	return 0;
}

int main() {
	char *args[] = {
		"ls",
		"-l",
		"/tmp",
		NULL,
	};
	
	run(args[0], args);
	wait(NULL);
	fprintf(stderr,"hlavny program konci\n");
	return 0;
}
