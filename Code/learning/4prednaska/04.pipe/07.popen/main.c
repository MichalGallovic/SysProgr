#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
//	FILE *stream = popen("sort","w");
//
//	fprintf(stream, "zzz asd 07\n");
//	fprintf(stream, "www asd 06\n");
//	fprintf(stream, "eee asd 04\n");
//	fprintf(stream, "rrr asd 05\n");
//	fprintf(stream, "ttt asd 02\n");
//	fprintf(stream, "hhh asd 01\n");
//
//	pclose(stream);

    FILE *stream = popen("ls -la ../", "r");
    char buffer[1024];
    int fd = open("out.txt", O_WRONLY | O_CREAT,0640);
    dup2(fd, STDOUT_FILENO);
    while(!feof(stream) && !ferror(stream)){
        if(fgets( buffer, sizeof(buffer), stream) == NULL)
            exit(EXIT_FAILURE);
        fputs(buffer, stdout);
    }
    pclose(stream);
	return 0;
}
