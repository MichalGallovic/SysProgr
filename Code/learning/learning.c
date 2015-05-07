#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

int spawn(char* program, char** arg_list) {
	pid_t child_pid;
	
	child_pid = fork();
	
	if(child_pid != 0) {
		return child_pid;
	} else {
		execvp(program, arg_list);
		
		fprintf(stderr, "Error occured in execvp\n");
		abort();
	}
}

sig_atomic_t sigint_count = 0;

void handler(int signal_number) {
	++sigint_count;
    printf("Control C blee \n");
}

sig_atomic_t child_exit_status = 0;
void clean_up_child_process(int signal_number) {
    int status;
    wait(&status);
    child_exit_status = status;
    printf("\n\nEnded child process async\n");
    sleep(5);
    printf("Bye from async pid %d\n", getpid());
}

void sigusr1_handler(int signal_number) {
    printf("\nsignal %d triggered\n", signal_number);
}

int main(int argc, char *argv[]) {
	printf("\n");
//	pid_t child_pid;
//	
//	printf("main program process ID %d\n", (int) getpid());
//	
//	child_pid = fork();
//	
//	if(child_pid != 0) {
//		printf("parent process ID %d\n", (int) getpid());
//		printf("child process ID %d\n", (int)child_pid);
//	} else {
//		printf("child process getpid %d\n", (int)getpid());
//		printf("parent pid from child %d\n", child_pid);
//	}
//	
//	

//	char command[100];
//	char *argv2[10];
//	
//	strcpy(command, "/bin/ls");
//	
//	argv2[0] = command;
//	argv2[1] = "/";
//	argv2[2] = NULL;
//	
//	
//	execv(command, argv2);
//	
//	printf("Noope");

//	char *arg_list[] = {
//		"ls",
//		"-l",
//		"/",
//		NULL
//	};
//	
//	spawn("ls", arg_list);
//	printf("done with main program\n");
//
//	int child_status;
//	pid_t child_pid;
//	
//	child_pid = fork();
//	
//	wait(&child_status);
//	
//	if (child_pid != 0) {
//		printf("Hello from parent!\n");
//	} else {
//		printf("Hey I'm child!\n");
//		return 0;
//	}
//	
//	if (WIFEXITED(child_status)) {
//		printf("the child process exited normally, with exit code %d\n", WEXITSTATUS(child_status));
//	} else {
//		printf("the child process exited abnormally\n");
//		return 0;
////	}
//	
//  struct sigaction sa;
// 	memset(&sa, 0, sizeof(sa));
// 	sa.sa_handler = &handler;
// 	sigaction(SIGINT, &sa, NULL);
// 
// 
//     while(sigusr1_count < 10) {
//         sleep(1);
//     }
// 
// 	printf("SIGINT was raise %d times \n", sigusr1_count);
//
//    struct sigaction sigchild_action;
//    memset(&sigchild_action, 0, sizeof(sigchild_action));
//    sigchild_action.sa_handler = &clean_up_child_process;
//  
//    if(sigaction(SIGCHLD, &sigchild_action, NULL) == -1) {
//        perror("Toz error\n");
//        exit(1);
//    }
//
//    pid_t child_pid;
//
//    child_pid = fork();
//
//    switch(child_pid) {
//        case -1:
//            perror("fork");
//            return -1;
//            
//        case 0:
//            printf("child is gonna sleep in 2secs pid %d\n", getpid());
////            sleep(2);
//            exit(0);
//            break;
//        default:
//            printf("Hi I'm parent, gonna wait pid %d\n", getpid());
////            unsigned int secondsToGo = 0;
////            if((secondsToGo = sleep(5)) != 0) {
////                perror("Parent received signal\n");
////                fprintf(stderr,"Seconds to go %u\n", secondsToGo);
////            }
//            break;
//    }
//    
//    int i = 0;
//    for(;i<100000;i++) {
//        printf("/");
//    }
    struct sigaction sigusr_action;
    memset(&sigusr_action, 0, sizeof(sigusr_action));

    sigusr_action.sa_handler = &sigusr1_handler;
    if(sigaction(SIGUSR1, &sigusr_action, NULL) == -1) {
        perror("assigning signal SIGUSR1 handler error");
        exit(1);
    }

    sleep(60);

    
 	return 0;
}
