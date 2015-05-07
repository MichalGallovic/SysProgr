#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

// bez wait() vid kto je rodicom dietata vo vypisoch ;)

int main() {
	pid_t	pid;
	int 	i = 0;

	fprintf(stderr,"hlavny program (rodic) ma pid = %d\n", getpid());
	pid = fork();
	switch ( pid ) {
	// chyba
	case -1:
		perror("fork");
		return -1;
	// dieta - vie si zistit pid rodica pomocou getppid()
	case 0:
		i = 16;
		fprintf(stderr, "detsky proces ma pid = %d, jeho rodic = %d\n", getpid(), getppid() );
		break;
	// rodic - NEVIE zistit pidy detskych procesov pomocou sys volani
	default:
		i = -17;
		fprintf(stderr, "rodic ma pid = %d, jeho dieta = %d\n", getpid(), pid );
		break;
	}

	fprintf(stderr, "%d: i = %d\n", getpid(), i);
	return 0;
}
