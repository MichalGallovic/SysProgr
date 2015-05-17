#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include "shared.h"

/*****************************************************************
 *
 * S H A R E D    O B J E C T    H A N D L I N G    L I B R A R Y
 *
 *****************************************************************/

#define		SHARED_MAX	128

#define		SHARED_LOCKED		1
#define		SHARED_NO_LOCKED	0

typedef struct _s {
	int	shmid;
	char 	*shmptr;
	key_t	key;
	int	size;
} shared_s;

typedef union _semun {
	unsigned short int *array;
} semun;

static	int		shared_initialized = 0;
static	int		shared_cnt = 0;
static	shared_s	shared_obj[SHARED_MAX];
static	int		shared_semid;
static	int 		shared_verb = SHARED_NO_VERB;

static int shared_lock_internal(int semid, int i) {
	struct sembuf buf[1];
	int ret;

	buf[0].sem_num = i;
	buf[0].sem_op = -1;
	buf[0].sem_flg = SEM_UNDO;
	while ( 1 ) {
		ret = semop( semid, buf, 1 );
		if ( ret == -1 ) {
			if ( errno == EINTR )
				continue;
			if ( shared_verb == SHARED_VERB ) {	
				perror( "shared_lock_sem_internal" );
				return SHARED_E_SEM;
			}
		}
		break;
	}
	return SHARED_OK;
}

static int shared_unlock_internal(int semid, int i) {
	struct sembuf buf[1];
	int ret;

	buf[0].sem_num = i;
	buf[0].sem_op = +1;
	buf[0].sem_flg = SEM_UNDO;
	while ( 1 ) {
		ret = semop( semid, buf, 1 );
		if ( ret == -1 ) {
			if ( errno == EINTR )
				continue;
			if ( shared_verb == SHARED_VERB ) {	
				perror( "shared_unlock_sem_internal" );
				return SHARED_E_SEM;
			}
		}
		break;
	}
	return SHARED_OK;
}

int shared_attach_internal( int id ) {
	shared_obj[id].shmptr = shmat( shared_obj[id].shmid, NULL, 0 );
	if ( shared_obj[id].shmptr == (void*) -1 ) {
		if ( shared_verb == SHARED_VERB )
			perror( "shared_attach" );
		return SHARED_E_SHM;
	}
	return SHARED_OK;
}

int shared_detach_internal( int id ) {
	int ret;
	ret = shmdt( shared_obj[id].shmptr );
	if ( ( ret == -1 ) && ( shared_verb == SHARED_VERB ) ) {
		perror( "shared_detach" );
		return SHARED_E_SHM;
	}
	shared_obj[id].shmptr = (void*)-1;
	return SHARED_OK;
}

int shared_destroy_internal( int id ) {
	int ret = 0;

	// ak este nebol segment odpojeny z virtualneho priestoru procesu, zle
	if ( shared_obj[id].shmptr != (void*)-1 )
		return SHARED_E_INVAL;

	ret = shmctl( shared_obj[id].shmid, IPC_RMID, 0 );
	if ( ( ret == -1 ) && ( shared_verb == SHARED_VERB ) ) {
		perror( "shared_destroy" );
		return SHARED_E_SHM;
	}
	shared_obj[id].shmid = -1;
	shared_obj[id].size = 0;
	shared_obj[id].key = IPC_PRIVATE;
	shared_cnt--;
	return SHARED_OK;
}

int shared_read_internal( int id, int from, void *buf, int size, int locked ) {
	int ret;

	// ak nie je libka inicializovana, cele zle
	if ( shared_initialized == 0 )
		return SHARED_E_INIT;
	// ak je id mimo rozsahu, tiez zle
	if ( (id<0) || (id>SHARED_MAX) )
		return SHARED_E_INVAL;
	// ak pre dane id nie je inicializovany objekt, zle
	if ( shared_obj[id].shmid == -1 )
		return SHARED_E_ID;
	// ak objekt nie je pripojeny, zle
	if ( shared_obj[id].shmptr == (void*) -1 )
		return SHARED_E_PTR;
	// ak je 'from' zaporne alebo vacsie ako velkost objektu, zle
	if ( (from<0) || (from>shared_obj[id].size-1) )
		return SHARED_E_ARG;
	// ak je citana velkost vacsia nez velkost zdielaneho objektu, zle
	if ( shared_obj[id].size < size+from )
		return SHARED_E_TOOBIG;

	if ( locked == SHARED_LOCKED ) {
		ret = shared_lock_internal( shared_semid, id );
		if ( ret != SHARED_OK )
			return ret;
	}

	memcpy( buf, shared_obj[id].shmptr+from, size );

	if ( locked == SHARED_LOCKED ) {
		ret = shared_unlock_internal( shared_semid, id );
		if ( ret != SHARED_OK )
			return ret;
	}

	return SHARED_OK;
}

int shared_write_internal( int id, int from, const void *buf, int size, int locked ) {
	int ret;

	// ak nie je libka inicializovana, cele zle
	if ( shared_initialized == 0 )
		return SHARED_E_INIT;
	// ak je id mimo rozsahu, tiez zle
	if ( (id<0) || (id>SHARED_MAX) )
		return SHARED_E_INVAL;
	// ak pre dane id nie je inicializovany objekt, zle
	if ( shared_obj[id].shmid == -1 )
		return SHARED_E_ID;
	// ak objekt nie je pripojeny, zle
	if ( shared_obj[id].shmptr == (void*) -1 )
		return SHARED_E_PTR;
	// ak je 'from' zaporne alebo vacsie ako velkost objektu, zle
	if ( (from<0) || (from>shared_obj[id].size-1) )
		return SHARED_E_ARG;
	// ak je zapisovana velkost nez velkost zdielaneho objektu, zle
	if ( shared_obj[id].size < size+from )
		return SHARED_E_TOOBIG;

	if ( locked == SHARED_LOCKED ) {
		ret = shared_lock_internal( shared_semid, id );
		if ( ret != SHARED_OK )
			return ret;
	}

	memcpy( shared_obj[id].shmptr+from, buf, size );

	if ( locked == SHARED_LOCKED ) {
		ret = shared_unlock_internal( shared_semid, id );
		if ( ret != SHARED_OK )
			return ret;
	}

	return SHARED_OK;
}

void shared_init_fork() {
	int i;
	for ( i = 0; i < SHARED_MAX; i++ )
		shared_obj[i].shmptr = (void*) -1;
}

void shared_set_verbose( int val ) {
	if ( ( val != SHARED_VERB ) && ( val != SHARED_NO_VERB ) )
		return;
	shared_verb = val;
}

int shared_init( key_t key, int initialize ) {
	semun semunion;
	ushort val[SHARED_MAX];
	int i, ret;

	if ( (initialize != SHARED_INITIALIZE) && (initialize != SHARED_NO_INITIALIZE) )
		return SHARED_E_ARG;

	for ( i=0; i<SHARED_MAX; i++ ) {
		shared_obj[i].shmid = -1;
		shared_obj[i].shmptr = (void*) -1;
		shared_obj[i].size = 0;
		shared_obj[i].key = IPC_PRIVATE;
	}
	shared_cnt = 0;

	// vytvorime semafory pre vsetky potencialne zdielane objekty
	shared_semid = semget( key, SHARED_MAX, IPC_CREAT | 0600 );
	if ( ( shared_semid == -1 ) && ( shared_verb == SHARED_VERB ) ) {
		perror( "shared_init: semget" );
		return SHARED_E_SEM;
	}

	if ( initialize == SHARED_INITIALIZE ) {
		// inicializujeme binarne semafory na hodnotu 1
		for ( i = 0; i < SHARED_MAX; i++ )
			val[i] = 1;
		semunion.array = val;
		ret = semctl( shared_semid, 0, SETALL, semunion );
		if ( ( ret == -1 ) && ( shared_verb == SHARED_VERB ) ) {
			perror( "shared_init: semctl SETALL to 1" );
			semctl( shared_semid, 0, IPC_RMID, NULL );
			return SHARED_E_SEM;
		}
	}

	if ( pthread_atfork( NULL, NULL, shared_init_fork ) != 0 ) {
		if ( shared_verb == SHARED_VERB )
			perror( "shared_init: atfork" );
	}

	shared_initialized = 1;
	return SHARED_OK;
}

int shared_fini() {
	int id, ret;

	for ( id = 0; id < SHARED_MAX; id++ ) {
		if ( shared_obj[id].shmid != -1 ) {
			if ( shared_obj[id].shmptr != (void*)-1 )
				shared_detach_internal( id );
			shared_destroy_internal( id );
		}
	}

	ret = semctl( shared_semid, 0, IPC_RMID, 0 );
	if ( ( ret == -1 ) && ( shared_verb == SHARED_VERB ) ) {
		perror( "shared_fini" );
		return SHARED_E_SEM;
	}

	shared_initialized = 0;
	return SHARED_OK;
}

int shared_create( key_t key, int size ) {	
	int id;

	// ak nie je libka inicializovana, cele zle
	if ( shared_initialized == 0 )
		return SHARED_E_INIT;

	// ak je uz vytvorenych max. pocet zdielanych pamatovych objektov
	if ( shared_cnt == 128 )
		return SHARED_E_OBJCNT;

	// zistime, ci uz nahodou nemame segment s takym klucom vytvoreny
	for ( id = 0; id < 128; id++ )
		if ( (key != IPC_PRIVATE ) && (shared_obj[id].key == key) )
			return id;

	// najdeme prvy volny slot
	id = 0;
	while ( (id < 128) && ( shared_obj[id].shmid != -1 ) )
		id++;
	
	// vytvorime zdielany segment
	shared_obj[id].shmid = shmget( key, size, IPC_CREAT | 0600 );
	if ( shared_obj[id].shmid == -1 ) {
		if ( shared_verb == SHARED_VERB )
			perror( "shared_create: shmget" );
		return SHARED_E_SHM;
	}
	// pripojime zdielany segment do virtualneho priestoru procesu
	if ( shared_attach_internal( id ) != SHARED_OK ) {
		shmctl( shared_obj[id].shmid, IPC_RMID, NULL );
		shared_obj[id].shmid = -1;
		return SHARED_E_SHM;
	}

	shared_obj[id].key = key;
	shared_obj[id].size = size;
	shared_cnt++;

	return id;
}

int shared_attach( int id ) {
	// ak nie je libka inicializovana, cele zle
	if ( shared_initialized == 0 )
		return SHARED_E_INIT;
	// ak je id mimo rozsahu, tiez zle
	if ( (id<0) || (id>SHARED_MAX) )
		return SHARED_E_INVAL;
	// ak pre dane id nie je inicializovany objekt, zle
	if ( shared_obj[id].shmid == -1 )
		return SHARED_E_ID;
	// ak uz je pripojeny objekt, netreba nic robit
	if ( shared_obj[id].shmptr != (void*)-1 )
		return SHARED_OK;

	return shared_attach_internal( id );
}

int shared_detach( int id ) {
	// ak nie je libka inicializovana, cele zle
	if ( shared_initialized == 0 )
		return SHARED_E_INIT;
	// ak je id mimo rozsahu, tiez zle
	if ( (id<0) || (id>SHARED_MAX) )
		return SHARED_E_INVAL;
	// ak pre dane id nie je inicializovany objekt, zle
	if ( shared_obj[id].shmid == -1 )
		return SHARED_E_ID;
	// ak objekt nie je pripojeny, netreba nic robit
	if ( shared_obj[id].shmptr == (void *)-1 )
		return SHARED_OK;

	return shared_detach_internal( id );
}

int shared_destroy( int id ) {
	int ret;

	// ak nie je libka inicializovana, cele zle
	if ( shared_initialized == 0 )
		return SHARED_E_INIT;
	// ak je id mimo rozsahu, tiez zle
	if ( (id<0) || (id>SHARED_MAX) )
		return SHARED_E_INVAL;
	// ak pre dane id nie je inicializovany objekt, zle
	if ( shared_obj[id].shmid == -1 )
		return SHARED_E_ID;

	// ak je objekt pripojeny, skusime ho odpojit
	if ( shared_obj[id].shmptr != (void*)-1 ) {
		ret = shared_detach_internal( id );
		if ( ret != SHARED_OK )
			return ret;
	}
	return shared_destroy_internal( id );
}

int shared_write( int id, int from, const void *buf, int size ) {
	return shared_write_internal( id, from, buf, size, SHARED_NO_LOCKED );
}

int shared_write_locked( int id, int from, const void *buf, int size ) {
	return shared_write_internal( id, from, buf, size, SHARED_LOCKED );
}

int shared_read( int id, int from, void *buf, int size ) {
	return shared_read_internal( id, from, buf, size, SHARED_NO_LOCKED );
}

int shared_read_locked( int id, int from, void *buf, int size ) {
	return shared_read_internal( id, from, buf, size, SHARED_LOCKED );
}

// objekt NEMUSI byt pripojeny, aby sme mohli urobit lock alebo unlock
// dovod: odpajat segment by sme mali pri zamknutom semafore,
// aby niekto druhy medzitym segment nezrusil
int shared_lock( int id ) {
	// ak nie je libka inicializovana, cele zle
	if ( shared_initialized == 0 )
		return SHARED_E_INIT;
	// ak je id mimo rozsahu, tiez zle
	if ( (id<0) || (id>SHARED_MAX) )
		return SHARED_E_INVAL;
	// ak pre dane id nie je inicializovany objekt, zle
	if ( shared_obj[id].shmid == -1 )
		return SHARED_E_ID;
	// ak objekt nie je pripojeny, zle
	//if ( shared_obj[id].shmptr == (void*) -1 )
	//	return SHARED_E_PTR;

	return shared_lock_internal( shared_semid, id );
}

// objekt NEMUSI byt pripojeny, aby sme mohli urobit lock alebo unlock
// dovod: odpajat segment by sme mali pri zamknutom semafore,
// aby niekto druhy medzitym segment nezrusil
int shared_unlock( int id ) {
	// ak nie je libka inicializovana, cele zle
	if ( shared_initialized == 0 )
		return SHARED_E_INIT;
	// ak je id mimo rozsahu, tiez zle
	if ( (id<0) || (id>SHARED_MAX) )
		return SHARED_E_INVAL;
	// ak pre dane id nie je inicializovany objekt, zle
	if ( shared_obj[id].shmid == -1 )
		return SHARED_E_ID;
	// ak objekt nie je pripojeny, zle
	//if ( shared_obj[id].shmptr == (void*) -1 )
	//	return SHARED_E_PTR;

	return shared_unlock_internal( shared_semid, id );
}
