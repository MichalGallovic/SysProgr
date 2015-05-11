#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MSGSZ     128
#define KEY	  0xBACACABA

typedef struct _msgbuf_s {
    long    mtype;
    char    mtext[MSGSZ];
} msgbuf_s;

int main() {
    int msqid;
    key_t key;
    msgbuf_s sbuf, rbuf;

    // get id of queue with key = KEY
    key = KEY;
    if ((msqid = msgget(key, IPC_CREAT | 0660)) < 0) {
        perror("msgget");
        exit(1);
    }

    // send message
    sbuf.mtype = 1;
    sprintf(sbuf.mtext, "[client] Did you get this?");
    if (msgsnd(msqid, &sbuf, strlen(sbuf.mtext) + 1, 0) < 0) {
        perror("msgsnd");
        exit(-1);
    }

    // receive message
    if (msgrcv(msqid, &rbuf, MSGSZ, 2, 0) < 0) {
        perror("msgrcv");
        exit(-1);
    }

    // print message
    printf("[client]: '%s'\n", rbuf.mtext);
    return 0;
}

