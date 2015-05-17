#include <unistd.h>
#include <stdio.h>
#include "shared.h"

#define		SHARED_KEY1	0xBABA
#define		SHARED_KEY2	0xDADA

int main() {
	int	shared_id;
	char 	znak;

	// inicializujeme kniznicu
	shared_set_verbose( SHARED_VERB );
	if ( shared_init( SHARED_KEY1, SHARED_INITIALIZE ) != SHARED_OK )
		return -1;

	// vytvorime zdielany objekt
	shared_id = shared_create( SHARED_KEY2, 1024 );
	if ( shared_id == -1 )
		return -1;
	fprintf( stderr, "server shared id = %d\n", shared_id );

	// uzamkneme zdielany objekt a zapisujeme do neho
	if ( shared_lock( shared_id ) != SHARED_OK )
		return -1;
	fprintf(stderr, "server ide pisat do ZP\n");
	for ( znak = 'a'; znak <= 'z'; znak++ ) {
		shared_write( shared_id, znak - 'a', (char*)&znak, 1 );
		usleep( 10000 );
	}
	znak = '\0';
	shared_write( shared_id, znak - 'a', (char*)&znak, 1 );
	fprintf(stderr, "server dopisal do ZP\n");
	// odomkneme zdielany objekt
	if ( shared_unlock( shared_id ) != SHARED_OK )
		return -1;

	// cakame, kym si druhy proces precita spravu
	// ukoncenie cakania je indikovane zmenou prveho znaku na znak '*'
	while ( 1 ) {
		// citame pri zamknutom mutexe!!!
		shared_read_locked( shared_id, 0, (char*)&znak, 1 );
		if ( znak == '*' )
			break;
	}

	// mozeme zrusit zdielany objekt, pretoze klient uz vsetko precital
	shared_destroy( shared_id );
	// ukoncime cinnost kniznice
	//shared_fini();

	return 0;
}
