#include "sharemem.h"

static void default_signal_handler( int signo ) {
	printf( "R: obdrzany signal %d\n", signo );
	free_shared_mem();
	free_shared_offset();
	free_semaphor();
	exit( EXIT_SUCCESS );
} 
main()
{
  char *s;
  // nastavenie obsluhy signalov
  signal( SIGTERM, default_signal_handler );
  signal( SIGKILL, default_signal_handler );
  signal( SIGQUIT, default_signal_handler ); 
  alloc_shared_mem( SHARED_BUFFER_SIZE );
  alloc_shared_offset();
  alloc_semaphor();
  if( nice(5)== -1)
    perror ("Chyba v nastaveni priority pre reader"); 
  /* Now read from the memory.*/
  while(1){
  lock_semaphore(); 
  for (s = get_share_buf(); *s != NULL; s++){
  putchar(*s);
  }
  putchar('\n');
  unlock_semaphore();
  sleep(1);
 }
  exit(0);
}
