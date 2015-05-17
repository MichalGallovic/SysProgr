#include "shm.h"

static int verbose = NO_VERBOSE;

void set_verbose( int val ) {
	if ( ( val != VERBOSE ) && ( val != NO_VERBOSE ) )
		return;
	verbose = val;
}

int open_sem(key_t key) {
	int sem;
	sem = semget( key, 1, IPC_CREAT | 0600 );
	if ( ( sem == -1 ) && ( verbose == VERBOSE ) )
		perror( "semget:" );
	return sem;
}

int init_sem(int sem) {
	int ret;
	semun semunion;
	ushort val[1];

	val[0] = 1;
	semunion.array = val;
	ret = semctl( sem, 0, SETALL, semunion );
	if ( ( ret == -1 ) && ( verbose == VERBOSE ) )
		perror( "init_sem:" );
	return ret;
}

int lock_sem(int sem) {
	struct sembuf buf[1];
	int ret;

	buf[0].sem_num = 0;
	buf[0].sem_op = -1;
	buf[0].sem_flg = SEM_UNDO;
	ret = semop( sem, buf, 1 );
	if ( ( ret == -1 ) && ( verbose == VERBOSE ) )
		perror( "lock_sem:" );

	return ret;
}

int unlock_sem(int sem) {
	struct sembuf buf[1];
	int ret;

	buf[0].sem_num = 0;
	buf[0].sem_op = +1;
	buf[0].sem_flg = SEM_UNDO;
	ret = semop( sem, buf, 1 );
	if ( ( ret == -1 ) && ( verbose == VERBOSE ) )
		perror( "lock_sem:" );

	return ret;
}

int free_sem(int sem) {
	int ret;
	ret = semctl( sem, 0, IPC_RMID, 0 );
	if ( ( ret == -1 ) && ( verbose == VERBOSE ) )
		perror( "free_sem:" );
	return ret;
}

char * open_shm(key_t key, int size, int *shmid) {
	char *shm = NULL;

	*shmid = shmget( key, size, IPC_CREAT | 0600 );
	if ( *shmid == -1 ) {
		if ( verbose == VERBOSE )
			perror( "open_shm: shmget:" );
		return NULL;
	}

	shm = shmat( *shmid, NULL, 0 );
	if ( shm == (void*) -1 ) {
		if ( verbose == VERBOSE )
			perror( "open_shm: shmat:" );
		return NULL;
	}

	return shm;
}

int close_shm(char * shm) {
	int ret;
	ret = shmdt( shm );
	if ( ( ret == -1 ) && ( verbose == VERBOSE ) )
		perror( "close_shm:" );
	return ret;
}

int free_shm(int shmid) {
	int ret = 0;
	ret = shmctl( shmid, IPC_RMID, 0 );
	if ( ( ret == -1 ) && ( verbose == VERBOSE ) )
		perror( "free_shm:" );
	return ret;
}

/*****************************************************************
 *
 * S H A R E D    O B J E C T    H A N D L I N G    L I B R A R Y
 *
 *****************************************************************/

#define		E_OBJCNT	-1

#define		SHARED_MAX	128

typedef struct _s {
	int	shmid;
	char 	*shmptr;
	int	semid;
} shared_s;

static	int		shared_cnt = 0;
static	shared_s	shared_obj[SHARED_MAX];
static	int		shared_semid;

int shared_init() {
	int i;
	for ( i=0; i<SHARED_MAX; i++ ) {
		shared_obj[i].shmid = -1;
		shared_obj[i].shmptr = NULL;
		shared_obj[i].semid = -1;
	}
	shared_cnt = 0;

	
}

int shared_create( key_t key, int size ) {	
	int id;

	// ak je uz vytvorenych max. pocet zdielanych pamatovych objektov
	if ( shared_cnt == 128 )
		return E_OBJCNT;

	// najdeme prvy volny slot
	id = 0;
	while ( shared_obj[id].shmptr != NULL )
		id++;
	
	// vytvorime 
	return id;
}

int shared_destroy( int id ) {
}
