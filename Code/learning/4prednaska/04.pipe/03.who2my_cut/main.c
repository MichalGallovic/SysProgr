#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void run_child( int fd[2] ) {
	close(1);
	close(1);
	dup( fd[1] );
	close( fd[0] );
	close( fd[1] );

	execlp("who","who",NULL);
	perror("[child]: Error: exec 'who'");
	return;
}

void run_parent( int fd[2] ) {
	int finish = 0;
	char buf[12];

	close( fd[1] ); // co ked toto zabudneme dat???
	while( !finish ) {
		// nacitame prvych 12 znakov
		if ( read(fd[0], buf, 12) != 12 )
			break;
		// vypiseme prvych 11
		// POZOR: predpokladame, ze v kazdom riadku je aspon 11 znakov!
		write(1, buf, 11);
		write(1, "\n", 1);
		// zvysok riadku precitame znak po znaku
		while ( (read(fd[0],buf,1)==1) && (buf[0]!='\n') )
			;
	} // while !finish

	wait( NULL );
}

int main() {
	int rura[2];

	if ( pipe(rura) == -1 ) {
		perror("Error: pipe");
		return -1;
	}

	switch( fork() ) {
	case -1:
		perror("Error: fork");
		return -1;
	case 0:
		run_child(rura);
		return 0;
	default:
		run_parent(rura);
		return 0;
	} // switch fork
	return 0;
}
