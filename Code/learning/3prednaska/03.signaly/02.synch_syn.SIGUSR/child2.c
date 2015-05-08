#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

/*Program demonstruje synchronne odchytenie synchronizaceho signalu pouzitim funkcie sigwait.
Signal musi byt v tomto pripade blokovany. */

static void par_handler(int signo){
   fprintf(stderr, "[child] received signal %d\n", signo);
	exit( EXIT_SUCCESS );
};

int main() {
  int sig_number;
  sigset_t set;
  struct sigaction sa_par;
  
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  sigprocmask(SIG_BLOCK, &set, NULL);

  memset(&sa_par,0, sizeof(sa_par));
  sa_par.sa_handler=&par_handler;
  sigaction(SIGTERM, &sa_par, NULL);

  while(1){ 
     sigwait(&set, &sig_number);
     fprintf(stderr, "[child] received signal %d from parent\n", sig_number);
     sleep(1);
     fprintf(stderr, "[child] send signal SIGUSR2 to parent\n");
     kill(getppid(),SIGUSR2);
  }
  return 0;
}


