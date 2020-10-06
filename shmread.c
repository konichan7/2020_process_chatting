#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include"chatshm.h"

int main(void){

    // dotChange, timeChange is for ".." algorithm.
    int shmid,dotChange;
    long timeChange;

    // make CHAT_INFO type pointer.
    CHAT_INFO *chatInfo = NULL;
    void *shmaddr = (void*)0;

    shmid = shmget((key_t)20200406, sizeof(CHAT_INFO), 0666|IPC_CREAT|IPC_EXCL);
    // if shared memory is already created in shmwrite program, just get it.
    if(shmid<0){
	shmid=shmget((key_t)20200406,sizeof(CHAT_INFO),0666);
	shmaddr=shmat(shmid,(void*)0,0666);
	// if shmat function not worked correctly, print error message.
	if(shmaddr<(void*)0){
	    perror("attach falied");
	    exit(-1);
	}
    }
    // when shared memory is created in this program.
    else{
	shmaddr=shmat(shmid,(void*)0,0666);
    }

    // chatInfo point to address of shared memory.
    chatInfo=(CHAT_INFO*)shmaddr;

    // initialize dotChange and timeChange.
    dotChange=chatInfo->isDot;
    timeChange=chatInfo->messageTime;

    printf("=============CHAT MESSAGE===============\n");

    while(1){
	// when user enter "..", (chatInfo->isDot) is increased by 1.
	// this is implemented at shmwrite.c.
	if((chatInfo->isDot)>dotChange){
	    // print next message (latest message)
	    printf("[%s] %ld : %s\n",chatInfo->userID,chatInfo->messageTime,
                                chatInfo->message);
	    dotChange++;
	}
	else{
	    // messageTime is only increased by 1 when new message is entered.
	    if((chatInfo->messageTime)>timeChange){
		// if there's new message, then show it.
	        printf("[%s] %ld : %s\n",chatInfo->userID,chatInfo->messageTime,
				        chatInfo->message);
		// if user enter "/bye", shmread program is terminated.
	        if(0==strcmp(chatInfo->message,"/bye\n")){
	            printf("%s is out\n",chatInfo->userID);
	            break;
	        }
		timeChange++;
	    }
	    // this task is repeated every 0.1s.
            usleep(100000);
	}
    }
    return 0;
}
