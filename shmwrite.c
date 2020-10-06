#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>
#include"chatshm.h"

int main(int argc,char* argv[]){
    int shmid;
    char userID[20];
    CHAT_INFO *chatInfo=NULL;
    void* shmaddr = (void*)0;
    // if there's no input string, print error message and exit.
    if(argc<2){
	fprintf(stderr,"[usage]: ./shrmwrite USERID\n");
	exit(-1);
    }
    // if shared memory is created, 'shmid' get its shmid.
    // Otherwise, 'shmid' get -1.
    shmid = shmget((key_t)20200406,sizeof(CHAT_INFO),0666|IPC_CREAT|IPC_EXCL);
    // if shared memory already exist, just get it.
    if(shmid<0){
	shmid=shmget((key_t)20200406,sizeof(CHAT_INFO),0666);
	// shmaddr get the address of shared memory.
	shmaddr=shmat(shmid,(void*)0,0666);
	// if shmat function doesn't working correctly, print error message and exit.
	if(shmaddr<(void*)0){
	    perror("attach failed");
	    exit(-1);
	}
    }
    // shared memory is created correctly, get address of shared memory.
    else{
        shmaddr=shmat(shmid,(void*)0,0666);
    }

    // chatInfo point to shmaddr which is the address of shared memory.
    chatInfo=(CHAT_INFO*)shmaddr;
    // set the default string value of 'chat_notification'
    strcpy(chatInfo->chat_notification,"ENTER THE MESSAGE");
    // argv[1] means the first input string which is the ID of user's.
    strcpy(userID,argv[1]);
    printf("============CHATTING MESSAGE============\n");
    while(1){
	char inputStr[40];
	// print 'chat_notification'.
	printf("%s ",chatInfo->chat_notification);
	fgets(inputStr,40,stdin);

	// if inputStr is "..", print out next message (print the latest message.)
	if(0==strcmp(inputStr,"..\n")){
	    chatInfo->isDot++;
	}

	/*
	if excute many(more than one) shmwrite program,
	each chatter(writer) may compete for the chatInfo(actually,chatInfo[0]).
	so we don't need to use many space.
	the occupier change every time.
	*/

	else{
	    strcpy(chatInfo->userID,userID);
	    chatInfo->messageTime++;
	    strcpy(chatInfo->message,inputStr);
	    // if user enter "/bye", that program is terminated.
	    // from that moment, other user's chat_notification is changed like below.
	    if(0==strcmp(chatInfo->message,"/bye\n")){
            	strcpy(chatInfo->chat_notification,"YOUR FRIEND NOT IN HERE ANYMORE");
            	break;
            }
	}
    }
    return 0;
}
