#include <stdio.h>
#include <pthread.h>
#include <assert.h>

#define ITER 100000
int cnt = 0;

void *count(void* t) {
	int tmp;
	int i;

	for ( i = 0; i < ITER; i++ ) {
		tmp = cnt;
		tmp++;
		cnt = tmp;
	}

	return NULL;
}

int main() {
	pthread_t t1,t2;
	int err;

	err = pthread_create(&t1,NULL,count,NULL);
	assert( err == 0 );
	err = pthread_create(&t2,NULL,count,NULL);
	assert( err == 0 );

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	if ( cnt != 2*ITER )
		fprintf(stderr, "ERROR: cnt = %i\n", cnt);
	else
		fprintf(stderr, "OK: cnt = %i\n", cnt);
	
	return 0;
}
