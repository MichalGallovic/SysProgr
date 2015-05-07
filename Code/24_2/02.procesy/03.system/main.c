#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int grep( const char *what ) {
	size_t len;
	char *buf;
	int ret;

	len = strlen("grep -x ") + strlen(what) + strlen(" /usr/dict/words") + 1;
	buf = (char*) malloc( len * sizeof(char) );
	memset( buf, '\0', len );
	
	// WARNING!!!!
	// ./main "huhu /dev/null; echo "
	// grep -x huhu /dev/null; echo /usr/dict/words
	
	sprintf(buf, "grep -x %s /usr/dict/words", what);
	
	ret = system( buf );
	free (buf);
	return ret;
}

int main( int argc, char *argv[] ) {
	int ret;

	if (argc != 2) {
		fprintf(stderr,"Error: chybny pocet argumentov\n");
		return -1;
	}

	printf("zadany ret: '%s'\n", argv[1]);

	ret = grep( argv[1] );
	return ret;
}
