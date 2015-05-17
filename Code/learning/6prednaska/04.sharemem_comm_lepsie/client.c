#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include "shared.h"

#define		SHARED_KEY1	0xBABA
#define		SHARED_KEY2	0xDADA

int main() {
	int	shared_id;
	int	i;
	char	znak;

	// inicializujeme kniznicu
	shared_set_verbose( SHARED_VERB );
	if ( shared_init( SHARED_KEY1, SHARED_NO_INITIALIZE ) != SHARED_OK )
		return -1;

	// vytvorime zdielany objekt
	shared_id = shared_create( SHARED_KEY2, 1024 );
	if ( shared_id == -1 )
		return -1;
	fprintf( stderr, "client shared id = %d\n", shared_id );

	// uzamkneme zdielany objekt... a budeme citat
	// kedze server zapisuje pri zamknutom semafore, tak nas proces
	// bude pokracovat az po uvolneni semafora serverom
	if ( shared_lock( shared_id ) != SHARED_OK )
		return -1;
	fprintf(stderr, "client ide citat zo ZP\n");
	i = 0;
	while ( 1 ) {
		shared_read( shared_id, i, (char*)&znak, 1 );
		if ( znak == '\0' )
			break;
		fputc( toupper( znak ), stderr );
		i++;
	}
	fputc( '\n', stderr );
	// zapiseme znak '*' na zaciatok zdielaneho objektu, aby sme dali serveru vediet,
	// ze klient uz vsetko precital
	znak = '*';
	sleep( 10 );
	shared_write( shared_id, 0, (char*)&znak, 1 );
	fprintf(stderr, "client docital zo ZP\n");

	// odpojime zdielany objekt, rusit ho bude server
	// odpajanie MUSIME spravit pri zamknutom mutexe,
	// aby ho medzitym nezrusil server
	shared_detach( shared_id );

	// odomkneme zdielany objekt
	if ( shared_unlock( shared_id ) != SHARED_OK )
		return -1;

	return 0;
}
