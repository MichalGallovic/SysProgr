#ifndef _SHM_H_
#define _SHM_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>

#define	NO_VERBOSE	0
#define	VERBOSE		1

#define	SHM_SIZE	128
#define	SHM_KEY		0xDEADBEEF

typedef union _semun {
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
} semun;

void set_verbose(int val);

int open_sem(key_t key);
int init_sem(int sem);
int lock_sem(int sem);
int unlock_sem(int sem);
int free_sem(int sem);

char *open_shm(key_t key, int size, int* shmid);
int close_shm(char *shm);
int free_shm(int shmid);

#endif // _SHM_H_
