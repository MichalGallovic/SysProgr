#ifndef SHARE_MEM_INIT
#define SHARE_MEM_INIT

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <errno.h>

#define SEM_KEY 4545
#define SHM_KEY 5678
#define SHM_KEY1 5679

#define SHARED_BUFFER_SIZE 54

void free_shared_mem();
void free_shared_offset();			// uvolni zdielanu oblast pamate
int alloc_shared_mem(int size);
int alloc_shared_offset();		// ziska zdielanu oblast pamate
void free_semaphor();			// uvolni semafor
int alloc_semaphor();                   // ziska semafor
int init_semaphor();			// inic. semafor
int lock_semaphore();			// zablokuje semafor
int unlock_semaphore();			// uvolni semafor
void clear_data();
void clear_offset();			// vymaze pamat
char* get_share_buf();                  // vrati zdielanu pamat
void set_offset();                 // aktualizuje offset
char* get_offset_buf();
int get_offset();                 // vrati aktualnu polohu v zdielanej pamati
#endif
