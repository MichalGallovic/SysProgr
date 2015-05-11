#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define CHECK(command) if( ! (command) ) { fprintf(stderr, "Error: '%s' at %s line %d: %s\n", #command, __FILE__, __LINE__, strerror(errno)); exit(EXIT_FAILURE); }
int main() {
    const char fileName[] = "vstup.txt";
    int file;
    char buffer[10];
    int numRead;

    CHECK( (file = open(fileName,O_RDONLY) ) != -1);
    CHECK( unlink(fileName) != -1);
    CHECK( (numRead = read(file, buffer, 10) ) != -1);
    CHECK( close(file) != -1);

    write(STDOUT_FILENO, buffer, numRead);
    write(STDOUT_FILENO, "\n", 1);

}
