#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
char *get_current_time() {
    time_t current = time(NULL);

    return asctime(localtime(&current));
}
void close_file(int fd) {
    if(close(fd) == -1) {
        perror("Error close");
        exit(EXIT_FAILURE);
    }
}
void write_to_file(char *path, char *txt) {
    int fd = open_file(path,O_WRONLY | O_CREAT | O_APPEND);
    size_t len = strlen(txt);
    if(write(fd, txt, len*sizeof(char)) == -1) {
        perror("Error write");
        exit(EXIT_FAILURE);
    }

    close_file(fd);
}
int open_file(char *path, int flags) {
    int fd;
    if((fd = open("udaje.txt",flags)) == -1) {
        perror("Error open");
        exit(EXIT_FAILURE);
    }

    return fd;
}


void print_file(char *path) {
    int fd = open_file(path, O_RDONLY);
    
    unsigned char buff[10];
    ssize_t bytes_read;
    int i;
    do{
        if((bytes_read = read(fd, buff, sizeof(buff)))  == -1) {
            perror("Error read");
            return -1;
        }

        for(i = 0; i< bytes_read; i++) {
            printf("%c",buff[i]);
        }

    }while(bytes_read == sizeof(buff));
    printf("\nEOF\n");
    close_file(fd);
}

int main()
{
	pid_t pid;
    int fd, status;
    
    pid = fork();

    switch(pid) {
        case -1:
            perror("Error fork");
            return -1;
            break;
        case 0:
            write_to_file("udaje.txt",get_current_time());
            if(close(fd) == -1) {
                perror("Error close");
                return -1;
            }
            exit(EXIT_SUCCESS);
            break;
        default:
            wait(&status);
            if(WIFEXITED(status)) {
                print_file("udaje.txt");
            } else {
                fprintf(stderr,"Child exited with code %d", WEXITSTATUS(status));
            }
            break;
    }

} 
