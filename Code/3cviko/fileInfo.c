#include <stdio.h>
#include <sys/stat.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <time.h> 
#include <pwd.h> 
#include <grp.h> 
#include <unistd.h> 
#include <string.h> 


//Funkcia vypise informacie o subore 'fileName'
//Je prikladom ziskania informacii pomocou lstat
//Vstup: 'fileName' - nazov suboru o ktorom sa vypisu informacie
void printfFileInfo(char *fileName)
{
	struct stat info;
	struct passwd *pwd;
	struct group  *grp;


	//------------------------------------------------------------------------
	// ziskanie informacii od struktury stat
	//------------------------------------------------------------------------
	// aky je rozdiel medzi tymito funkciami?
	//   int lstat(const char *path, struct stat *buf);
	//   int  stat(const char *path, struct stat *buf);
	if( lstat(fileName, &info) == -1 ) {
	 	fprintf(stderr, "Neviem zistit informacie o \"%s\" (%s)\n", fileName, strerror(errno) );
		exit(EXIT_FAILURE);
	}


	//------------------------------------------------------------------------
	// nazov
	//------------------------------------------------------------------------
	printf("%-20s: %s\n", "nazov", fileName);


	//------------------------------------------------------------------------
	// typ suboru (1. sposob)
	//------------------------------------------------------------------------
	printf("%-20s: ","typ (1. sposob)");

	if (      ( info.st_mode & S_IFMT ) == S_IFBLK ) { // Block special
		printf("block special file");
	}
	else if ( ( info.st_mode & S_IFMT ) == S_IFCHR ) { // Character special
		printf("character special file");
	}
	else if ( ( info.st_mode & S_IFMT ) == S_IFIFO ) { // FIFO special
		printf("pipe or FIFO special file");
	}
	else if ( ( info.st_mode & S_IFMT ) == S_IFREG ) { // Regular
		printf("regular file");
	}
	else if ( ( info.st_mode & S_IFMT ) == S_IFDIR ) { // Directory
		printf("directory");
	}
	else if ( ( info.st_mode & S_IFMT ) == S_IFLNK ) { // Symbolic link
		printf("symbolic link");
	}
	else if ( ( info.st_mode & S_IFMT ) == S_IFSOCK ) { // Socket
		printf("socket");
	}
	printf("\n");


	//------------------------------------------------------------------------
	// typ suboru (2. sposob)
	//------------------------------------------------------------------------
	printf("%-20s: ","typ (2. sposob)");

	if(	S_ISBLK(info.st_mode) ) {        //block special file
		printf("block special file");
	}
	else if( S_ISCHR(info.st_mode) ) {   //character special file
		printf("character special file");
	}
	else if ( S_ISDIR(info.st_mode) ) {  //directory.
		printf("directory");
	}
	else if ( S_ISFIFO(info.st_mode) ) { //pipe or FIFO special file.
		printf("pipe or FIFO special file");
	}
	else if ( S_ISREG(info.st_mode) ) {  //regular file.
		printf("regular file");
	}
	else if ( S_ISLNK(info.st_mode) ) {  //symbolic link.
		printf("symbolic link");
	}
	else if ( S_ISSOCK(info.st_mode) ) { //socket.
		printf("socket");
	}
	printf("\n");


	//------------------------------------------------------------------------
	// pristupove prava
	//------------------------------------------------------------------------
	printf("%-20s: ","pristupove prava");

	//vlastnik
	printf("%c", ( info.st_mode & S_IRUSR ) ? 'r' : '-');            
	printf("%c", ( info.st_mode & S_IWUSR ) ? 'w' : '-');   
	printf("%c", ( info.st_mode & S_IXUSR ) ? 'x' : '-');  

	//skupina
	printf("%c", ( info.st_mode & S_IRGRP ) ? 'r' : '-');            
	printf("%c", ( info.st_mode & S_IWGRP ) ? 'w' : '-');   
	printf("%c", ( info.st_mode & S_IXGRP ) ? 'x' : '-');  

	//ostatny
	printf("%c", ( info.st_mode & S_IROTH ) ? 'r' : '-');            
	printf("%c", ( info.st_mode & S_IWOTH ) ? 'w' : '-');   
	printf("%c", ( info.st_mode & S_IXOTH ) ? 'x' : '-');  

	printf("\n");


	//------------------------------------------------------------------------
	// pocet hardverovych liniek
	//------------------------------------------------------------------------
	printf("%-20s: %d\n", "pocet hard. liniek", info.st_nlink);


	//------------------------------------------------------------------------
	// cas
	//------------------------------------------------------------------------
	printf("%-20s: %s","posledny pristup",   ctime(&info.st_atime)); 
	printf("%-20s: %s","posl. zmena udajov", ctime(&info.st_mtime)); 
	printf("%-20s: %s","posl. zmena stavu",  ctime(&info.st_ctime)); //zmena meta-udajov (cas posl. zmeny obsahu suboru, pristupove prava, vlastnictvo,.....)
	//preco nie je na konci formatovacieho retazca funkcie printf znak '\n'?


	//------------------------------------------------------------------------
	// vlastnik
	//------------------------------------------------------------------------
	printf("%-20s: ", "vlastnik");

	if ((pwd = getpwuid(info.st_uid)) != NULL)
		printf("%-8.8s", pwd->pw_name);
	else
		printf("%-8d", (int)info.st_uid);

	printf("\n");


	//------------------------------------------------------------------------
	// skupina
	//------------------------------------------------------------------------
	printf("%-20s: ", "skupina");

	if ((grp = getgrgid(info.st_gid)) != NULL)
		printf("%-8.8s", grp->gr_name);
	else
		printf("%-8d", (int)info.st_gid);

	printf("\n");


	//------------------------------------------------------------------------
	// velkost:
	// v pripade regularneho suboru je to velkost suboru v bajtoch
	// v pripade symbolickej linky je to dlzka cesty v bajtoch 
	//------------------------------------------------------------------------
	printf("%-20s: %d\n", "velkost", (int)info.st_size);


	//------------------------------------------------------------------------
	// vypis cieloveho suboru linkylinky
	//------------------------------------------------------------------------
	if( S_ISLNK(info.st_mode) ) {
		char * linkTarget = (char *) malloc((info.st_size+1)* sizeof(char));
	
		if( readlink(fileName, linkTarget, info.st_size) == -1 ) {
			fprintf(stderr, "Chyba pri zistovani ciela linky (%s)\n", strerror(errno) );
			exit(EXIT_FAILURE);                                                                      	
		}
		
		linkTarget[info.st_size] = '\0';
		
		printf("%-20s: %s\n", "ciel linky", linkTarget);

		free(linkTarget);
	}

}

//Program vypise informacie o subore, ktory je argumentom programu
int main(int argc, char * argv[])
{
	if( argc == 2 ) {
		printfFileInfo(argv[1]);
		return EXIT_SUCCESS;
	}
	else {
		fprintf(stderr, "%s: chyny pocet argumentov\n", argv[0]);
		return EXIT_FAILURE;
	}
}

