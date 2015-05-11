#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

//doplnte funkciu SignalHandler
void SignalHandler(int signal) {
    printf("Received signal ID: %d\n",signal);
}
int main()
{
	//vypis PID
	printf("PID = %d\n", getpid());

	//nastavenie signal handlera
    struct sigaction sa;
    memset(&sa,0, sizeof(sa));
    sa.sa_handler = &SignalHandler;

    //sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
	
	//synchronne cakanie na signal
	sigset_t sigset;
	sigfillset( &sigset);
	sigdelset( &sigset, SIGUSR1);

	printf("pred sigsuspend\n");
    sigsuspend( &sigset);
	printf("za sigsuspend\n");
	
	return EXIT_SUCCESS;
}

 
