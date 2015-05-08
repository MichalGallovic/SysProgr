#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int child_status;
    pid_t child_pid;
    child_pid = fork();
    
    wait(&child_status);



    if(child_pid != 0) {
        sleep(15);
        printf("\nSo I end, dear parent am I with PID %d\n", getpid());
    } else {
        printf("Bye bye, im child with pid %d\n", getpid());
        printf("btw my parent is %d\n", getppid());

        exit(0);
    }

    if(WIFEXITED(child_status)) {
        printf("normal exit\n");
    } else {
        printf("abnormal with code %d",WEXITSTATUS(child_status));
    }


    return 0;    
}
