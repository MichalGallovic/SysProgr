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

    sigaction(SIGUSR1, &sa, NULL);
    //sigaction(SIGINT, &sa, NULL);
	//synchronne cakanie na signal
	sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
    
    int sig;
    int result = sigwait(&sigset,&sig);

    if(result == 0)
        printf("sigwait got signal: %d\n", sig);
	return EXIT_SUCCESS;
}

 
