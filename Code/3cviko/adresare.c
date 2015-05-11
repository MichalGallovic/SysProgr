#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

//Funkcia vypise obsah adresara 'dirName'
//Do kazdeho riadku vypise nazov suboru, medzeru, nazov suboru aj s cestov
//Vstup: 'dirName' - nazov adresara
//Vystup: EXIT_SUCCESS v pripade uspesneho vykonania, inak EXIT_FAILIRE
int listDir(char * dirName) {
	DIR * dir; //adresar
	struct dirent * entry; //polozka adresara
	
	dir  = opendir(dirName);
	if( dir == NULL ) {
		perror("opendir");
		return EXIT_FAILURE;
	}

	while( (entry = readdir(dir)) != NULL ) {
//		printf("%-20s", entry->d_name); //vypis nazvu suboru

		//dalsie informacie mozeme ziskavat pomocou stat/lstat, 
		//ak ide o adresar funkciu mozeme volat rekurizvne
		//najprv potrebujeme (do premennej entryPath) zostavit cestu ku polozke adresara
		int entryPathLength = strlen(dirName) + strlen(entry->d_name) + 2; 
		char * entryPath = (char *) malloc(entryPathLength * sizeof(char)); 
		sprintf(entryPath, "%s/%s", dirName, entry->d_name);
		printf(" %s\n", entryPath);
		free(entryPath);
	}

	closedir(dir);

	return EXIT_SUCCESS;
}

//Program vypise obsah adresara, ktory je argumentom programu
int main(int argc, char *argv[]) {
	if( argc == 2 ) {
		listDir(argv[1]);
		return EXIT_SUCCESS;
	}
	else {
		fprintf(stderr, "%s: chybny pocet argumentov\n", argv[0]);
		return EXIT_FAILURE;
	}
}


