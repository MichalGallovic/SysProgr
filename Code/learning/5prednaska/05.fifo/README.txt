Program pipe.c demonstruje jednoduchu komunikaciu procesov rodic potomok
cez kanal.
Program pipe1.c demonstruje komunikaciu dvoch detskych procesov cez kanal (ps -e|wc -l).
V programoch sa pouziva funkcia dup2.
Programy rfifo3.c wfifo3.c demonstruju komunikaciu dvoch nezavislych procesov
cez pomenovany kanal (fifo) (who|wc -l). Programy sa spustaju v dvoch oknach.