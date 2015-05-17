#include "commonMsgQueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/msg.h>

int main(int argc, char * argv[]) 
{
    int msgQueueId; //id fornty sprav
    MSG_NUM_STUDENTS msgNumStudent; //sprava pre prijatie udaju o pocte studentov
    MSG_STUDENT msgStudent; //sprava pre prijatie udajov o studentovi

    //podla kluca ziskame id fronty sprav 
    //(analogia ku ziskaniu deskriptora suboru, identifikovaneho cestou v suborovm systeme)
    CHECK( (msgQueueId=msgget(MSG_QUEUE_KEY, S_IRUSR | S_IWUSR)) != -1);

    //nacitanie poctu studentov
    CHECK( msgrcv(msgQueueId, & msgNumStudent, sizeof(msgNumStudent.numStudents), MSG_TYPE_NUM_STUDENTS, 0) != -1);

    //nacitanie informaci o studentoch 
    int i;
    for(i = 0; i < msgNumStudent.numStudents; i++ ) {
        CHECK( msgrcv(msgQueueId, & msgStudent, sizeof(msgStudent.student), MSG_TYPE_STUDENT, 0) != -1);

        //vypis prijatej spravy
        PrintStudent( &msgStudent.student);
    }

    //zrusenie fronty sprav
    CHECK( msgctl(msgQueueId, IPC_RMID, NULL) != -1 );

    return EXIT_SUCCESS;
}
