/*
Komunikacia cez mapovanu pamat

Pseudokod synchronizacie

        rodic:
             inicializuj(SEM_HW_ASSIGNED , 0) //semafor pre synchronizaciu zadania ulohy    
             inicializuj(SEM_HW_COMPLETE,  0) //semafor pre synchronizaciu odovzdania ulohy 
    
Teacher (dieta):            Student (dieta):
    zadaj_ulohu                 wait(SEM_HW_ASSIGNED)
    post(SEM_HW_ASSIGNED)       vypracuj_ulohu
    wait(SEM_HW_COMPLETE)       post(SEM_HW_COMPLETE)
    vypis_vypracovanu_ulohu     
*/

#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include "../common/utility.h"
#include "../common/homework.h"

//konstanty pre oznacenie semaforov
enum semaphores {
	SEM_HW_ASSIGNED,  //semafor pre synchronizaciu zadania domacej ulohy
	SEM_HW_COMPLETE,  //semafor pre synchronizaciu odovzdania domacej ulohy
	SEM_COUNT         //pocet semaforov
};

//union pre pracu so semaformi (definovany podla manualu)
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};



//Funkcia studenta, ktoru bude vykonavat detsky proces 
//Proces bude postupovat nasledovne: 
// - pocka na zadanie ulohy ucitelom (pouzije semafor)
// - precita a vypise ulohu na standardny vystup (udaje z mapovaneho suboru zapisane ucitelom)
// - vypracuje ulohu (doplni udaje v mapovanom subore) a vypise vypracovanu ulohu na standardny vystup
// - oznami ucitelovi, ze je uloha vypracovana (pouzije semafor)
//Vstupy: fileDescr - deskriptor suboru, ktory sa bude mapovat do pamate
//        semId     - id mnoziny semaforov
void RunStudentProcess(int fileDescr, int semId)
{
	Homework * homework; //adresa zaciatku mapovanej pamate (s mapovanou pamatou budeme pracovat ako so strukturou)
    struct sembuf op[1];

	//namapovanie suboru do pamate
	CHECK( (homework = mmap(NULL, sizeof(Homework), PROT_READ | PROT_WRITE, MAP_SHARED, fileDescr, 0) ) != (Homework*)MAP_FAILED);
	
	//zatvorenie suboru
	CHECK( close(fileDescr) != -1);
	//cakanie na zadanie ulohy (vykonanie operacie wait na semafore s cislom SEM_HW_ASSIGNED)
    op[0].sem_num = SEM_HW_ASSIGNED;
    op[0].sem_op = -1;
    op[0].sem_flg = 0;

    CHECK( semop(semId, op, 1) != -1);

	//vypis zadanej ulohy pred vypracovanim
	PrintHomework(homework);
	
	//vypracovanie ulohy
	DoHomework(homework);

	//vypis vypracovanej ulohy
	PrintHomework(homework);

	//sleep(5); //testovanie ci ucitel caka na oznamenie o vypracovani ulohy
	
	//oznamenie o vypracovani ulohy (operacia signal (post) na semafore s cislom SEM_HW_COMPLETE) 
	op[0].sem_num = SEM_HW_COMPLETE;
	op[0].sem_op = +1;
	op[0].sem_flg = 0;

	CHECK( semop(semId, op, 1) != -1);

	//odmapovanie suboru z pamate
	CHECK( munmap(homework, sizeof(Homework)) != -1);
}

//Funkcia ucitela, ktoru bude vykonavat detsky proces
//Proces bude postupovat nasledovne:
// - zada ulohu (zapise ulohu do mapovaneho suboru) a vypise ju na stdandardny vystup 
// - oznami studentovi, ze uloha je kompletne zadana
// - pocka na vypracovanie ulohy studentom
// - vypise vypracovanu ulohu na standardny vystup
//Vstupy: fileDescr - deskriptor suboru, ktory sa bude mapovat do pamate
//        semId     - id mnoziny semaforov
void RunTeacherProcess(int fileDescr, int semId)
{
	Homework * homework; //adresa zaciatku mapovanej pamate (s mapovanou pamatou budeme pracovat ako so strukturou)
    struct sembuf op[1];

    CHECK( (homework = (Homework *)mmap(NULL,sizeof(Homework),PROT_WRITE | PROT_READ, MAP_SHARED, fileDescr, 0) ) != (Homework *)MAP_FAILED);
    
    CHECK( close(fileDescr) != -1);
    	
	//ucitel vygenerovanie novu ulohu pre studenta
	GenerateRandomHomework(homework);

	//vypis zadanej ulohy
	PrintHomework(homework);

	//sleep(5); //testovanie ci student caka na oznamenie o zadani ulohy

	//oznamenie o zadani ulohy (operacia signal (post) nad semaforom s cislom SEM_HW_ASSIGNED)
    op[0].sem_num = SEM_HW_ASSIGNED;
    op[0].sem_op = +1;
    op[0].sem_flg = 0;

    CHECK( semop(semId, op, 1) != -1);

	//cakanie na vypracovanie ulohy (operacia wait nad semaforom s cislom SEM_HW_COMPLETE)
	op[0].sem_num = SEM_HW_COMPLETE;
	op[0].sem_op = -1;
	op[0].sem_flg = 0;

    CHECK( semop(semId, op, 1) != -1);
	//vypis vypracovanej ulohy
	PrintHomework(homework);

	//odmapovanie suboru z pamate
    CHECK( munmap(homework, sizeof(Homework)) != -1); 
}

int main()
{
	int semId; //id mnoziny semaforov
	pid_t pid; //pomocna premenna pre PID procesov

    unsigned short semValues[SEM_COUNT];
    union semun semUnion;
    char fileName[] = "/tmp/homeworkAwesomeXXXXXX";
    int fd;

    CHECK( (fd = mkstemp(fileName) ) != -1);

	//namapovanie suboru do pamate
	CHECK( lseek(fd, sizeof(Homework), SEEK_SET) != -1 );
	CHECK( write(fd, "",1) != -1);

	//inicializacia semaforov na nulove hodnoty
	CHECK( (semId = semget(IPC_PRIVATE, SEM_COUNT, IPC_CREAT | S_IRUSR | S_IWUSR) ) != -1);

	semValues[SEM_HW_ASSIGNED] = 0;
	semValues[SEM_HW_COMPLETE] = 0;
	semUnion.array = semValues;

	CHECK( semctl(semId, 0, SETALL, semUnion) != -1);

	//vytvorenie procesov ucitela a studenta
	CHECK( (pid = fork()) != -1);
	if( pid == 0 ) {
		RunStudentProcess(fd, semId);
		exit(EXIT_SUCCESS);
	}
	CHECK( (pid = fork()) != -1);
	if( pid == 0 ) {
		RunTeacherProcess(fd, semId);
		exit(EXIT_SUCCESS);
	}

	//zatvorenie suboru a jeho odstrananie zo suboroveho systemu 
	//(detske procesy mozu stale so suborom pracovat, pretoze ho maju otvoreny)
	CHECK( close(fd) != -1);
	CHECK( unlink(fileName) != -1);

	//cakanie na ukoncenie procesov ucitela a studenta
	CHECK( wait(NULL) != -1);
	CHECK( wait(NULL) != -1);

	//odstranenie mnoziny semaforov
    CHECK( semctl(semId, 0, IPC_RMID) != -1);	
	return EXIT_SUCCESS;
}
