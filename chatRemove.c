#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include"chatshm.h"

int main(void){
    int shmid;
    shmid=shmget((key_t)20200406,sizeof(CHAT_INFO),0666);
    if(shmid<0){
	perror("shmget is failed");
	exit(-1);
    }
    if(shmctl(shmid,IPC_RMID,0)<0){
	printf("failed to delete shared memory\n");
	exit(-1);
    }
    else{
	printf("successfully delete shared memory\n");
    }
    return 0;
}
