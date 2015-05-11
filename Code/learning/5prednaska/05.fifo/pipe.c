#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void run_child( int fd[2] ) {
	close( fd[1] );
	dup2( fd[0], STDIN_FILENO );
	execlp( "sort", "sort", NULL );
	perror( "Error: child: exec sort");
}

void run_parent( int fd[2] ) {
	FILE *stream;

	close( fd[0] );
	stream = fdopen( fd[1], "w" );
	if ( !stream ) {
		perror( "Error: parent: fdopen" );
		return;
	}
	fprintf (stream, "This is a test.\n");
	fprintf (stream, "Hello, world.\n");
	fprintf (stream, "My dog has fleas.\n");
	fprintf (stream, "This program is great.\n");
	fprintf (stream, "One fish, two fish.\n");
	fflush (stream);

	close (fd[1]);
}

int main ()
{
  int fds[2];

  pipe (fds);
  switch ( fork() ) {
  case -1:
    perror("fork");
    return -1;
  case 0:
    run_child(fds);
    return -1;
  default:
    run_parent(fds);
    break;
  }

  wait(NULL);
  fprintf(stderr, "parent: Child finished \n");
  return 0;
}
