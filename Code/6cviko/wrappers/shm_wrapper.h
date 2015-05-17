#ifndef _SHM_WRAPPER_H_
#define _SHM_WRAPPER_H_

#include "common.h"

typedef struct {
    int pid;
    // ...
} shm_data;

typedef struct {
    int id;         // identifikator zdielanej pamate
    shm_data * data;    // data

} shm_wrapper;

shm_wrapper shmObj; // globalna premenna wraperu

// vytvori zdielanu pamate
void initShmWrapper(shm_wrapper * shmObj) {
    CHECK( (shmObj->id = shmget(IPC_PRIVATE, sizeof(*shmObj->data), S_IRUSR | S_IWUSR)) != -1 );    
}

// vytvori zdielanu pamate
void initShm() {
    initShmWrapper(&shmObj);
}

// zmaze zdielanu pamat
void deinitShmWrapper(shm_wrapper * shmObj) {
    CHECK( (shmctl(shmObj->id, IPC_RMID, 0)) != -1 );
}

// zmaze zdielanu pamat
void deinitShm() {
    deinitShmWrapper(&shmObj);
}

// vrati ukazatel na zdielane data - pripoji sa na zdielanu pamat
shm_data * connectShmWrapper(shm_wrapper * shmObj) {
    CHECK( (shmObj->data = shmat(shmObj->id, NULL, 0)) != (void *) -1);
    return shmObj->data;
}

// vrati ukazatel na zdielane data - pripoji sa na zdielanu pamat
shm_data * connectShm() {
    return connectShmWrapper(&shmObj);
}

// odpoji zdielanu pamat
void disconnectShmWrapper(shm_wrapper * shmObj) {
    CHECK( shmdt(shmObj->data) != -1);  
}

// odpoji zdielanu pamat
void disconnectShm() {
    disconnectShmWrapper(&shmObj);
}

#endif
