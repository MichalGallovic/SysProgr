#include <unistd.h>
#include <stdio.h>

int main() {
	int i = 0;

	fork();
	i++;

	printf("i = %d\n", i);
	return 0;
}
