#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>


sig_atomic_t child_status = 0;

void handler(int signal_number){
  int status;
  wait (&status);
  /* Store its exit status in a global variable. */
  child_status = status;
  printf("Child sent me %d\n", WEXITSTATUS(status));
};


int main() {
  pid_t pid;  
  struct sigaction sa;

  memset(&sa,0, sizeof(sa));
  sa.sa_handler=&handler;
  sigaction(SIGCHLD, &sa, NULL);

  pid = fork();
  switch (pid) {
    case 0:
      execl("./child1", "./child1", NULL);
      perror("[perror] Calling execl is unsuccessful\n");
      break;
    case -1:
      fprintf(stderr, "[perror] Cannot create child process!\n");
      return 1;
    default:
      fprintf(stderr, "[perror] I am the parent process. I have created a child with PID=%d and will do some work\n",pid);
      fprintf(stderr, "[perror] Now I am testing my child status value\n");
      sleep(1);
      fprintf(stderr, "[perror] My child finished I can also\n");	     
      break;
  }
  return 0;
}
