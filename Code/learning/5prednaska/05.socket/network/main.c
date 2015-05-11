#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h> // for gethostbyname
#include <sys/socket.h>
#include <string.h>

void get_home_page( int fd ) {
	char buffer[1024*1024];
	ssize_t ret;

	sprintf(buffer, "GET /\n");
	write(fd, buffer, strlen(buffer));

	while( 1 ) {
		ret = read(fd, buffer, 1024*1024-1);
		if ( ret <= 0 )
			break;
		buffer[ret] = '\0';
		fprintf(stderr,"%s",buffer);
	}
}

int main(int argc, char *argv[]) {
	int fd;
	struct sockaddr_in name;
	struct hostent *hostinfo;

	if ( argc != 2 ) {
		fprintf(stderr,"Error: invalid number of arguments\n");
		return -1;
	}

	if ( (fd=socket( PF_INET, SOCK_STREAM, 0 )) == -1 ) {
		perror("Error: socket");
		return -1;
	}

	hostinfo = gethostbyname( argv[1] );
	if ( hostinfo == NULL ) {
		fprintf(stderr, "Error resolving hostname '%s'\n", argv[1]);
		return -1;
	}
	name.sin_family = AF_INET;
	name.sin_addr = *((struct in_addr*)hostinfo->h_addr);
	name.sin_port = htons(80);	// HTTP port
	
	if ( connect(fd, (const struct sockaddr*)&name, sizeof(struct sockaddr_in)) == -1 ) {
		perror("Error: connect");
		return -1;
	}

	get_home_page(fd);
	return 0;
}
