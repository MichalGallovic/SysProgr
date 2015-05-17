#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

#define SEG_SIZE 0x6400

int seg_id;

int main() {


    char *shared_mem;
    struct shmid_ds shmbuf;
    int count;
    pid_t pid;
    
    seg_id = shmget(IPC_PRIVATE, SEG_SIZE, IPC_CREAT | 0600);
    if(seg_id == -1) {
        perror("shmget:");
        return -1;
    }

    shared_mem = (char *)shmat(seg_id, 0,0);

    if(shared_mem == (void*)-1) {
        perror("shmat:");
    }

    fprintf(stderr, "shared mem attached address %p\n", shared_mem);
    sprintf(shared_mem,"%d", 2000);
    
    pid = fork();
    
    switch (pid) {
    	case -1:
            exit(EXIT_FAILURE);
        case 0:
            count = atoi(shared_mem) + 100;
            sprintf(shared_mem,"%d",count);
            if(shmdt(shared_mem) == -1) {
                perror("shmdt:");
                return -1;
            }
            exit(EXIT_SUCCESS);
            break;
        default:
            wait(NULL);
            break;
    }

    

    

    shared_mem = (char *)shmat(seg_id,0,0);

    if(shared_mem == (void *)-1) {
        perror("shamt 0:");
        return -1;
    }

    fprintf(stderr,"shared mem re-attached at addr %p\n", shared_mem);

    fprintf(stderr, "shared mem content '%s'\n", shared_mem);

    if(shmdt(shared_mem) == -1) {
        perror("shmdt:");
        return -1;
    }

    if(shmctl(seg_id, IPC_RMID, NULL) == -1) {
        perror("shmctl IPCRMID:");
        return -1;
    }

    return -1;
}
