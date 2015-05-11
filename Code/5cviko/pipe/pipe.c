#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../common/student.h"
#include "../common/utility.h"

void runReader(int input, int output) {
	STUDENT student;
    close(output);

	while(read(input,&student,sizeof(student)) > 0 ){
		PrintStudent(&student);
	}
	printf("child: vsetko precitane\n");
    close(input);
	
}

void runWriter(int input, int output) {
	STUDENT student;
    close(input);
	SetStudent(& student, "meno1", 1);
	CHECK( write(output,&student, sizeof(student)) != -1);
	SetStudent(& student, "meno2", 2);
	CHECK( write(output,&student, sizeof(student)) != -1);
    close(output);
}

int main(int argc, char * argv[]) {
	pid_t pid;
	int channel[2]; //komunikacny kanal implementovany nepomenovanou rurou

	//vytvorenie nepomenovanej rury
    CHECK( pipe(channel) != -1 );

	pid = fork();
	switch(pid) {
	case -1:
		perror("fork");
		break;
	case 0:
		runReader(channel[0], channel[1]); //detsky process
		break;
	default:
		runWriter(channel[0], channel[1]); //rodicovsky process		
		wait(NULL);
	}

	return EXIT_SUCCESS;
}
