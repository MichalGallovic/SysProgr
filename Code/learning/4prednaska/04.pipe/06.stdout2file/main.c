#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>

void run_child() {
	int fd;

	fd = open("out.txt", O_WRONLY | O_CREAT, 0640 );
	close( STDOUT_FILENO );
	dup( fd );
	close( fd );
	execlp( "ls", "ls", "-la", NULL );
	perror("Error: child: exec ls");
}

int main() {
	switch( fork() ) {
	case -1:
		perror("Error: fork");
		return -1;
	case 0:
		run_child();
		return -1;
	default:
		break;
	} // swtich fork

	wait(NULL);
	return 0;
}
