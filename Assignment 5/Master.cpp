#include<bits/stdc++.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/msg.h>
#include<errno.h>

using namespace std;


// Page Table
struct sm1_t{
	int frame_no;
	bool valid;
};


// Free frame list
struct sm2_t{
	int curr;	// current free frames available
	int frame_list[];
};


// Process to page mapping
struct sm3_t{
	pid_t pid;
	int m;
};


int finished = 0;
pid_t pid1, pid2, pid3;


// Signal Handler
void sig_handler(int signo){
	finished = 1;
	cout<<"\nMaster terminating..."<<endl;
	
	kill(pid3, SIGTERM);
	kill(pid2, SIGUSR2);
}



int main(){
	int k, m, f;
	int i;

	pid1 = getpid();

	cout<<"Enter k (number of processes): ";
	cin>>k;
	cout<<"Enter m (max page number for any process): ";
	cin>>m;
	cout<<"Enter f (Number of frames): ";
	cin>>f;

	srand(time(NULL));

	signal(SIGUSR1, sig_handler);


	// Create Page Table
	key_t ptb_key = rand();
	int sm1id_pt = shmget(ptb_key, m*sizeof(sm1_t)*k, IPC_CREAT|0666);
	sm1_t *pt_t = (sm1_t *)shmat(sm1id_pt, NULL, 0);
	// Initialize Page Table		
	for(i=0; i<k*m; i++){
		pt_t[i].frame_no = -1;
		pt_t[i].valid = 0;
	}
	cout<<"Page Table Created..."<<endl;


	// Create Free Frame List
	key_t ff_key = rand();
	int sm2id_ff = shmget(ff_key, sizeof(sm2_t) + f*sizeof(int), IPC_CREAT|IPC_EXCL|0666);
	sm2_t *ff_t = (sm2_t *)shmat(sm2id_ff, NULL, 0);
	// Initialize free frame list
	for(i=0; i<f; i++){
		ff_t->frame_list[i] = -1;
	}
	ff_t->curr = f-1;



	// Create PCB
	key_t pcb_key = rand();
	int sm3id_pcb = shmget(pcb_key, k*sizeof(sm3_t), IPC_CREAT|IPC_EXCL|0666);
	sm3_t *pcb_t = (sm3_t *)shmat(sm3id_pcb, NULL, 0);
	// Initialize process to page map
	for(i=0; i<k; i++){
		pcb_t[i].pid = i;
		pcb_t[i].m = rand()%m + 1;
	}



	// MQ1
	key_t mq1_key = rand();
	int mq1id = msgget(mq1_key, IPC_CREAT|IPC_EXCL|0666);
	if (mq1id == -1)
	{
		printf("Message Queue1 creation failed %d",mq1_key);
		exit(1);
	}

	// MQ2
	key_t mq2_key = rand();
	int mq2id = msgget(mq2_key, IPC_CREAT|IPC_EXCL|0666);
	if (mq2id == -1)
	{
		printf("Message Queue1 creation failed %d",mq2_key);
		exit(1);
	}

	// MQ3
	key_t mq3_key = rand();
	int mq3id = msgget(mq3_key, IPC_CREAT|IPC_EXCL|0666);
	if (mq3id == -1)
	{
		printf("Message Queue1 creation failed %d",mq3_key);
		exit(1);
	}



	// Execute Scheduler
	if((pid2=fork()) == 0)
	{
		char *argv[9] ={"/usr/bin/xterm", "-hold", "-e", "./sched"};

		argv[4] = new char[to_string(mq1id).length() + 1];
		strcpy(argv[4], to_string(mq1id).c_str());		
		argv[5] = new char[to_string(mq2id).length() + 1];
		strcpy(argv[5], to_string(mq2id).c_str());
		argv[6] = new char[to_string(k).length() + 1];
		strcpy(argv[6], to_string(k).c_str());
		argv[7] = new char[to_string(pid1).length() + 1];
		strcpy(argv[7], to_string(pid1).c_str());
		argv[8]=NULL;
				
		execv(argv[0],argv);
		exit(0);
	}



	// Execute MMU
	if((pid3=fork()) == 0)
	{
		char *argv[13] ={"/usr/bin/xterm", "-hold", "-e", "./MMU"};

		argv[4] = new char[to_string(mq2id).length() + 1];
		strcpy(argv[4], to_string(mq2id).c_str());		
		argv[5] = new char[to_string(mq3id).length() + 1];
		strcpy(argv[5], to_string(mq3id).c_str());
		argv[6] = new char[to_string(sm1id_pt).length() + 1];
		strcpy(argv[6], to_string(sm1id_pt).c_str());
		argv[7] = new char[to_string(sm2id_ff).length() + 1];
		strcpy(argv[7], to_string(sm2id_ff).c_str());
		argv[8] = new char[to_string(sm3id_pcb).length() + 1];
		strcpy(argv[8], to_string(sm3id_pcb).c_str());
		argv[9] = new char[to_string(m).length() + 1];
		strcpy(argv[9], to_string(m).c_str());
		argv[10] = new char[to_string(k).length() + 1];
		strcpy(argv[10], to_string(k).c_str());
		argv[12]=NULL;

		execv(argv[0], argv);
	}


	// Execute k processes
	for(i=0; i<k; i++){

		// Form page reference string
		int x = rand() % (10*pcb_t[i].m - 2*pcb_t[i].m) + 2*pcb_t[i].m;
		int page_ref[x+1];
		int j;
		for(j=0; j<x; j++)
			page_ref[j] = rand()%m;
		page_ref[j]=-9;


		// Execute Process
		if(fork() == 0){
			int r=7;
			char* argv[x+r+2] ={"/usr/bin/xterm", "-hold", "-e", "./process"};
			
			argv[4] = new char[to_string(i).length() + 1];
			strcpy(argv[4] , to_string(i).c_str());
			argv[5] = new char[to_string(mq1id).length() + 1];
			strcpy(argv[5] , to_string(mq1id).c_str());
			argv[6] = new char[to_string(mq3id).length() + 1];
			strcpy(argv[6] , to_string(mq3id).c_str());
		
			for(j=r; j<x+1+r; j++)
			{
				argv[j] = new char[to_string(page_ref[j-r]).length() + 1];
				strcpy(argv[j], to_string(page_ref[j-r]).c_str());
			
				if(page_ref[j-r]==-9)
					break;
			}
			argv[j]=NULL;
			cout<<"Process "<<i<<" created"<<endl;
			cout<<"Page Reference string"<<endl;
			for(r=7; r<j; r++)
				cout<<argv[r]<<" ";
			cout<<"\n\n";

			execv(argv[0], argv);
			exit(0);
		}
		usleep(250000);
	}


	// Pause
	if(!finished)
		pause();

	// Remove the shared memory
	shmctl(sm1id_pt, IPC_RMID, NULL);
	shmctl(sm2id_ff, IPC_RMID, NULL);
	shmctl(sm3id_pcb, IPC_RMID, NULL);
	msgctl(mq1id, IPC_RMID, NULL);
	msgctl(mq2id, IPC_RMID, NULL);
	msgctl(mq3id, IPC_RMID, NULL);

	return 0;
}
