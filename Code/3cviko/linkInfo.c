#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <libgen.h>

//makro pre jednoduchsiu kontrolu navratovej hodnoty volania funkcie a vypis popisu chyby
#define CHECK(command) if( ! (command) ) { fprintf(stderr, "Error: '%s' at %s line %d: %s\n", #command, __FILE__, __LINE__, strerror(errno)); exit(EXIT_FAILURE); }

//typ reprezentujuci hodnotu prepinaca (zadany, nezadany)
typedef enum {
	UNSET = 0,
    SET = 1
} OPTION;

//nastavenia programu (podla vstupnych parametrov)
typedef struct {
	OPTION help;
	char * symbolicLink; //cesta ku zadanej symbolickej linke
} SETTINGS;

//vypis help-u a ukoncenie programu
void printHelpAndExit(FILE * stream, int exitCode) {
	fprintf(stream, "Usage: linkInfo (-h | linka)\n");
	fprintf(stream, "Program vypise informacie o linke a jej ciely\n");
	fprintf(stream, "Prepinac -h vypise help\n");
	exit(exitCode);
}

//parsovanie vstupnych argumentov programu
void parseArguments(int argc, char*argv[], SETTINGS * settings) {
	int opt;
	
	//inicializacia nastaveni na defaultne hodnoty
	settings->help = UNSET;
	settings->symbolicLink = NULL;
		
	//parsovanie argumentov zacinajucich pomlckov		
	do {
		opt = getopt(argc, argv, ":h");		
		
		switch(opt) {
        case 'h': 
			settings->help = SET;
			break;
		case ':':
			fprintf(stderr, "CHYBA: nezadany argument volby '%c'\n", optopt);
			printHelpAndExit(stderr, EXIT_FAILURE);
			break;
		case '?':
			fprintf(stderr, "CHYBA: neznama volba '%c'\n", optopt);
			printHelpAndExit(stderr, EXIT_FAILURE);
			break;
		}
	}while(opt != -1);

    //parsovanie argumentov bez pomlcky
	if( settings->help == UNSET ) {		
		if( argv[optind] == NULL ) {
			fprintf(stderr, "CHYBA: nezadana vstupna linka\n");
			printHelpAndExit(stderr, EXIT_FAILURE);
		}
		
		if( argv[optind+1] != NULL ) {
			fprintf(stderr, "CHYBA: prilis vela parametrov\n");
			printHelpAndExit(stderr, EXIT_FAILURE);
		}
			
		settings->symbolicLink = argv[optind];
	}
}

//validacia vstupnych argumentov programu
void validateArguments(SETTINGS * settings) {
	//doplnte valiciu, ktora overi, ci je vstupny argument symbolicka linka

	struct stat info;
	CHECK(lstat(settings->symbolicLink, &info) != -1);

	if(!S_ISLNK(info.st_mode)) {
	    fprintf(stderr,"CHYBA: '%s' nie je symlink\n", settings->symbolicLink);
	    printHelpAndExit(stderr, EXIT_FAILURE);
    }
}

//Vypisanie informacii o symbolickej linke a jej cieli (hlavna funkcionalita programu)
void printSymbolicLinkInfo(SETTINGS * settings) {
    struct stat info;
    CHECK(lstat(settings->symbolicLink, &info) != -1);
    printf("linka: %s, velkost %d\n",settings->symbolicLink,(int) info.st_size); 
    
    // cielova adresa kam ukazuje linka
    char *link;
    link = strdup(settings->symbolicLink);
    assert(link != NULL);

    char *target;
    size_t length;
    char *directory;
    while(1) {
        printf("hladanie ciela: %s\n", link);
        CHECK( lstat(link, &info) != -1);
        if(S_ISLNK(info.st_mode) ) {
            directory = dirname(strdup(link));
            length = strlen(directory) + 1 + info.st_size + 1;
            target = (char *) malloc(length * sizeof(char));
            assert(target != NULL);
            strcpy(target, directory);
            strcat(target,"/");

            CHECK( readlink(link, target+strlen(target), info.st_size) != -1 );

            free(link);
            link = target;
        } else {
            break;
        }
    }

    printf("ciel linky: %s", link);
    free(link);
}


int main(int argc, char * argv[]) {
	SETTINGS settings;
	
	parseArguments(argc, argv, &settings);
	         
	if( settings.help == SET ) {
		printHelpAndExit(stdout, EXIT_SUCCESS);
	}
	
	validateArguments(&settings);
	
	printSymbolicLinkInfo(&settings); //hlavna funkcionalita programu

	return EXIT_SUCCESS;
}

