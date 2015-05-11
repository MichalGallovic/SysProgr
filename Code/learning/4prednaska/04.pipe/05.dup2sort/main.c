#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void run_child(int fd[2]) {
	close(fd[1]);
	dup2(fd[0], STDIN_FILENO);
	// close( STDIN_FILENO ) == close(0)
	// dup(fd[0])
	// close(fd[0])
	execlp("sort","sort",NULL);
	perror("Error: child: exec sort");
}

void run_parent(int fd[2]) {
	FILE *stream;

	close(fd[0]);
	stream = fdopen(fd[1],"w");
	if ( !stream ) {
		perror("Error: parent: fdopen");
		close(fd[1]);
		return;
	}
	
	fprintf(stream, "zzz asd 1\n");
	fprintf(stream, "www asd 2\n");
	fprintf(stream, "rrr asd 3\n");
	fprintf(stream, "aaa asd 4\n");
	fprintf(stream, "bbb asd 5\n");
	fprintf(stream, "eee asd 6\n");
	fprintf(stream, "fff asd 7\n");
	fflush(stream);

	close(fd[1]);
}

int main() {
	int fd[2];

	if ( pipe(fd) == -1 ) {
		perror("Error: pipe");
		return -1;
	}

	switch( fork() ) {
	case -1:
		perror("Error: fork");
		return -1;
	case 0:
		run_child(fd);
		return -1;
	default:
		run_parent(fd);
		break;
	} // switch fork

	wait(NULL);
	return 0;
}
