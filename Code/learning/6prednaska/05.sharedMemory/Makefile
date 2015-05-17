CC=gcc
CFLAGS=-O2 -Wall
LDFLAGS=
LIBS=

APPLICATION_OBJ=main.o sharemem.o
W1_OBJ=write1.o sharemem.o
W2_OBJ=write2.o sharemem.o
R_OBJ=reader.o sharemem.o

all:	application write1 write2 reader

application:	$(APPLICATION_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o main $(APPLICATION_OBJ)

write1:	$(W1_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o write1 $(W1_OBJ) $(LIBS)

write2:	$(W2_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o write2 $(W2_OBJ) $(LIBS)

reader:	$(R_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o reader $(R_OBJ) $(LIBS)

.c.o:	%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f core main write1 write2 reader
	-rm -f $(APPLICATION_OBJ) $(W1_OBJ) $(W2_OBJ) $(R_OBJ)

