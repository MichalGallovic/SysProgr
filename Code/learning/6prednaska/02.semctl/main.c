#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <unistd.h>

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
	struct seminfo *seminfo_buf;
};

int bin_sem_init( int semid ) {
	union semun arg;
	unsigned short values[1];

	values[0] = 1;
	arg.array = values;
	return semctl( semid, 0, SETALL, arg );
}

int bin_sem_alloc( key_t key, int sem_flags ) {
	return semget( key, 1, sem_flags );
}

int bin_sem_dealloc( int semid ) {
	return semctl( semid, 0, IPC_RMID, NULL );
}

int bin_sem_wait( int semid ) {
	struct sembuf ops[1];

	ops[0].sem_num = 0;
	ops[0].sem_op = -1;
	ops[0].sem_flg = SEM_UNDO;

	return semop( semid, ops, 1 );
}

int bin_sem_post( int semid ) {
	struct sembuf ops[1];

	ops[0].sem_num = 0;
	ops[0].sem_op = +1;
	ops[0].sem_flg = SEM_UNDO;

	return semop( semid, ops, 1 );
}

int main() {
	int semid;

	if ( (semid=bin_sem_alloc( 0xCABA, IPC_CREAT | 0600 )) == -1 ) {
		perror("bin_sem_alloc:");
		return -1;
	}

	if ( bin_sem_init( semid ) == -1 ) {
		perror("bin_sem_init:");
		return -1;
	}

	fprintf(stderr, "%d\n", getpid() );

	bin_sem_wait( semid );
	bin_sem_wait( semid );

/*
	if ( bin_sem_dealloc( semid ) == -1 ) {
		perror("bin_sem_dealloc:");
		return -1;
	}
*/
	return 0;
}
