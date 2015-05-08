#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>

/*Program demonstruje synchronne odchytenie synchronizaceho signalu pouzitim funkcie sigwait.
Signal musi byt v tomto pripade blokovany. */

pid_t pidG;

static void default_signal_handler(int signo) {	
	kill( pidG, SIGTERM );
   fprintf(stderr, "[parent] received signal %d\n", signo);
	exit( EXIT_SUCCESS );
}


int main() {
    int child_status1;
    int sig_number;
    sigset_t set;
    struct sigaction sa_def;

    memset(&sa_def,0, sizeof(sa_def));
    sa_def.sa_handler=&default_signal_handler;
    sigaction(SIGINT, &sa_def, NULL);

    sigemptyset(&set);
    sigaddset(&set,SIGUSR2);
    //sigaddset(&set,SIGUSR1); // !!!!!!!!!
    sigprocmask(SIG_BLOCK, &set, NULL);	

  	 if ( (pidG = fork()) == 0 ) {
		execl( "./child2", "./child2",NULL );
		perror( "[child] nepodarilo sa spustit program child2" );
		exit( EXIT_FAILURE );
	 } else if ( pidG == -1 ) {
		perror( "[parent] nepodarilo sa spustit proces" );
		exit( EXIT_FAILURE );
	 }
    fprintf(stderr, "[parent] created child with PID=%d\n", pidG);
    
	while(1){
      fprintf(stderr, "[parent] send signal to child\n");
      kill(pidG, SIGUSR1);
      sleep(3);
      sigwait(&set, &sig_number);
      fprintf(stderr, "[parent] received signal %d from child\n",sig_number);
	}

   wait(&child_status1);	     
   return 0;
}

