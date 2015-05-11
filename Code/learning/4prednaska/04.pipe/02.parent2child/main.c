#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

typedef struct _msg {
	int id;
	char str[100];
} msg_s;

void run_child( int rura[2] ) {
	msg_s msg;

	close( 0 );
	dup( rura[0] );
	close( rura[0] );
	close( rura[1] );

	sleep(1);
	while( read(0, &msg, sizeof(msg)) > 0 ){
		fprintf(stderr, "[child]: prijata sprava %02d: '%s'\n", msg.id, msg.str);
    }


	return;
}

void run_parent( int rura[2] ) {
	msg_s msg;
	int i;
	close( 1 );
	dup( rura[1] );
	close( rura[0] );
	close( rura[1] );
	for ( i = 0; i < 7; i++ ) {
		msg.id = i+1;
		sprintf(msg.str, "text %02d", i);
		write(1, &msg, sizeof(msg) );
		fprintf(stderr, "[parent]: poslana sprava %02d: '%s'\n", msg.id, msg.str);
	} // for i
    sleep(2);
	close( 1 );	// !!! co ked nedame? deadlock!!!
	wait( NULL );

	return;
}

int main() {
	int rura[2];

	if ( pipe(rura) == -1 ) {
		perror("Error: pipe");
		return -1;
	}

	switch ( fork() ) {
	case -1:
		perror("Error: fork");
		return -1;
	case 0:
		run_child(rura);
		return 0;
	default:
		run_parent(rura);
		return 0;
	}

	return 0;
}
