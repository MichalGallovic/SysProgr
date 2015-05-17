#include <sys/wait.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "sharemem.h"


pid_t	pid,			// pid 
	pidC,			// pid write1
	pidT,			// pid write2
	pidG;			// pid reader



void create_share_mem() {
	alloc_shared_mem(SHARED_BUFFER_SIZE);
	alloc_shared_offset();
}

void free_share_mem() {
	free_shared_mem();
	free_shared_offset();
}

int end() {
	while( wait( NULL ) != -1 )
		;
	free_shared_mem();
	free_shared_offset();
	free_semaphor();
	printf( "koniec programu\n" );
	return EXIT_SUCCESS;
}

static void child_exit_handler(int signo) {
	pid_t childpid;
	int status;

	childpid = waitpid(0, &status, WNOHANG);
	if (-1 == childpid || 0 == childpid) {
		signal(signo, child_exit_handler);
		return;
	}

	signal(signo, child_exit_handler);	//znovunastavenie obsluhy daneho signalu
}

static void default_signal_handler(int signo) {	
	//obsluha signalu ukoncenia.
	printf("\npid %d: obdrzal signal %d, preto svoju cinnost konci\n", getpid(), signo);	
       //Obsluhuje signaly: KILL,TERM,QUIT
	kill( pidG, SIGTERM );
	kill( pidC, SIGTERM );
	kill( pidT, SIGTERM );
	free_share_mem();
        free_semaphor();
	exit( EXIT_SUCCESS );
}

int main(void) {
        // nastavime obsluhu pre tieto signaly
	signal(SIGCHLD, child_exit_handler);		// ukoncenie dietata
	signal(SIGTERM, default_signal_handler);	// ukoncenie procesu
	signal(SIGQUIT, default_signal_handler);	// ukoncenie procesu
	signal(SIGINT, default_signal_handler);		// prerusenie procesu
	
	// vytvorime zdielanu pamat
	create_share_mem();
	// alokacia a inicializacia semafora
	alloc_semaphor();
	init_semaphor();
	//vymazanie pamate
	clear_data();
	clear_offset();
	// forkneme a execneme proces write1 
	if ( (pidG = fork()) == 0 ) {
		execl( "./write1", "./write1",NULL );
		perror( "nepodarilo sa spustit proces write1" );
		exit( EXIT_FAILURE );
	} else if ( pidG == -1 ) {
		perror( "nepodarilo sa spustit proces write1" );
		free_shared_mem();
		exit( EXIT_FAILURE );
	}
	// forkneme a execneme proces write2
	if ( (pidC = fork()) == 0 ) {
		execl( "./write2", "./write2",NULL );
		perror( "nepodarilo sa spustit proces write2" );
		exit( EXIT_FAILURE );
	} else if ( pidC == -1 ) {
		perror( "nepodarilo sa spustit proces write2" );
		free_shared_mem();
		exit( EXIT_FAILURE );
	}
	// forkneme a execneme proces reader
	if ( (pidT = fork()) == 0 ) {
		execl( "./reader", "./reader",NULL );
		perror( "nepodarilo sa spustit proces reader" );
		exit( EXIT_FAILURE );
	} else if ( pidT == -1 ) {
		perror( "nepodarilo sa spustit proces reader" );
		free_shared_mem();
		exit( EXIT_FAILURE );
	}
	printf("w1= %d, w2= %d,reader= %d\n",pidG, pidC, pidT);
	return end();
}
