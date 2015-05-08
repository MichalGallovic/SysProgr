#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int filter(const struct dirent *entry) {
    char *ptr;

    if( !strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..") )
        return 0;

    ptr = rindex(entry->d_name, '.');
    if( ptr && (!strcmp(ptr,".c") || !strcmp(ptr,".h") ) )
        return 1;

    return 0;
}

int main(int argc, char *argv[]) {
    struct dirent **name_list;
    int n;

    if(argc != 2) {
        fprintf(stderr, "Error: nespravny pocet argumentov\n");
        return -1;
    }

    n = scandir( argv[1], &name_list, filter, alphasort);
    
    if(n == -1) {
        perror("scandir");
        return -1;
    }
    while(n--) {
        printf("%s\n", name_list[n]->d_name);
        free(name_list[n]);
    }
    free(name_list);

    return 0;

}
