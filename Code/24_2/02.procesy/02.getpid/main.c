#include <stdio.h>
#include <unistd.h>

int main() {
	printf("moj pid je: %d\n", getpid());
	printf("pid rodica: %d\n", getppid());
	return 0;
}
