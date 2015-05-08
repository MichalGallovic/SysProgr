#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

char *get_file_type( const char *path) {
    struct stat st;

    if( lstat(path, &st) == -1) {
        fprintf(stderr, "Error: lstat('%s'): %s\n", path, strerror(errno));
        return NULL;
    }

    if( S_ISLNK(st.st_mode) )
        return "symbolic link";
    else if ( S_ISDIR(st.st_mode) )
        return "directory";
    else if( S_ISCHR(st.st_mode) )
        return "character device";
    else if( S_ISBLK(st.st_mode) )
        return "block device";
    else if( S_ISFIFO(st.st_mode) )
        return "named fifo";
    else if( S_ISSOCK(st.st_mode) )
        return "socket";
    else if( S_ISREG(st.st_mode) )
        return "regular file";

    return "unknown file type";
}


int main (int argc, char *argv[]) {

    char *dir_path;
    char *type;
    DIR *dir;
    struct dirent *entry;
    char entry_path[PATH_MAX+1];
    size_t path_len;


    if(argc != 2) {
        perror("Error: nespravny pocet argumentov\n");
        return -1;
    }

    dir_path = argv[1];

    strncpy(entry_path, dir_path, PATH_MAX);
    entry_path[PATH_MAX] = '\0';

    path_len = strlen(entry_path);
    if(entry_path[path_len-1] != '/') {
        if(path_len == PATH_MAX) {
            fprintf(stderr,"Error: cesta je prilis dlha\n");
            return -1;
        }
        entry_path[path_len] = '/';
        path_len++;
        entry_path[path_len] = '\0';
    }
    dir  = opendir(dir_path);
    if( dir == NULL ) {
        perror('opendir');
        return -1;
    }

    while((entry = readdir(dir)) != NULL) {
        strncpy(entry_path + path_len, entry->d_name, PATH_MAX - path_len);
        entry_path[PATH_MAX] = '\0';

        type = get_file_type(entry_path);
        if( type == NULL )
            continue;
        printf("%-18s: %s\n", type, entry_path);
    }


    return 0;

}
