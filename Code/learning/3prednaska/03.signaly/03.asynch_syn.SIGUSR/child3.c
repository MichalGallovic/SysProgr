// problemy:
// 1. neblokovanie signalov -> default funkcia pri jeho obdrzani
// 2. asynchronne spracovany signal -> kombinacia so synchronnym spracovanim
// 3. testovanie premennej zavislej od signalu... a akcia...
// ... cele ZLE , vid `strace -f -ff -o asd ./parent3`

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

sig_atomic_t x=0;
sig_atomic_t finish = 0;

static void sigterm_signal_handler(int signo){
    fprintf(stderr, "[child] obtained signal %d\n", signo);	
    finish = 1;
};

void user_signal_handler(int signo){
   fprintf(stderr, "[child] received SIGUSR1\n");
   x++;
}

int main() {
  sigset_t set, set1;
  struct sigaction sa;
  struct sigaction sa_par;

  memset(&sa_par,0, sizeof(sa_par));
  sa_par.sa_handler=&sigterm_signal_handler;
  sigaction(SIGTERM, &sa_par, NULL);
  
  sigfillset(&set1);
  memset(&sa,0, sizeof(sa));
  sa.sa_mask=set1;
  sa.sa_handler=&user_signal_handler;
  sigaction(SIGUSR1, &sa, NULL);

  sigfillset(&set);
  sigdelset(&set, SIGTERM);
  sigdelset(&set, SIGUSR1);

  while( !finish ){
     if (x==0) // !!!
         sigsuspend(&set);
     fprintf(stderr, "[child] received %d signal(s) from parent\n", x);
     sleep(1);
     fprintf(stderr, "[child] send signal SIGUSR2 to parent\n");
     kill(getppid(),SIGUSR2);
     x=0;
  }

  return 0;
}


