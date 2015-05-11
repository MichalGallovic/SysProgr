
// Tento program robi to iste co zlozeny shellovsky prikaz: ps -e | wc -l

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main ()
{
  int fds[2];

  pipe (fds);
  switch( fork() ) {
  case -1:
	perror("fork1");
	return -1;
  case 0:
	close( fds[0] );
	dup2( fds[1], STDOUT_FILENO );
	execlp( "ps", "ps", "-e", NULL );
	perror("child1: error: exec ps");
	return -1;
  default:
	close( fds[1] );
	break;
  }
  wait( NULL );
  fprintf(stderr, "parent: process ps finished\n");

  switch( fork() ) {
  case -1:
	perror("fork2");
	return -1;
  case 0:
	close( fds[1] );
	dup2( fds[0], STDIN_FILENO );
	execlp( "wc", "wc", "-l", NULL );
	perror("child2: error: exec wc");
	return -1;
  default:
	close( fds[0] );
	break;
  }
  wait( NULL );
  fprintf(stderr, "parent: process wc finished\n");

  return 0;
}
