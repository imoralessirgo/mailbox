#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>


#define REQUEST 1
#define REPLY 2



//GLOBALS
#define MAXTHREAD 10

typedef struct msg msg;
typedef struct mailbox mailbox;
int numThreads;



struct msg{
	int iFrom; /* who sent the message (0 .. number-of-threads) */
	int value; /* its value */
	int cnt; /* count of operations (not needed by all msgs) */
	int tot; /* total time (not needed by all msgs) */
};

struct mailbox{
	sem_t psem;
	sem_t csem;
	msg message;
};

//mailbox array 
mailbox mbarr[MAXTHREAD]; 
pthread_t threadID[MAXTHREAD];



void initMailBox(int threadIndex){
	sem_t sem1,sem2;
	sem_init(&sem1,0,1); //producer
	sem_init(&sem2,0,0); //consumer
	msg msg = {0,0,0,0}; 
	mailbox mb = {sem1,sem2,msg};
	mbarr[threadIndex] = mb;
}
void sendMsg(int iTo, msg *pMsg){
	/*if(iTo < 0){
		printf("The result from thread %d is %d from %d operations during %d secs.",pMsg->iFrom,pMsg->value,pMsg->cnt,pMsg->tot);
		return;
	}*/
	sem_wait(&mbarr[iTo].psem);
	int val =  pMsg->value;
	mbarr[iTo].message.value = pMsg->value;
	val = 0;
	val = mbarr[iTo].message.value;
	sem_post(&mbarr[iTo].csem);
	return;		
}

void recvMsg(int iRecv, msg *pMsg){
	sem_wait(&mbarr[iRecv].csem);
	pMsg->value = mbarr[iRecv].message.value;	
	sem_post(&mbarr[iRecv].psem);
	int arr =  mbarr[iRecv].message.value;
	arr = pMsg->value;
	return;
}



void* adder(void* arg){
	int index = (long) arg;
	msg message;
	int count = 1;
	int  numOps = -1;
	time_t start_time = time(NULL);
	do{	
        	recvMsg(index,&message);
		numOps++;
		count += message.value;	
	}while(message.value > 0);
	message.iFrom = index + 1;
	message.value = count;
	message.cnt = numOps;
	message.tot = ((int)(time(NULL)-start_time));	
	
	printf("The result from thread %d is %d from %d operations during %d secs. \n",message.iFrom,message.value,message.cnt,message.tot);
	return;
}


void main(int argc,char* argv[]){
	if(argc < 2 || atoi(argv[1]) > 10){
		printf("Enter a number of threads lower than 11");
		exit(0);
	}else{
		numThreads = atoi(argv[1]);
		for(int i = 0; i<numThreads;i++){
			int index = i;
			initMailBox(i);
			pthread_create(&threadID[i],NULL,adder,(void *)index);
		}
	}
//	char buff[5];
	msg msg;
	int value, threadNum;
	while(1){
	//	buff = "2 5\n";
		char buff[100] = "";
		fgets(buff,100,stdin);
		if(buff[0] != '\n'){
			sscanf(buff,"%d %d",&value,&threadNum);
			if(value < 0){ // recieved negative input, ignore & prompt user again
				printf("%d id not a valid message value.\n",value);
			}else if(threadNum > numThreads){ // mailbox num not valid ignore & promt user again
				printf("Requested a larger number than existing threads\n");
			}else{ // mailbox and message valid, send message
				threadNum = threadNum - 1; //align to array index
				msg.value = value; // set message value 
				sendMsg(threadNum,&msg); // send message
			}	
		}else if(buff[0] == '\n'){ // found singnal to terminate
			msg.value = -1;
			for(int i = 0; i<numThreads;i++){ //for each thread deliver termination message
				sendMsg(i,&msg);
			}  //terminte all threads 
			break;
		}
	}

	for(int i = 0; i<numThreads;i++){ // for each child thread
		pthread_join(threadID[i],NULL); // wait on thread
		sem_destroy(&mbarr[i].psem); // destroy producer semaphore
		sem_destroy(&mbarr[i].csem); // destroy consumer semaphore 
	}
	//free(mbarr);
	//free(threadID);

}
