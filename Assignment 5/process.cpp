#include<bits/stdc++.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/msg.h>
#include<errno.h>


#define TOSCH 10
#define FROMSCH 20  // FROMSCH+id will be used to read msg from sch
#define TOMMU 10
#define FROMMU 20 // FROMMMU+id will be used to read msg from MMU


using namespace std;


// Sends page number to MMU
struct mq3_t 
{
	long mtype;
	int id;
	int pageno;
};


// Receives frame number from MMU
struct mq3_rt 
{
	long mtype;
	int frameno;
};


// Ready Queue
struct mq1_t
{
	long mtype;
	int id;
};



int main(int argc, char *argv[])
{

	int id = atoi(argv[1]);
	int mq1 = atoi(argv[2]);
	int mq3 = atoi(argv[3]);
	cout<<"Process created with id "<<id<<endl;
	cout<<"Page Reference String"<<endl;

	int *pn = new int[argc-4];
	for(int i=4; i<argc; i++){
		pn[i-4]=atoi(argv[i]);
		cout<<argv[i]<<" ";
	}
	cout<<"\n\n";

	if(mq1 == -1){
		printf("Message Queue1 creation failed");
		exit(1);
	}
	if(mq3 == -1){
		printf("Message Queue3 creation failed %d",mq3);
		exit(1);
	}
	
	// Inform the process number to scheduler
	mq1_t m;
	m.mtype=TOSCH;
	m.id=id;
	msgsnd(mq1, &m, sizeof(mq1_t) - sizeof(long), 0);

	int cpg = 0; //counter for page number array


	// Wait to be scheduled by Scheduler
	mq1_t mm;

	redo:
	mm.mtype=FROMSCH + id;
	msgrcv(mq1, &mm, sizeof(mq1_t) - sizeof(long), FROMSCH + id,  0);

	cout<<"\nI am starting\n"<<endl;

	
	// Loop until Page reference string exhausts
	while(cpg < argc-4){
		
		// Send page number to MMU
		mq3_t mmusend;
		mmusend.mtype = TOMMU;
		mmusend.id = id;
		mmusend.pageno = pn[cpg];
		msgsnd(mq3, &mmusend, sizeof(mq3_t) - sizeof(long), 0);
		cout<<"Request for page number "<<pn[cpg]<<endl;

		// Recieve frame number
		mq3_rt mmurecv;
		mmurecv.mtype = FROMMU+id;
		msgrcv(mq3, &mmurecv, sizeof(mq3_rt) - sizeof(long), FROMMU+id,  0);

		cout<<"Frame number "<<mmurecv.frameno<<" received"<<endl;
		


		if(mmurecv.frameno == -1){
			cout<<"Page Fault occured..."<<endl;
			goto redo;
		}
		else if(mmurecv.frameno == -2){
			cout<<"Invalid Page reference. Process Terminating..."<<endl;
			exit(1);
		}
		else{
			cpg++;
			continue;
		}
	}

	// Send -9 to MMU to sigal the end of page requests
	mq3_t mmusend;
	mmusend.mtype = TOMMU;
	mmusend.id = id;
	mmusend.pageno = -9;
	msgsnd( mq3, &mmusend, sizeof(mq3_t) - sizeof(long), 0);

	cout<<"Process "<<id<<" terminating..."<<endl;

	return 0;
}


