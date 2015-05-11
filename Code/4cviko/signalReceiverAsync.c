#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

//doplnte funkciu SignalHandler
void SignalHandler(int signal) {
    printf("SignalHandler process ID: %d\n", signal);
}
int main()
{
    //doplnte vypis PID
    printf("ID procesu: %d\n",(int) getpid());
    //doplnte asynchronne spracovanie signalov
    struct sigaction sa, sa_int;
    memset(&sa,0,sizeof(sa));
    memset(&sa_int,0,sizeof(sa_int));
    sa.sa_handler = &SignalHandler;
    sa_int.sa_handler = &SignalHandler;
    sigaction(SIGUSR1,&sa,NULL);
    sigaction(SIGINT,&sa,NULL);
	while(1) {
		printf(".");
		fflush(stdout);
		sleep(1);
	}
	
	return EXIT_SUCCESS;
}

 
