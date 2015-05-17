#include "commonMsgQueue.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/msg.h>

int main(int argc, char * argv[]) 
{
    int msgQueueId; //id fronty sprav
    MSG_NUM_STUDENTS msgNumStudent; //sprava pre poslanie poctu studentov
    MSG_STUDENT msgStudent; //sprava pre poslanie udajov o studentovi
    //ziskanie id ku fronte fronte sprav
    CHECK((msgQueueId = msgget(MSG_QUEUE_KEY, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) != -1);
    //poslanie poctu studentov
    msgNumStudent.mtype = MSG_TYPE_NUM_STUDENTS;
    msgNumStudent.numStudents = 2;
    CHECK( msgsnd(msgQueueId, &msgNumStudent, sizeof(msgNumStudent.numStudents), 0) != -1);
    //poslanie informacie o studnetnoch
    msgStudent.mtype = MSG_TYPE_STUDENT;
    SetStudent(& msgStudent.student, "Meno1", 1);
    //poslanie udajov 	
    CHECK( msgsnd(msgQueueId, &msgStudent, sizeof(msgStudent.student), 0) != -1);
    SetStudent(& msgStudent.student, "Meno2", 2);
    //poslanie udajov
    CHECK( msgsnd(msgQueueId, &msgStudent, sizeof(msgStudent.student), 0) != -1);
    return EXIT_SUCCESS;
}
