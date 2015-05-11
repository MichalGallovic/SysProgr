#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void run_child1(int fd[2]) {
	close(0);
	dup(fd[0]);
	close(fd[0]);
	close(fd[1]);
	execlp("cut","cut","-c","-11",NULL);
	perror("Error: child1: exec cut");
	return;
}

void run_child2(int fd[2]) {
	close(1);
	dup(fd[1]);
	close(fd[0]);
	close(fd[1]);
	execlp("who","who",NULL);
	perror("Error: child2: exec who");
	return;
}

int main() {
	int rura[2];
	
	if ( pipe(rura) == -1 ) {
		perror("Error: pipe");
		return -1;
	}

	switch ( fork() ) {
	case -1:
		perror("Error: fork");
		return -1;
	case 0:
		run_child1(rura);
		return 0;
	default:
		switch( fork() ) {
		case -1:
			perror("Error: fork2");
			return -1;
		case 0:
			run_child2(rura);
			return 0;
		default:
			break;
		} // switch fork
	} // switch fork

	// rodic caka na vsetky svoje deti
	waitpid(-1,NULL,0);
	return 0;
}
