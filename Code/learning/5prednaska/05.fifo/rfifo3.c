#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/stat.h>

#define FIFO_FILE "/tmp/mY.fifo"

int main(void) {
 int fd;

 umask(0);
 mkfifo(FIFO_FILE, 0660);

 if ( (fd = open(FIFO_FILE,O_RDONLY)) == -1 ) {
   perror("open");
   return -1;
 } 

 close(0);
 dup(fd);
 close(fd);
 execlp("wc", "wc", "-l", NULL);
 perror("Error: exec wc");

 return(0);
}
