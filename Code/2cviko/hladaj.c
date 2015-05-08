#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>

typedef enum {
    UNSET = 0,
    SET = 1
} OPTION;

typedef struct {
    OPTION a;
    OPTION b;
    char *argB;
} ARGS;


void parseArguments(int argc, char *argv[], ARGS *args) {
    args->a = UNSET;
    args->b = UNSET;
    args->argB = NULL;

    int opt;

    do{
    }while(opt != -1);

}

int main(int argc, char *argv[]) {

    if(argc < 4) {
        perror("Nespravny pocet argumentov");
        exit(EXIT_FAILURE);
    }


    char *path = argv[1];
    char *arg = argv[2];
    char *what = argv[3];
    
    char *arglist[] = {
        "find",
        path,
        arg,
        what,
        NULL
    };

    execvp("find",arglist);

    perror("find error");
	return EXIT_FAILURE;
}

	
