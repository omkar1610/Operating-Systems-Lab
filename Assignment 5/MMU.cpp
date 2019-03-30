#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <limits.h>
#include <math.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fstream>
#include <iostream>


#define PSEND_TYPE 10
#define MMUTOPRO 20
#define INVALID_PAGE_REF -2
#define PAGEFAULT -1
#define PROCESS_OVER -9
#define PAGEFAULT_HANDLED 5
#define TERMINATED 10

using namespace std;


// Page Table
struct sm1_t{
	int frame_no;
	bool valid;
};


// Free frame list
struct sm2_t{
	int curr;
	int frame_list[];
};


// Process to page mapping
struct sm3_t{
	pid_t pid;
	int m;
};


// Receives Page number from Process
struct mq3_rt{
	long mtype;
	int id;
	int pageno;
};


// Sends Page number to Process
struct mq3_st {
	long mtype;
	int frameno;
};


// Communication with Scheduler
struct mq2_t {
	long mtype;
	char mbuf[1];
};



int sm1,sm2,sm3,mq1,mq2,mq3;
sm1_t* sm1ptr;
sm2_t* sm2ptr;
sm3_t* sm3ptr;


ofstream fout("result.txt");

int m, k, ktemp;
int **rk;



// Function to handle Page Fault
int handlePageFault(int id, int pageno)
{
	int i;
	
	// If free frame is available
	if (sm2ptr->curr != -1)
	{
		// Select the free frame
		int freeframeindex = -1;
		int p = 0;

		for (; p<k; p++){
			if(sm2ptr->frame_list[p] == -1)
				break;
		}

		//store 1 more than the greatest no in freeframe indexes here
		int frametoinvalidate = p;
		
		// Invalidate pages with this frame
		for(int i=0;i<k;i++){
			for(int j=0;j<m;j++){
					if(sm1ptr[i*m+j].valid==1 && sm1ptr[i*m+j].frame_no==frametoinvalidate)
					sm1ptr[i*m+j].valid=0;
			}
		}
					
		// Validate the page table with the free frame
		sm1ptr[id*m+pageno].valid=1;
		sm1ptr[id*m+pageno].frame_no=p;

		sm2ptr->curr--;
		return p;
	}
	else{
		// Selec victim frame according to LRU
		int maxwtindex = 0, maxwt = -1;
		for (int p=0; p<k; p++){
			if(sm2ptr->frame_list[p]>maxwt){
				maxwt=sm2ptr->frame_list[p];	
				maxwtindex=p;
			}
		}

		int frametoinvalidate=maxwtindex;

		// Invalidate pages with this frame
		for(int i=0; i<k; i++){
			for(int j=0; j<m; j++){
				if(sm1ptr[i*m+j].valid==1 && sm1ptr[i*m+j].frame_no==frametoinvalidate)
					sm1ptr[i*m+j].valid=0;
			}
		}			

		// Validate the page table with the free frame
		sm1ptr[id*m+pageno].valid=1;
		sm1ptr[id*m+pageno].frame_no=maxwtindex;
		return frametoinvalidate;
	}
}


// Free pages for a process
void freepages(int i){
	for(int p=0; p < sm3ptr[i].m; p++){
		if(sm1ptr[i * m + p].valid == 1){

			//mark associated frame as free
			sm2ptr->frame_list[(sm1ptr[i * m + p].frame_no)] = -1;
			sm2ptr->curr++;
			//invalidate the page
			sm1ptr[i * m + p].valid=0;
		}
	}
}



int finished = 0;
void sig_handler(int sig){
	finished = 1;
}


int count = 0; // Maintains a global time stamp

// Returns frmae number for a corresponding page number
int serviceMRequest(){

	sm3ptr = (sm3_t*)(shmat(sm3, NULL, 0));
	sm1ptr = (sm1_t*)(shmat(sm1, NULL, 0));
	sm2ptr = (sm2_t*)(shmat(sm2, NULL, 0));


	int id = -1, pageno;
	
	// Read page number sent by process
	mq3_rt mttt;

	if(ktemp==0){
		finished=1;
		return 0;
	}

	int rst = msgrcv(mq3, &mttt, sizeof(mq3_rt)-sizeof(long), PSEND_TYPE, 0);
	if(rst == -1){
			if(errno == EINTR)
			return -1;
	}

	id = mttt.id;
	pageno = mttt.pageno;

	// If page number or id is negative, return
	if(pageno == -1 && id == -1)
		return 0;


	// If process is over
	if(pageno == PROCESS_OVER){
		// Free the pages allocated to the process
		freepages(id);

		// Send a TERMINATED message to the scheduler
		mq2_t mt;
		mt.mtype=TERMINATED;
		ktemp--;
		msgsnd( mq2, &mt, sizeof(mq2_t) - sizeof(long), 0);
		return 0;
	}


	count++;

	fout<<"Page reference : ("<<count<<","<<id<<","<<pageno<<")"<<endl;
		
	// If invalid page number
	if(sm3ptr[id].m < pageno || pageno < 0){
		fout<<"\nInvalid Page Reference : ("<<id<<","<<pageno<<")"<<endl;

		// Send frame number = -2 to process
		mq3_st m;
		m.mtype = id + MMUTOPRO;
		m.frameno = -2;
		msgsnd(mq3, &m, sizeof(mq3_st) - sizeof(long), 0);
		fout<<"Process "<<id<<": TRYING TO ACCESS INVALID PAGE REFERENCE\n"<<endl;
		// Free pages allocated to the process
		freepages(id);

		// Send TERMINATED message to scheduler
		mq2_t mt;
		mt.mtype = TERMINATED;
		ktemp--;
		msgsnd(mq2, &mt, sizeof(mq2_t) - sizeof(long), 0);
	}
	else{
		
		// If frame is not valid
		if (sm1ptr[id*m + pageno].valid == 0)
		{
			// Page fault has occured
			fout<<"Page Fault : ("<<id<<","<<pageno<<")\n";
			
			// Send frmae number = -1 to process
			mq3_st mtt;
			mtt.mtype = id + MMUTOPRO;
			mtt.frameno = -1;
			msgsnd(mq3, &mtt, sizeof(mq3_st) - sizeof(long), 0);

			rk[id][pageno]++;
			
			fout<<"Process "<<id<<": PAGE FAULT\n"<<endl;
			
			// Handle page fault and get a frame
			int fno = handlePageFault(id, pageno);
			sm1ptr[id*m + pageno].valid = 1;
			sm1ptr[id*m + pageno].frame_no = fno;
			
			// Send PAGE_FAULT HANDLED to scheduler
			mq2_t mt;
			mt.mtype = PAGEFAULT_HANDLED;
			msgsnd(mq2, &mt, sizeof(mq2_t) - sizeof(long), 0);
		}
		else{
			fout<<"Page Found\n"<<endl;
			
			// Send frame number to process
			mq3_st mt;
			mt.mtype = id + MMUTOPRO;
			mt.frameno = sm1ptr[id*m+pageno].frame_no;
			msgsnd(mq3, &mt, sizeof(mq3_st) - sizeof(long), 0);
		}
	}
}



int main(int argc, char const *argv[])
{
	mq2 = atoi(argv[1]);
	mq3 = atoi(argv[2]);
	sm1 = atoi(argv[3]);
	sm2 = atoi(argv[4]);
	sm3 = atoi(argv[5]);
	m = atoi(argv[6]);
	k = atoi(argv[7]);
	ktemp=k;
	
	signal(SIGUSR2, sig_handler);
	signal(SIGTERM, sig_handler);

	// 2D array that stores page fault details
	rk = new int*[k];

	for(int i=0; i<k; i++)
		rk[i] = new int[m];

	// int** resultkeeper;
	// resultkeeper = rk;
	for(int i=0; i<k; i++)
		for(int j=0; j<m; j++)
			rk[i][j] = 0; 


	while(!finished)
		serviceMRequest();


	// Output all results
	fout<<"\n\n#####################################################"<<endl<<endl;
	fout<<"Process_Number\t   Page_Faults"<<endl;
	int sum;
	for(int i=0; i<k; i++){
		fout<<i<<"\t\t\t";
		sum = 0;
		for(int j=0; j<m; j++)
			sum += rk[i][j];
		fout<<sum<<endl; 
	}

	fout.close();
	return 0;
}
