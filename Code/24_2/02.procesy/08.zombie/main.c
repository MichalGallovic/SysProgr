#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int main() {
	pid_t pid;

	pid = fork();
	switch(pid) {
	case -1:
		perror("fork");
		return -1;
	case 0:
		fprintf(stderr,"dieta... okamzite koncim\n");
		return 0;
	default:
		break;
	}
	
	sleep(120);
	return 0;
}
