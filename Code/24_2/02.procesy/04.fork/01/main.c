#include <unistd.h>

int main() {
	fork();
	write(1,"tento text sa vypise 2x\n",24);
	return 0;
}
