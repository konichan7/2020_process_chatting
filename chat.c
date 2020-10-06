#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>
#include"chatshm.h"

int main(int argc,char* argv[]){
    int shmid, dotChange;
    long timeChange;
    char userID[20];
    CHAT_INFO *chatInfo=NULL;
    void* shmaddr=(void*)0;
    // if there's no input string, print error message and exit.
    if(argc<2){
	fprintf(stderr,"[usage]: ./shrmwrite USERID\n");
        exit(-1);
    }
    // if shared memory is created, 'shmid' get its shmid.
    // Otherwise, 'shmid' get -1.
    shmid = shmget((key_t)20200406, sizeof(CHAT_INFO), 0666|IPC_CREAT|IPC_EXCL);
    // if shared memory already exist, just get it.
    if(shmid<0){
        shmid=shmget((key_t)20200406,sizeof(CHAT_INFO),0666);
	// shmaddr get the address of shared memory.
        shmaddr=shmat(shmid,(void*)0,0666);
	// if shmat function doesn't working correctly, print error message and exit.
        if(shmaddr<(void*)0){
            perror("attach falied");
            exit(-1);
        }
    }
    // shared memory is created correctly, get address of shared memory.
    else{
        shmaddr=shmat(shmid,(void*)0,0666);
    }
    // chatInfo point to shmaddr which is the address of shared memory.
    chatInfo=(CHAT_INFO*)shmaddr;

    // if very first input string is "read", run READING program.
    if(0==strcmp(argv[1],"read")){

	// initialize dotChange and timeChange.
        dotChange=chatInfo->isDot;
    	timeChange=chatInfo->messageTime;

	printf("=============CHAT MESSAGE===============\n");
	while(1){
	    // when user enter "..", (chatInfo->isDot) is increased by 1.
	    if((chatInfo->isDot)>dotChange){
	        printf("[%s] %ld : %s\n",chatInfo->userID,chatInfo->messageTime,
        		                        chatInfo->message);
            	dotChange++;
            }
	    // messageTime is only increased by 1 when new message is entered.
            else{
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
    }
    //if first input string(argv[1]) is not "read", run WRITING program.
    else{
	// set the default string value of 'chat_notification'
	strcpy(chatInfo->chat_notification,"ENTER THE MESSAGE");
	// argv[1] means the first input string which is the ID of user's.
	strcpy(userID,argv[1]);
	printf("============CHATTING MESSAGE============\n");
    	while(1){
	    char inputStr[40];
	    printf("%s ",chatInfo->chat_notification);
	    fgets(inputStr,40,stdin);
	    // if inputStr is "..", print out next message (print the latest message.)
	    if(0==strcmp(inputStr,"..\n")){
         	chatInfo->isDot++;
            }
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
    }
    return 0;
}
