//include
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
#include <iostream>

//defines
#define MAX_BUFFER_SIZE 100
#define MAX_PAGES 1000
#define TOSCH 10
#define FROMSCH 20  // FROMSCH+id will be used to read msg from sch
#define TOMMU 10
#define FROMMU 20 // FROMMMU+id will be used to read msg from MMU

//all sm_t and mq_t

using namespace std;


// Sends page number to MMU
struct mq3_t 
{
	long mtype;          /* Message type */
	int id;
	int pageno;
};


// Receives frame number from MMU
struct mq3_rt 
{
	long mtype;          /* Message type */
	int frameno;
};


// Ready Queue
struct mq1_t
{
	long mtype;          /* Message type */
	int id;
};

int main(int argc, char *argv[]) //argv[] ={id,mq1,mq3,ref_string}
{
	//convert args to proper type
	// for(int i=0; i<argc; i++)
		// cout<<"\nProcess created with args "<<argv[i]<<" at "<<i<<endl;

	int id = atoi(argv[1]);
	int mq1 = atoi(argv[2]);
	int mq3 = atoi(argv[3]);
	cout<<"\nprocess created with id "<<id<<endl;
	int *pn = new int[argc-4];
	for(int i=4; i<argc; i++)
	{
		pn[i-4]=atoi(argv[i]);
		cout<<argv[i]<<endl;
	}
	//get mq1, mq3
	//int mq1 = msgget(mq1_k, 0666);
	//int mq3 = msgget(mq3_k, 0666);
	if (mq1 == -1)
	{
		printf("Message Queue1 creation failed");
		exit(1);
	}
	if (mq3 == -1)
	{
		printf("Message Queue3 creation failed %d",mq3);
		exit(1);
	}
	
	//send on mq1 of type TOSCH with id as id
	mq1_t m;
	m.mtype=TOSCH;
	m.id=id;
	msgsnd(mq1, &m, sizeof(mq1_t) - sizeof(long), 0);
	int cpg = 0; //counter for page number array

	//wait on msg recv from scheduler mq1 of type FROMSCH + id
	mq1_t mm;
	redo:
	
	mm.mtype=FROMSCH + id;
	cout<<"\n\nWaiting on recv for someone to ask me to start\n\n";
	msgrcv(mq1, &mm, sizeof(mq1_t) - sizeof(long), FROMSCH + id,  0);
	cout<<"\n\nI am starting\n\n";

	
	while (cpg < argc-4)
	{
		// sending msg to mmu the page number
		
		mq3_t mmusend;
		//mmusend on mq3 of type TOMMU with current id and needed page no
		mmusend.mtype=TOMMU;
		mmusend.id=id;
		mmusend.pageno=pn[cpg];
		msgsnd(mq3, &mmusend, sizeof(mq3_t) - sizeof(long), 0);
		printf("%d has sent request for %d page number\n",id, pn[cpg]);

		//wait on mmurecv from mq3 of type FROMMU+id
		mq3_rt mmurecv;
		mmurecv.mtype = FROMMU+id;
		printf("\n\nWaiting on recv from MMU\n\n");
		int a = msgrcv(mq3, &mmurecv, sizeof(mq3_rt) - sizeof(long), FROMMU+id,  0);

		printf("%d Frame no. received\n", mmurecv.frameno);
		

		// if(mmurecv.frameno >= 0)
		// {
		// 	printf("Frame number from MMU received for process %d: %d\n" , id, mmurecv.frameno);
		// 	cpg++;
		// }
		if(mmurecv.frameno == -1) //here cpg will not be incremented
		{
			printf("Page fault occured for process %d\n", id);
			goto redo;
		}
		else if(mmurecv.frameno == -2)
		{
			printf("Invalid page reference for process %d. Process is terminating ...\n", id) ;
			exit(1);
		}
		else{
			printf("Frame number from MMU received for process %d: %d\n" , id, mmurecv.frameno);
			cpg++;
			continue;
		}
	}
	printf("\nOut of while loop\n");
	//mmusend on mq3 of type TOMMU with id as id and page no -9
	mq3_t mmusend;
	mmusend.mtype = TOMMU;
	mmusend.id = id;
	mmusend.pageno = -9;
	msgsnd( mq3, &mmusend, sizeof(mq3_t) - sizeof(long), 0);

	printf("Process %d Terminated successfly\n", id);

	// exit(1);
	return 0;
}


