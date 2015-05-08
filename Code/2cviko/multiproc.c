#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void child(int childId) {
	sleep(1);
	printf("child %d\n", childId);
}

int main()
{
	const int NUM_CHILDREN = 5;
	pid_t pid;
	int childId;

	//vytvorenie detskych procesov
	
	for(childId = 1; childId <= NUM_CHILDREN; childId++) {
	    pid = fork();

	    if(pid == -1) {
	        perror("for");
	        exit(EXIT_FAILURE);
        } else if( pid == 0 ) {
            child(childId);
            exit(EXIT_SUCCESS);
        }
	}

	//pockanie na ukoncenie detskych procesov
	for(childId = 1; childId <= NUM_CHILDREN; childId++) {
	    wait(NULL);	
	}
	printf("detske procesy ukoncene\n");

	return EXIT_SUCCESS;
}

