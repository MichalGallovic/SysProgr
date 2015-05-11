#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void writer( const char *msg, int cnt, FILE *stream ) {
	while ( cnt > 0 ) {
		fprintf(stream, "%s\n", msg);
		fflush(stream);
		sleep( 1 );
		cnt--;
	} //while cnt > 0
}

void reader( FILE *stream ) {
	char buffer[1024];

	while ( !feof(stream) && !ferror(stream) ) {
		if ( fgets( buffer, sizeof(buffer), stream ) == NULL )
			break;
		fputs( buffer, stdout );
	} // while feof && ferror
}

void run_child(int fd[2]) {
	FILE *stream;
	
	close( fd[1] );
	stream = fdopen( fd[0], "r" );
	if ( stream == NULL ) {
		perror("[child]: fdopen");
		return;
	}

	reader( stream );
	close( fd[0] );
}

void run_parent(int fd[2]) {
	FILE *stream;

	close( fd[0] );
	stream = fdopen( fd[1], "w" );
	if ( stream == NULL ) {
		perror("[parent]: fdopen");
		return;
	}

	writer( "Ahoj svjete!", 5, stream );
	close( fd[1] );
}

int main() {
	int fd[2];
	pid_t pid;

	if ( pipe(fd) == -1 ) {
		perror("Error: pipe");
		return -1;
	}
	pid = fork();
	switch ( pid ) {
	case -1:
		perror("Error: fork");
		return -1;
	case 0:
		run_child(fd);
		return 0;
	default:
		run_parent(fd);
		return 0;
	} //switch

	return 0;
}
