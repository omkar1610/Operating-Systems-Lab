#include<bits/stdc++.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/msg.h>
#include<errno.h>


#define FROMPROCESS 10
#define TOPROCESS 20  // TOPROCESS+id will be used to read msg from sch
#define FROMMMU 20

#define PAGEFAULT_HANDLED 5
#define TERMINATED 10

using namespace std;


// Communication between MMU
struct mq2_t {
	long mtype;
	char mbuf[1];
} ;


// Ready Queue
struct mq1_t{
	long mtype;
	int id;
};


// Signal Handler
void sig_handler(int sig){
	exit(0);
}



int main(int argc , char * argv[])
{
	signal(SIGUSR2, sig_handler);


	int mq1 = atoi(argv[1]);
	int mq2 = atoi(argv[2]);
	int k = atoi(argv[3]);
	int master_pid = atoi(argv[4]);

	if (mq1 == -1){
		perror("Message Queue1 creation failed");
		exit(1);
	}
	if (mq2 == -1){
		perror("Message Queue2 creation failed");
		exit(1);
	}


	int terminated_process = 0;
	int curr_id;


	// Run loop till all processes are complete
	while (1){

		// If all processes are terminated, break
		if (terminated_process == k)
			break;

		// Receive process number at the front of Ready queue
		mq1_t msgrecv;
		msgrecv.mtype = FROMPROCESS;
		msgrcv(mq1, &msgrecv, sizeof(mq1_t) - sizeof(long), FROMPROCESS, 0);
		cout<<"Scheduling Process with id "<<msgrecv.id<<endl;
		

		curr_id = msgrecv.id;

		// Schedule the process
		mq1_t msgsend;
		msgsend.mtype=TOPROCESS + curr_id;
		msgsend.id=curr_id;
		msgsnd(mq1, &msgsend, sizeof(mq1_t) - sizeof(long), 0);

		
		// Receive Message from MMU
		mq2_t mmurecv;
		msgrcv(mq2, &mmurecv, sizeof(mq2_t) - sizeof(long), 0,  0);
		
		if (mmurecv.mtype == PAGEFAULT_HANDLED){
			// If MMU has sent PAGEFAULT_HANDLED, then enqueue it at the back
			msgsend.mtype = FROMPROCESS;
			msgsend.id = curr_id;
			cout<<"Page fault handled for process "<<msgrecv.id<<", readded to back: "<<endl;
			msgsnd(mq1, &msgsend, sizeof(mq1_t) - sizeof(long), 0);
			continue;
		}
		else if(mmurecv.mtype == TERMINATED){
			// If Process has TERMINATED, increase the count of terminated process
			cout<<"Terminated process: "<<msgrecv.id<<"\n\n";
			terminated_process++;
			continue;
		}
		else{
			printf("Wrong message from mmu\n");
			exit(1);
		}
	}

	printf("Scheduler terminating ...\n") ;
	kill(master_pid, SIGUSR1);
	pause();
	exit(1) ;
}

