#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "commonFifo.h"

int main(int argc, char * argv[]) 
{
	STUDENT student; //struktura pre docasne ulozenie precitanych udajov 
    int fd, readResult;
    umask(0);
    CHECK(mkfifo(FIFO_NAME, 0660) != -1 || errno == EEXIST);
    // otvorenie fifo na zapis
    CHECK( (fd = open(FIFO_NAME,O_RDONLY) ) != -1);
    CHECK(unlink(FIFO_NAME) != -1);
	//citanie udajov a vypis udajov
	while( (readResult = read(fd, &student, sizeof(STUDENT) )) == sizeof(STUDENT)) {
		PrintStudent(&student);
	}
	if(readResult == -1 ){
	    perror("read");
	    exit(EXIT_FAILURE);
	}
	printf("vsetky udaje prijate\n");

	//zatvorenie fifo
    CHECK(close(fd) != -1);
	return EXIT_SUCCESS;
}

