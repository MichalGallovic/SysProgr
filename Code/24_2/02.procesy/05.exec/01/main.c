#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main() {
	char cmd[100];
	char *argv[10];

	strcpy(cmd, "/bin/ls");
	argv[0] = cmd;
	argv[1] = "/tmp";
	argv[2] = "-la";
	argv[3] = NULL;

	execv(cmd, argv);

	fprintf(stderr, "Error: sem sa nikdy nemame dostat\n");
	return -1;
}
