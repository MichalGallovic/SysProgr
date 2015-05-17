#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include "commonSocket.h"

int main() {
	STUDENT student;
	int sock;
	struct sockaddr_un sockAddr;
	ssize_t receivedDataLength;
	int sockAddrLen;
	//vytvorenie soketu cez ktory bude klient komunikovat
	//socket
    CHECK( (sock = socket(AF_UNIX, SOCK_STREAM, 0)) != -1 );
	//vytvorenie spojenia zo serveroom
	//connect
	sockAddr.sun_family = AF_UNIX;
	strcpy(sockAddr.sun_path, SOCKET_ADDRESS);
	sockAddrLen = SUN_LEN(&sockAddr); 
    CHECK( connect(sock,(struct sockaddr *)&sockAddr, sockAddrLen) != -1);
	//nastavenie udajov, ktore budu poslane serveru
	SetStudent(&student, "MenoStudenta", 1);

	//poslanie udajov
	//write alebo send
    CHECK( send(sock,&student, sizeof(STUDENT), 0)  != -1);
	//prijatie udajov
	//read alebo recv
    CHECK( (receivedDataLength = read(sock, &student, sizeof(STUDENT))) != -1);
	//vypis prijatych udajov zo servera
	if(receivedDataLength != sizeof(STUDENT)) {
	    fprintf(stderr, "CHYBA: neboli prijate kompletne udaje\n");
	    exit(EXIT_FAILURE);
    }
	PrintStudent(&student);

	//zatvorenie soketu
	//close
    CHECK( close(sock) != -1);
	return EXIT_SUCCESS;
}
