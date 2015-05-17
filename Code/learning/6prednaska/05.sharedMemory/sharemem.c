#include"sharemem.h"

typedef union {
	int val;
	struct semid_ds *buf;
	ushort *array;
} semun;

static int sem;			//semafor kvoli synchronizacii pristupu k zdielanej pamati
static int shmid, shmid1;
static char *shared_buffer;	//zdielana pamat
int *offset;                   //posunutie v bufry

void set_offset()
{
	*offset = *offset + 1;
}

int get_offset()
{
    return *offset;
}

char* get_offset_buf()
{
	return (shared_buffer + (*offset * sizeof(char)));
}

char* get_share_buf()
{
	return shared_buffer;
}

int unlock_semaphore()
{				//odomknutie semaforom
	struct sembuf semaphore[1];
	semaphore[0].sem_num = 0;
	semaphore[0].sem_op = +1;
	semaphore[0].sem_flg = SEM_UNDO;
	if (semop(sem, semaphore, 1) == -1) {
		perror("can't unlock semaphore");
		exit(1);
	}
	//printf("hodnota semafora= %d\n", semctl(sem,0,GETVAL,0));
	printf("UNLOCK %d\n", getpid());
	return 0;
}

int lock_semaphore()
{				//uzamknutie semaforom
	struct sembuf semaphore[1];
	printf("LOCK %d\n", getpid());
	semaphore[0].sem_num = 0;
	semaphore[0].sem_op = -1;
	semaphore[0].sem_flg = SEM_UNDO;
	if (semop(sem, semaphore, 1) == -1) {
		perror("can't lock semaphore");
		exit(1);
	}
	//printf("Proces= %d,cakaju= %d,sem=%d \n",semctl(sem,0,GETPID,0),semctl(sem,0,GETNCNT,0),semctl(sem,0,GETVAL,0));
	return 0;
}

void free_semaphor()
{				//uvolnime semafor
	if (semctl(sem, 0, IPC_RMID, 0) == -1) {
		perror("can't remove semaphore");
	}
}

int alloc_semaphor()
{				//alokovanie semafora
	sem = semget(SEM_KEY, 1, 0664 | IPC_CREAT);
	if (sem == -1) {
		perror("semget() failed");
		exit(1);
	}
	return 0;
}

int init_semaphor()
{                              //inicializacia semsfora
    semun semunion;
    ushort values[1];
    values[0] = 1;
    semunion.array = values;
    if (semctl(sem,0,SETALL, semunion) == -1){
	perror("semctl() failed");
	exit(1);
    }
    return 0;
}

int alloc_shared_mem(int size)
{				//alokovanie zdialanej pamati o velkosti size
	char *shbuf;
	shmid = shmget(SHM_KEY, size, IPC_CREAT | 0664);	//alokujeme segment zdielanej pamate
	if (shmid == -1) {
		perror("shmget() failed");
		exit(1);
	}
	shbuf = (char *) shmat(shmid, NULL, 0);
	if ((int) shbuf == -1) {
		perror("shmat() failed");
		exit(1);
	}
	shared_buffer = shbuf;
	return 0;
}
int alloc_shared_offset()
{
	char* shoff;
	shmid1 = shmget(SHM_KEY1, 1, IPC_CREAT | 0664);
	if (shmid1 == -1){
	    perror("shmget failed");
	    exit(1);
	}
	shoff = (char*) shmat(shmid1, NULL, 0);
	if ((int)shoff == -1){
	    perror("shmat failed");
	    exit(1);
	}
	offset = (int*)shoff;
	return 0;
}

void free_shared_mem()
{				//uvolnenie zdielanej pamate
	if (shmctl(shmid, IPC_RMID, 0) == -1) {
		perror("can't remove shared memory");
	}
}

void free_shared_offset()
{
	if (shmctl(shmid1, IPC_RMID, 0) == -1){
	    perror("can't remove shared offset");
	}
}

void clear_data()
{
	*((int *) shared_buffer) = 0;
}

void clear_offset()
{
	*offset = 0;
}
