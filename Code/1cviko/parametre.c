#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
//typ reprezentujuci hodnotu prepinaca (zadany, nezadany)
typedef enum {
	UNSET = 0,
	SET = 1
} OPTION;

//struktura reprezentujuca vstupne argumenty
typedef struct {
	OPTION h; //volba h (help) bez argumentu
	OPTION a; //volba a bez argumentu
	OPTION b; //volba b s povinnym argumentom
	char*  bArg; //argument volby b
	OPTION c; //volba c s povinnym argumentom
	char*  cArg;    //argument volby c ako retazec (nastavi sa pri parsovani)
	int    cIntArg; //argument volby c ako cislo   (nastavi sa pri validovani)
	OPTION d; //volba d s volitelnym argumentom
	char*  dArg; //argument volby d
} ARGS;

void printErr(char* txt) {
	fprintf(stderr,KRED"%s"KNRM,txt);
}

//vypis help-u a ukoncenie programu
void printHelpAndExit(FILE* stream, int exitCode){
	fprintf(stream, "Usage: parametre [-h] [-a | alpha] [(-b | --beta) argB] (-c | --gamma) argC [(-d | delta) [argD]]\n");
	fprintf(stream, "Program vypise nastavenie svojich vstupnych argumentov\n");
	fprintf(stream, "Prepinace:\n");
	fprintf(stream, " -h, --help   vypise help\n");
	fprintf(stream, " -a, --alpha  popis vyznamu parametra .....\n");
	fprintf(stream, " -b, --beta   popis vyznamu parametra .....\n");
	fprintf(stream, " -c, --gamma  popis vyznamu parametra .....\n");
	fprintf(stream, " -d, --delta  popis vyznamu parametra .....\n");
	exit(exitCode);
}

//parsovanie argumentov
void parseArgs(int argc, char * argv[], ARGS * args) {
	int opt;

	//inicializacia parametrov 
	args->h = UNSET;
	args->a = UNSET;
	args->b = UNSET;
	args->bArg = NULL;
	args->c = UNSET;
	args->cArg = NULL;
	args->d = UNSET;
	args->dArg = NULL;

	//parsovanie argumentov
//	printf("Debug: parsovanie argumentov:\n");
//	printf("Pocet argumentov %d\n", argc);
//	int i;
//	for(i = 0; i < argc; i++) {
//		printf("%s\n",argv[i]);
//	}

	struct option long_options[] = {
		{"help",0,NULL,'h'},
		{"alpha",0,NULL,'a'},
		{"beta",1,NULL,'b'},
		{"gamma",1,NULL,'c'},
		{"delta",2,NULL,'d'},
		{0,0,0,0}
	};
	int option_index = 0;
	do {
		opt = getopt_long(argc, argv, ":hab:c:d::",long_options,&option_index);
		printf("Debug:    opt = '%c'(%3d), optopt='%c', optarg=%s, optind=%d, opterr=%d\n",
		                         opt, opt, optopt, optarg, optind, opterr);
		//switch DOPLNTE
		
		switch(opt) {
			case 'h':
				args->h = SET;
				break;
			case 'a':
				args->a = SET;
				break;
			case 'b':
				args->b = SET;
				args->bArg = optarg;
				break;
			case 'c':
				args->c = SET;
				args->cArg = optarg;
				break;
			case 'd':
				args->d = SET;
				args->dArg = optarg;
				break;
			case ':':
				fprintf(stderr, "Nebol zadany argument prepinaca -%c\n",optopt);
				printHelpAndExit(stderr,EXIT_FAILURE);
			case '?':
				fprintf(stderr,"Neznama volba -%c\n",optopt);
				printHelpAndExit(stderr, EXIT_FAILURE);
			default:
				break;
		}
		
		
	}while(opt != -1);

	printf("Debug: zvysne argumenty:\n");
	while(optind < argc ) {
		printf("Debug:    non-option ARGV-element: %s\n", argv[optind++]);
	}
}

//validacia argumentov
void validateArgs(ARGS * args) {

	//ak nebol zadany argument prepinaca d, tak nastav jeho hodnotu na prazdny retazec
	//DOPLNTE
	if(args->d == SET && args->dArg == NULL) args->dArg = (char*) "nenastaveny";
	
	//over ci bol zadany povinny prepinac c
	//DOPLNTE
	if(args->c != SET) {
		fprintf(stderr, "Nebol zadany prepinac c\n");
		printHelpAndExit(stderr, EXIT_FAILURE);
	}	
	//skonvertuj argument prepinaca c na cislo
	//DOPLNTE
	if(sscanf(args->cArg,"%d",&args->cIntArg) <= 0) {
		printErr("Argument prepinaca nie je cislo\n");
		printHelpAndExit(stderr, EXIT_FAILURE);	
	}

}

//debug vypis zadanych argumentov
void printOptions(ARGS * args) {
	printf("vypis parametrov programu:\n");
	printf("    h: %s\n", args->h==SET?"SET":"UNSET");
	printf("    a: %s\n", args->a==SET?"SET":"UNSET");
	printf("    b: %s arg: %s\n", args->b==SET?"SET":"UNSET", args->bArg);
	printf("    c: %s arg: %s\n", args->c==SET?"SET":"UNSET", args->cArg);
	printf("    d: %s arg: %s\n", args->d==SET?"SET":"UNSET", args->dArg);
}

int main(int argc, char * argv[]) {
	printf("\n");
	ARGS args;

	parseArgs(argc, argv, &args);

	if( args.h == SET ) {
		printHelpAndExit(stdout, EXIT_SUCCESS);
	}

	validateArgs(&args);

	printOptions(&args); //tu by bola funkcionalita programu

	return EXIT_SUCCESS;
}
