#include <unistd.h>
#include "shm.h"

int main() {
	int	semid, shmid;
	char	*shm;
	int	ret = 0;
	char 	znak;

	if ( (shm = open_shm( SHM_KEY, SHM_SIZE, &shmid )) == NULL ) {
		ret = -1;
		goto e_open_shm;
	}
	if ( (semid = open_sem( SHM_KEY )) == -1 ) {
		ret = -2;
		goto e_open_sem;
	}
	if ( init_sem( semid ) == -1 ) {
		ret = -3;
		goto e_init_sem;
	}

	if ( lock_sem( semid ) == -1 ) {
		ret = -4;
		goto e_lock_sem;
	}
	fprintf(stderr, "server ide pisat do ZP\n");
	for ( znak = 'a'; znak <= 'z'; znak++ ) {
		shm[znak - 'a'] = znak;
		usleep( 10000 );
	}
	shm[ znak - 'a' ] = '\0';
	fprintf(stderr, "server dopisal do ZP\n");
	if ( unlock_sem( semid ) == -1 ) {
		ret = -5;
		goto e_unlock_sem;
	}

	// cakame, kym si druhy proces precita spravu
	// ukoncenie cakania je indikovane zmenou prveho znaku na znak '*'
	while ( *shm != '*' )
		;

e_unlock_sem:
e_lock_sem:
e_init_sem:
	free_sem( semid );
e_open_sem:
	close_shm( shm );
	free_shm ( shmid );
e_open_shm:
	return ret;
}
