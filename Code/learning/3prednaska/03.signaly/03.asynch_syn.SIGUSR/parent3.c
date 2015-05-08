// problemy:
// 1. neblokovanie signalov -> default funkcia pri jeho obdrzani
// 2. asynchronne spracovany signal -> kombinacia so synchronnym spracovanim
// 3. testovanie premennej zavislej od signalu... a akcia...
// ... cele ZLE , vid `strace -f -ff -o asd ./parent3`

#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>

pid_t pidG;
sig_atomic_t x=0;
sig_atomic_t finish = 0;

static void sigint_signal_handler(int signo) {	
	//obsluha signalu ukoncenia.
   fprintf(stderr, "[parent] obtained signal: %d\n", signo);	
	kill( pidG, SIGTERM );
   finish = 0;
}

static void user_sig_handler(int signo){
   fprintf(stderr, "[parent] received SIGUSR2\n");
   x++;
}

int main() {
   int child_status1;
   sigset_t set, set1;
   struct sigaction sa_def;
   struct sigaction sa_user;

   memset(&sa_def,0, sizeof(sa_def));
   sa_def.sa_handler=&sigint_signal_handler;
   sigaction(SIGINT, &sa_def, NULL);

   memset(&sa_user,0, sizeof(sa_user));
   sigfillset(&set1);
   sa_user.sa_mask=set1;
   sa_user.sa_handler=&user_sig_handler;
   sigaction(SIGUSR2, &sa_user, NULL);
 	
  	if ( (pidG = fork()) == 0 ) {
		execl( "./child3", "./child3", NULL );
		perror( "[child] nepodarilo sa spustit proces" );
		exit( EXIT_FAILURE );
	} else if ( pidG == -1 ) {
		perror( "[parent] nepodarilo sa spustit proces" );
		exit( EXIT_FAILURE );
	}
   fprintf(stderr, "[parent] created child with PID=%d\n", pidG);
    
   sigfillset(&set);
   sigdelset(&set, SIGUSR2);
   sigdelset(&set, SIGINT);

	while( !finish ){
      fprintf(stderr, "[parent] send signal SIGUSR1 to child\n");
      kill(pidG, SIGUSR1);
      sleep(3);
      if (x==0) // !!!
         sigsuspend(&set);
      fprintf(stderr, "[parent] received %d signal(s) from child\n", x);
      x=0;
	}

   wait(&child_status1);	     
   return 0;
}
