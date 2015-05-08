#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

char *get_timestamp() {
    time_t now = time(NULL);
    return asctime(localtime(&now));
}

int main(int argc, char *argv[]) {
    char *path;
    char *ts;
    int len;

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    int fd;

//    if(argc < 2) {
//        fprintf(stderr,"Error: nespravny pocet argumentov\n");
//        return -1;
//    }
//   
//    path = argv[1];
//    ts = get_timestamp();
//    len = strlen(ts);
//
//    fd = open(path,O_WRONLY | O_CREAT | O_APPEND, mode);
//    if(fd == -1) {
//        perror("open");
//    }
//
//    if(write(fd, ts, len) != len)
//        perror("write");
//    if(close(fd) == -1)
//        perror("close");
    
//    fd = open(path, O_RDONLY);
//    size_t offset = 0;
//    size_t bytes_read = 0;
//    unsigned char buf[16];
//    int i;
//    do {
//        bytes_read = read(fd,buf, sizeof(buf));
//        printf("0x%06X ", offset);
//        for(i = 0;i < bytes_read; i++) 
//            printf(" %02X", buf[i]);
//        printf("\n");
//        offset += bytes_read;
//    } while( bytes_read == sizeof(buf));
//
//    if(close(fd) == -1)
//        perror("close");
// 
//    if((fd = open(path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR)) == -1) {
//        perror("open");
//        return -1;
//    }
//    if( close(1) == -1) {
//        perror("close stdout");
//        return -2;
//    }
//    if(dup(fd) == -1) {
//        perror("dup");
//        return -3;
//    }
//    if(close(fd) == -1) {
//        perror("close fd");
//        return -4;
//    }
//
//    fprintf(stdout, "Takze toto je teraz v subore\n");
//    printf("A tym padom aj toto\n");
    
//    if(argc != 3) {
//       perror("Missing arguments: input + output file needed\n");
//       fprintf(stderr,"you gave me %d arguments \n",argc -1 );
//       return -1;
//    }
//
//    struct stat stat_buf;
//    int open_fd = open(argv[1], O_RDONLY);
//    off_t offset = 0;
//    fstat(open_fd,&stat_buf);
//    int write_fd = open(argv[2], O_WRONLY | O_CREAT, stat_buf.st_mode);
//    printf("%d\n",stat_buf.st_size);
//   if( sendfile(write_fd,open_fd, &offset, stat_buf.st_size) == -1) {
//       perror("Error: writing to file failed");
//       return -1;
//   }
//   
//   close(open_fd);
//   close(write_fd);
//
    char *wd = NULL;
    int ret = 0;
    
    wd = getcwd(wd,0);
    if(wd == NULL) {
        fprintf(stderr,"getcwd %d",errno);
        return -1;
    }

    printf("The working dir is: '%s'\n",wd);

    printf("Changind dir '../../../\n");

    free(wd);

    if(chdir("../../../") == -1) {
        perror("chdir");
    }

    wd = NULL;
    wd = getcwd(wd, 0);
    if(wd == NULL ) {
        perror("getcwd");
        return -1;
    }

    printf("The working dir is: '%s'\n",wd);

    free(wd);
    return 0;
}
