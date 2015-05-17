#include "sharemem.h"

static void default_signal_handler( int signo ) {
	printf( "W1: obdrzany signal %d\n", signo );
	exit( EXIT_SUCCESS );
}

main()
{
  char c;
  char *s;
// nastavenie obsluhy signalov
  signal( SIGTERM, default_signal_handler );
  signal( SIGKILL, default_signal_handler );
  signal( SIGQUIT, default_signal_handler ); 

  alloc_shared_mem( SHARED_BUFFER_SIZE );
  alloc_shared_offset();
  alloc_semaphor();
  if(nice(5)== -1)
    perror("Chyba v nastaveni priprity w1"); 
  lock_semaphore(); 
  /* Now put some things into the memory for the other process to read.*/
  for (c = 'a'; c <= 'z'; c++){
  s = get_offset_buf();
  *s = c;
  set_offset();
  sleep(1);
  }
  *(++s) = NULL;
  unlock_semaphore();
  exit(0);
}
