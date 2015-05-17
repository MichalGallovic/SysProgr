/*****************************************************************
 *
 * S H A R E D    O B J E C T    H A N D L I N G    L I B R A R Y
 *
 *****************************************************************/

#ifndef _SHARED_H_
#define _SHARED_H_

#include <sys/types.h>

#define		SHARED_NO_INITIALIZE	0	// neinicializovat hodnoty mnoziny semaforov pre kniznicu
#define		SHARED_INITIALIZE	1	// inicializovat hodnoty mnoziny semaforov pre kniznicu

#define		SHARED_NO_VERB	0	// nevypisovanie chybovych sprav na stderr
#define		SHARED_VERB	1	// vypisovanie chybovych sprav na stderr

#define		SHARED_OK	 0	// vsetko ok
#define		SHARED_E_INIT	-1	// neinicializovana kniznica
#define		SHARED_E_INVAL	-2	// id segmentu mimo platny rozsah
#define		SHARED_E_ID	-3	// neinicializovany segment
#define		SHARED_E_OBJCNT	-4	// presiahnuty rozsah
#define		SHARED_E_SEM	-5	// chyba pri praci so semaforom
#define		SHARED_E_SHM	-6	// chyba pri praci so zdielanym segmentom
#define		SHARED_E_PTR	-7	// segment nie je pripojeny do virtualneho adresneho priestoru
#define		SHARED_E_TOOBIG	-8	// prilis vela udajov
#define		SHARED_E_ARG	-9	// neplany argument (mimo rozsah)

void shared_set_verbose( int val );		// nastavi verbose alebo no-verbose
int shared_init( key_t key, int initialize );	// inicializacia kniznice
int shared_fini();				// upratanie kniznice

int shared_create( key_t key, int size );	// vytvorenie zdielaneho segmentu spolu s pripojenim
int shared_destroy( int id );			// zrusenie zdielaneho segmentu spolu s odpojenim
int shared_attach( int id );			// pripojenie zdielaneho segmentu
int shared_detach( int id );			// odpojenie zdielaneho segmentu
int shared_lock( int id );			// uzamknutie binarneho semaforu pre zdielany segment
int shared_unlock( int id );			// odomknutie binarneho semaforu pre zdielany segment
int shared_write( int id, int from, const void *buf, int size );	// zapis do zdielaneho segmentu bez ohladu na stav semafora
int shared_read( int id, int from, void *buf, int size );		// citanie zo zdielaneho segmentu bez ohladu na stav semafora
int shared_write_locked( int id, int from, const void *buf, int size );	// zapis do zdielaneho segmentu pri zamknutom semafore
int shared_read_locked( int id, int from, void *buf, int size );	// citanie zo zdielaneho segmentu pri zamknutom semafore

#endif // _SHARED_H_
