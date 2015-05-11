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
	STUDENT student;
    int fd;
    umask(0);
	//vytvorenie fifo
    CHECK(mkfifo(FIFO_NAME, 0660) != -1 || errno == EEXIST);
	//otvorenie fifo na zapis
    if((fd = open(FIFO_NAME,O_WRONLY)) == -1) {
        perror("FIFO sender open");
        return EXIT_FAILURE;
    }

	//posielanie udajov
	SetStudent(&student, "Meno1", 1); //nastavenie udajov, ktore sa budu posielat
	CHECK(write(fd,&student,sizeof(student)) != -1);
	//poslanie udajov o studentovi
	SetStudent(&student, "Meno2", 2); //nastavenie udajov, ktore sa budu posielat
	CHECK(write(fd, &student, sizeof(student)) != -1);
	//poslanie udajov o studentovi

	//zatvorenie fifo
    CHECK(close(fd) != -1);
		
	return EXIT_SUCCESS;
}

