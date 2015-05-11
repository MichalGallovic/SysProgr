#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_FILE "/tmp/mY.fifo"

int main(void) {
 int fd;

 umask(0);
 mkfifo(FIFO_FILE, 0660);

 if((fd = open(FIFO_FILE, O_WRONLY)) == -1) {
   perror("open");
   return -1;
 } 

 close(STDOUT_FILENO);
 dup(fd);
 close(fd);
 execlp("who", "who", NULL); 
 perror("Error: exec who");

 return(0);
}
