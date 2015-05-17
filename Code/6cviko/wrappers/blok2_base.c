#include "sem_wrapper.h"
#include "shm_wrapper.h"

// makra, struktury, konstanty, ...
typedef enum {
    UNSET=0,
    SET=1
} OPTION;

typedef struct {
    OPTION h;
    OPTION c;
    char * countArgStr;
    int  countArg;
} ARGS;

int childTerminationFlag = 0;   // flag, ktory podmienuje infinite loop child procesu, nastavi sa po prijati signalu

/*************************** deklaracie funkcii **********************************/

// vypise nacitane argumenty
void printArgs(ARGS * args);
// vypise help
void printHelp(FILE* stream);
// vypis ehelp a ukonci program
void printHelpAndExit(FILE* stream, int exitCode);
// nacita vstupne prepinace/argumenty
void parseArguments(int argc, char * argv[], ARGS * args);
// zvaliduje vstupne argumenty
void validateArguments(ARGS * args);
// zresetuje hodnoty argumentov
void resetArguments(ARGS * args);

// vytvori countArg pocet procesov, ktore budu synchronizovane mnozinou semaforou, vrati mnozinu ich pid
void processProgramLogic(pid_t * childrenPids, int childrenNum);
// pocka na ukoncovaci signal - SIGINT
void waitForTerminationSignal();
// prisiel signal na ukoncenie, preposli tento signal vsetkym child procesom
void sendTermSignalToChildren(pid_t * childrenPids, int childrenNum);
// pocka na ukoncenie vsetkych child procesov
void waitForChildrenTermination(int childrenNum);
// synchonizovane pomocou semaforou zapisuje pid do pamate/na obrazovku v infinite loope, 
// ukonci sa po prijati signalu
void processChild(int childId);
// nastavi flag podmienujuci infitie loop child procesu na 1
void sigUsr1Handler();

/************************************ main ***************************************/

int main(int argc, char * argv[]){
    ARGS args;

    // spracuje vstupne prepinace/argumenty
    resetArguments(&args);
    parseArguments(argc, argv, &args);
    validateArguments(&args);
    // printArgs(&args);

    // inicializuje mnozinu semaforov
    initSem(args.countArg);

    // vytvori zdielanu pamate
    initShm();

    // vytvori countArg pocet procesov, ktore budu synchronizovane mnozinou semaforou, vrati mnozinu ich pid
    pid_t * childrenPids = (pid_t *) malloc(args.countArg * sizeof(pid_t));
    processProgramLogic(childrenPids, args.countArg);

    // pocka na ukoncovaci signal - SIGINT
    waitForTerminationSignal();

    // prisiel signal na ukoncenie, preposli tento signal vsetkym child procesom
    sendTermSignalToChildren(childrenPids, args.countArg);

    // pocka na ukoncenie vsetkych child procesov
    waitForChildrenTermination(args.countArg);
    printf("Vsetky detske procesy ukoncene\n");

    // uprace pamat v ramci mnoziny semaforov
    deinitSem();

    // zmaze zdielanu pamat
    deinitShm();

    return EXIT_SUCCESS;
}

/***************************** definicie funkcii *********************************/

// synchonizovane pomocou semaforou zapisuje pid do pamate/na obrazovku v infinite loope, 
// ukonci sa po prijati signalu
void processChild(int childId) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigUsr1Handler;

    // registruje handler pre SIGUSR1, ktory 
    CHECK( sigaction(SIGINT, &sa, NULL) != -1);

    // pointer na zdielanu pamat
    shm_data * data;

    printf("childId: %d, child pid: %d, wait for getSemId(childId): %d\n", childId, getpid(), getSemId(childId));

    // spravi synchronizaciu cez semafory, vypise pid
    while(childTerminationFlag == 0) {
        waitSem( childId ); // caka na uvolnenie svojho semaforu
        printf("pid: %d\n", getpid());
        data = connectShm();    // pripji zdielanu pamat
        //printf("pid-read: %d\n", data->pid);
        data->pid = getpid();
        //printf("pid-write: %d\n", data->pid);
        sleep(2);
        disconnectShm();    // odpoji zdielanu pamat

        enableSem( getSemId(childId + 1) ); // uvolni dalsi semafor
    }
}

// nastavi flag podmienujuci infitie loop child procesu na 1
void sigUsr1Handler() {
    printf("handler\n");
    childTerminationFlag = 1;   
}

// vytvori countArg pocet procesov, ktore budu synchronizovane mnozinou semaforou, vrati mnozinu ich pid
void processProgramLogic(pid_t * childrenPids, int childrenNum) {
    pid_t pid;

    int childId;
    // vytvori childrenNum detskych procesov
    for(childId = 0; childId < childrenNum; childId++) {
        CHECK( (pid = fork()) != -1 );
        switch(pid) {
            case 0: // child
                processChild(childId);
                exit(EXIT_SUCCESS);
            default:    // parent
                childrenPids[childId] = pid;    // uloz pid child procesu
                break;
        }
    }
    enableSem( getSemId(0) );   // povoli prvy semafor
}

// pocka na ukoncovaci signal - SIGINT
void waitForTerminationSignal() {
    int sigNum;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR2);

    CHECK( sigprocmask(SIG_BLOCK, &set, NULL) != -1 );
    CHECK( sigwait(&set, &sigNum) != -1 );
}

// prisiel signal na ukoncenie, preposli tento signal vsetkym child procesom
void sendTermSignalToChildren(pid_t * childrenPids, int childrenNum) {
    int childId;
    for(childId = 0; childId < childrenNum; childId++) {
        kill(childrenPids[childId], SIGINT);
    }
}

// pocka na ukoncenie vsetkych child procesov
void waitForChildrenTermination(int childrenNum) {
    int childId;
    for(childId = 0; childId < childrenNum; childId++) {
        wait(NULL);
    }
}

// vypise nacitane argumenty
void printArgs(ARGS * args) {
    printf("Ncitane parametre: \n");
    printf("\thelp: %d\n", args->h);
    printf("\tcountArgStr: %s\n", args->countArgStr);
    printf("\tcountArg: %d\n", args->countArg);
}

// vypise help
void printHelp(FILE* stream) {
    fprintf(stream, "Usage: parametre [-h | --help] -c|--count <countArg>\n");
    fprintf(stream, "Prepinace:\n");
    fprintf(stream, " -h, --help  vypise help\n");
    fprintf(stream, " -c, --count <countArg> vytvori countArg pocet semaforov\n");
}

// vypis ehelp a ukonci program
void printHelpAndExit(FILE* stream, int exitCode) {
    printHelp(stream);
    exit(exitCode);
}

// nacita vstupne prepinace/argumenty
void parseArguments(int argc, char * argv[], ARGS * args) {
    int opt;
    struct option longOptions[] = {
        {"help" , no_argument, NULL, 'h'},
        {"count", no_argument, NULL, 'c'},
        {NULL   , 0          , NULL, 0  }
    };

    // nacitaj parametre s prepinacom
    do {
        opt = getopt_long(argc, argv, ":hc:", longOptions, NULL);
        switch(opt) {
            case 'h':
                args->h = SET;
                printHelp(stdout);
                break;
            case 'c':
                args->c = SET;
                args->countArgStr = optarg;
                break;
            case ':':
                fprintf(stderr, "Chyba povinny argument prepinaca: %c\n", optopt);
                printHelpAndExit(stderr, EXIT_FAILURE);
            case '?':
                fprintf(stderr, "Neznamy prepinac: %c\n", optopt);
                printHelpAndExit(stderr, EXIT_FAILURE);
                break;
        }
    } while (opt != -1);

    /*
    // nacitaj ostatne prepinace
    while (optind < argc) {
    arg = argv[optind];
    if (args->path == UNSET && arg != NULL) {
    args->path = SET;
    args->pathArg = arg;
    } else {
    printf("Ignorovany argument bez prepicana: %s\n", arg);
    }
    optind++;
    }
     */
}

// zvaliduje vstupne argumenty
void validateArguments(ARGS * args) {
    if (args->c == UNSET) {
        printf("CHYBA: nebol zadany prepinac -c\n");
        printHelpAndExit(stderr, EXIT_FAILURE);
    }

    if (sscanf(args->countArgStr, "%d", &args->countArg) != 1) {
        printf("CHYBA: argument prepinaca -c musi byt cislo\n");
        printHelpAndExit(stderr, EXIT_FAILURE);
    }
}

// zresetuje hodnoty argumentov
void resetArguments(ARGS * args) {
    args->h = UNSET;
    args->c = UNSET;
    args->countArgStr = "default";
    args->countArg = 0;
}
