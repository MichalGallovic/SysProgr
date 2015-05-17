#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include "commonSocket.h"


int main() {
	STUDENT student;
    int serverSocket;
    int clientSocket;
    ssize_t receivedLength;
    struct sockaddr_un serverAddr;
	//vytvorenie soketu
	CHECK( (serverSocket = socket(AF_UNIX, SOCK_STREAM, 0)) != -1 );
	//socket
	unlink(SOCKET_ADDRESS);
	//previazanie adresy soketu (mena suboru v suborovom systeme) na soket
	//bind
	strcpy(serverAddr.sun_path, SOCKET_ADDRESS);
	serverAddr.sun_family = AF_UNIX;

    CHECK( bind(serverSocket,(struct sockaddr *)&serverAddr,sizeof(struct sockaddr_un)) != -1);
	//oznacenie soketu ako soketu, ktory bude pouzity na akceptovanie prihadzajucich ziadosti o pripojenie (funkciou accept) 
	//a nastavenie maximalnej velkosti fronty pre prichadzajuce ziadosti o pripojenie
	//listen
	CHECK(listen(serverSocket,5) != -1 );

	//akceptacia ziadosti o pripojenie
	//accept
    CHECK( (clientSocket = accept(serverSocket, NULL, NULL))  != -1 ); 

    printf("[server]: Spojenie s klientom vytvorene\n");
	//citanie udajov od klienta:
	//read alebo recv

    CHECK( (receivedLength = recv(clientSocket, &student, sizeof(STUDENT), MSG_WAITALL) ) != -1 );
    if(receivedLength != sizeof(STUDENT) ) {
        fprintf(stderr, "CHYBA: nebol prijate kompletne udaje\n");
        exit(EXIT_FAILURE);
    }

	//vypis prijatych udajov
	PrintStudent(&student);
	
	//zmena nacitanych udajov
	student.id++;

	//poslanie zmenenych udajov
	//write alebo send
    CHECK( write(clientSocket, &student, sizeof(STUDENT)) != -1);
	//zatvorenie soketu
	//close
	CHECK(close(clientSocket) != -1 );

	//odstranenie soketu zo suboroveho systemu
	//unlink

	return EXIT_SUCCESS;
}
