#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/msg.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <ctype.h>
#include <sys/shm.h>

#define MAX_BUFFER_SIZE 100
#define MAX_PAGES 1000
#define MAX_PROCESS 1000
#define FROMPROCESS 10
#define TOPROCESS 20  // TOPROCESS+id will be used to read msg from sch
// #define TOMMU 10
#define FROMMMU 20

#define PAGEFAULT_HANDLED 5
#define TERMINATED 10


struct mq2_t {
	long    mtype;
	char mbuf[1];
} ;


struct mq1_t {
	long    mtype;
	int id;
} ;




int k; //no. of processes

int main(int argc , char * argv[])//mq1_key, mq2_key,k, master_pid
{
	//convert all args to proper type
	int mq1 = atoi(argv[1]);
	int mq2 = atoi(argv[2]);
	k = atoi(argv[3]);
	int master_pid = atoi(argv[4]);
	//get mq1, mq2
	//int mq1 = msgget(mq1_key, 0666);
	//int mq2 = msgget(mq2_key, 0666);
	if (mq1 == -1)
	{
		perror("Message Queue1 creation failed");
		exit(1);
	}
	if (mq2 == -1)
	{
		perror("Message Queue2 creation failed");
		exit(1);
	}
	int terminated_process = 0;
	int curr_id;
	while (1)
	{
		//We can imagine all messages of type FROMPROCESS forming the ready queue
		//msg of type TOPROCESS+processno are meant for the processno process
		
		//read message of type FROMPROCESS from mq1 into msgrecv
		mq1_t msgrecv;
		msgrecv.mtype=FROMPROCESS;
		msgrcv( mq1, &msgrecv, sizeof(mq1_t) - sizeof(long), FROMPROCESS,  0);
		
		//set curr_id = msgrecv.id;
		curr_id = msgrecv.id;
		//make msgsend.mtype = TOPROCESS + curr_id;
		//send msgsend to mq1;
		mq1_t msgsend;
		
		msgsend.mtype=TOPROCESS + curr_id;
		msgsend.id=curr_id;
		msgsnd( mq1, &msgsend, sizeof(mq1_t) - sizeof(long), 0);
		
		
		//mmurecv from mq2
		mq2_t mmurecv;
		
		msgrcv( mq2, &mmurecv, sizeof(mq2_t) - sizeof(long), 0,  0);
		
		if (mmurecv.mtype == PAGEFAULT_HANDLED)
		{
			msgsend.mtype = FROMPROCESS;
			msgsend.id=curr_id;
			//send msg_send on mq1;
			msgsnd( mq1, &msgsend, sizeof(mq1_t) - sizeof(long), 0);
		}
		else if (mmurecv.mtype == TERMINATED)
		{
			terminated_process++;
		}
		else
		{
			printf("Wrong message from mmu\n");
			exit(1);
		}
		if (terminated_process == k)
			break;
	}
	kill(master_pid, SIGUSR1);
	pause();
	printf("Scheduler terminating ...\n") ;
	exit(1) ;
}

