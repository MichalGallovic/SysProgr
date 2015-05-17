#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define	SEG_SIZE	0x6400

int main() {
	int seg_id;
	char *shared_mem;
	struct shmid_ds shm_buf;

	// vytvorenie
	seg_id = shmget( IPC_PRIVATE, SEG_SIZE, IPC_CREAT | 0600 );
	//seg_id = shmget( 0xBACA , SEG_SIZE, IPC_CREAT | 0600 );
	if ( seg_id == -1 ) {
		perror( "shmget:" );
		return -1;
	}

	// pripojenie do VM procesu
	shared_mem = (char*) shmat( seg_id, 0, 0 );
	// !!! !!! !!!
	if ( shared_mem == (void*)-1 ) {
		perror( "shmat:" );
		return -1;
	}
	// !!! !!! !!!
	fprintf(stderr, "shared memory attached at address %p\n", shared_mem);

	// zistovanie info o zdielanom segmente
	if ( shmctl( seg_id, IPC_STAT, &shm_buf ) == -1 ) {
		perror( "shmctl IPC_STAT:" );
		return -1;
	}
	fprintf(stderr, "shared memory segment size = 0x%X\n", shm_buf.shm_segsz);

	// zapis do zdielanej pamate
	sprintf( shared_mem, "Ahoj, svjete!" );

	// odpojenie ZP z VM procesu
	if ( shmdt( shared_mem ) == -1 ) {
		perror( "shmdt:" );
		return -1;
	}

	// znovupripojenie, ale na konkretnu adresu
	shared_mem = (char*) shmat( seg_id, (void*)0x5000000, 0 );
	// !!! !!! !!!
	if ( shared_mem == (void*)-1 ) {
		perror( "shmat 0x5000000:" );
		return -1;
	}
	// !!! !!! !!!
	fprintf(stderr, "shared memory re-attached at address %p\n", shared_mem );

	// vypisanie obsahu
	fprintf(stderr, "shared memory content: '%s'\n", shared_mem );

	// odpojenie z VM
	if ( shmdt( shared_mem ) == -1 ) {
		perror( "shmdt:" );
		return -1;
	}

	// zrusenie zdielaneho segmentu
	if ( shmctl( seg_id, IPC_RMID, NULL ) == -1 ) {
		perror( "shmctl IPC_RMID:" );
		return -1;
	}

	return 0;
}
