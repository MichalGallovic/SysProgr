#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

//makro pre jednoduchsiu kontrolu navratovej hodnoty volania funkcie a vypis popisu chyby
#define CHECK( command ) if( ! (command) ) { fprintf(stderr, "Error: '%s' at %s line %d: %s\n", #command, __FILE__, __LINE__, strerror(errno)); exit(EXIT_FAILURE); }
//typ reprezentujuci hodnotu prepinaca (zadany, nezadany)
typedef enum {
	UNSET = 0,
    SET = 1
} OPTION;

//nastavenia programu (podla vstupnych parametrov)
typedef struct {
	OPTION help;
	char * outputFile; //cesta ku vystupnemu suboru
} SETTINGS;

//vypis help-u a ukoncenie programu
void printHelpAndExit(FILE * stream, int exitCode) {
	fprintf(stream, "Usage: presmerovanie (-h | vystupny_subor)\n");
	fprintf(stream, "Program vypise do suboru vystupny_subor procesy pouzivatela\n");
	fprintf(stream, "Prepinac -h vypise help\n");
	exit(exitCode);
}

//parsovanie vstupnych argumentov programu
void parseArguments(int argc, char*argv[], SETTINGS * settings) {
	int opt;
	
	//inicializacia nastaveni na defaultne hodnoty
	settings->help = UNSET;
	settings->outputFile = NULL;
		
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
			fprintf(stderr, "CHYBA: nezadany vystupny subor\n");
			printHelpAndExit(stderr, EXIT_FAILURE);
		}
		
		if( argv[optind+1] != NULL ) {
			fprintf(stderr, "CHYBA: prilis vela parametrov\n");
			printHelpAndExit(stderr, EXIT_FAILURE);
		}
			
		settings->outputFile = argv[optind];
	}
}

int main(int argc, char * argv[]) {
	SETTINGS settings;

	parseArguments(argc, argv, &settings);
	         
	if( settings.help == SET ) {
		printHelpAndExit(stdout, EXIT_SUCCESS);
	}

	//doplnte otovorenie suboru na zapis
    int fd;
    CHECK( (fd = open(settings.outputFile, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR )) != -1 );
	//zatvorte standardny vystup
    CHECK( close(STDOUT_FILENO) != -1 );
	//vytvorte kopiu deskriptoru suboru tak, aby bola kopia bola v tabulke otvorenych suborov na pozicii standardneho vystup
    CHECK( dup2(fd, STDOUT_FILENO) != -1 );
	//zatvorte deskriptor suboru	
	CHECK( close(fd) != -1);
	printf("Zoznam aktualnych procesov:\n");
	fflush(stdout);
	//doplte nahradenie obrazu aktualneho procesu, obrazom procesu "ps u"	
    execlp("ps","ps","u",NULL);

    perror("Execlp");

	return EXIT_FAILURE;
}
	
