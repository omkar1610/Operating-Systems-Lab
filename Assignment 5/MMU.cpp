//includes
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
//defines
#define PSEND_TYPE 10
#define MMUTOPRO 20
#define INVALID_PAGE_REF -2
#define PAGEFAULT -1
#define PROCESS_OVER -9
#define PAGEFAULT_HANDLED 5
#define TERMINATED 10

using namespace std;
//struct all sm_t and mq_t
// Page Table
char resultf[1000];


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


// PCB
struct sm3_t{
	pid_t pid;
	int m;
	int f_cnt;
	int f_allo;
};


// Receives Page number from Process
struct mq3_rt 
{
	long mtype;          /* Message type */
	int id;
	int pageno;
};


// Sends Page number to Process
struct mq3_st 
{
	long mtype;          /* Message type */
	int frameno;
};


// Communication with Scheduler
struct mq2_t {
	long mtype;
	char mbuf[1];
};


//all pointers and keys and ids
int sm1,sm2,sm3,mq1,mq2,mq3;
key_t sm1k,sm2k,sm3k,mq1k,mq2k,mq3k;
sm1_t* sm1ptr;
sm2_t* sm2ptr;
sm3_t* sm3ptr;

ofstream fout("result.txt");

int m,k,ktemp;
int** rk;

int handlePageFault(int id, int pageno)
{
	int i;
	//if (free frame available ie current not -1)
	if (sm2ptr->curr != -1)
	{
		//select an free frame
		fout<<"\nFree frame available\n";
		int freeframeindex=-1, maxwt=-1;
		int p=0;
		for (;p<k;p++)
		{
			if(sm2ptr->frame_list[p] == -1)
			{
				break;
			}
		}
		//store 1 more than the greatest no in freeframe indexes here
		int frametoinvalidate = p;
		//all pages with this frame number to be invalidated
		for(int i=0;i<k;i++){
			for(int j=0;j<m;j++)
			{
				if(sm1ptr[i*m+j].valid==1 && sm1ptr[i*m+j].frame_no==frametoinvalidate)
					sm1ptr[i*m+j].valid=0;
			
			}
		}
					
		//validate the (page id,page no) with current frame no
		sm1ptr[id*m+pageno].valid=1;
		sm1ptr[id*m+pageno].frame_no=p;
		//reduce current available frame no
		sm2ptr->curr--;
		return p;
	}
	else
	{
		fout<<"\nFree frame unavailable\n";
		int maxwtindex=0, maxwt=-1;
		for (int p=0;p<k;p++)
		{
			if(sm2ptr->frame_list[p]>maxwt)
			{
				maxwt=sm2ptr->frame_list[p];	
				maxwtindex=p;
			}
		}
		//store 1 more than the greatest no in freeframe indexes here
		int frametoinvalidate=maxwtindex;
		fout<<"\n"<<frametoinvalidate<<" is victim frame\n";
		//all pages with this frame number to be invalidated
		for(int i=0;i<k;i++){
			for(int j=0;j<m;j++)
			{
				if(sm1ptr[i*m+j].valid==1 && sm1ptr[i*m+j].frame_no==frametoinvalidate)
					sm1ptr[i*m+j].valid=0;
			
			}
		}			
		//validate the (page id,page no) with current frame no
		sm1ptr[id*m+pageno].valid=1;
		sm1ptr[id*m+pageno].frame_no=maxwtindex;
		return frametoinvalidate;
	}
}

void freepages(int i)
{
	//for (all pt entries associated with process i)
	printf("\nprocess %d related stuff being cleaned\n",i);
	for(int p=0; p < sm3ptr[i].m; p++)
	{
		if(sm1ptr[i * m + p].valid == 1)
		{
			//mark associated frame as free
			sm2ptr->frame_list[(sm1ptr[i * m + p].frame_no)] = -1;
			sm2ptr->curr++;
			//invalidate the page
			sm1ptr[i * m + p].valid=0;
		}
	}
}


int finished = 0;
void handletgerm(int sig)
{
	// fout<<"\n\nHere are the results, process no on horizontal axis\n\n";
	// for(int i=0;i<k;i++)
	// {
	// 	fout<<"\n";
	// 	for(int j=0;j<m;j++)
	// 		fout<<"\t"<<rk[i][j]; 
	// }
	// fout.close();
	finished = 1;
	// exit(0);
}


int count=0;
int serviceMRequest()
{
	//get pointer to all sm
	sm3ptr = (sm3_t*)(shmat(sm3, NULL, 0));
	sm1ptr = (sm1_t*)(shmat(sm1, NULL, 0));
	sm2ptr = (sm2_t*)(shmat(sm2, NULL, 0));
	int id = -1, pageno;
	

	//read message on mq3 of type PSEND_TYPE, put id in id and pageno in pageno
	mq3_rt mttt;
	fout<<"\nNo of process left:"<<ktemp<<"\n";
	if(ktemp==0)
	{

		finished=1;
		return 0;
	}
	fout<<"\nwaiting on some process to ask me something\n";
	int rst = msgrcv(mq3, &mttt, sizeof(mq3_rt)-sizeof(long), PSEND_TYPE, 0);
	if(rst == -1)
	{
		if(errno == EINTR)
			return -1;
		perror("msgrcv");
		exit(EXIT_FAILURE);
	}

	id=mttt.id;
	pageno=mttt.pageno;
	if(pageno == -1 && id == -1)
	{
		return 0;
	}
	int i = id;
	if(pageno == PROCESS_OVER)
	{
		freepages(id);
		//send message of type TERMINATED on mq2
		mq2_t mt;
		mt.mtype=TERMINATED;
		ktemp--;
		fout<<"\n"<<id<<" terminated sent to sched\n";
		msgsnd( mq2, &mt, sizeof(mq2_t) - sizeof(long), 0);
		return 0;
	}

	//stuff to print
	count ++;
	// printf("Page reference : (%d,%d,%d)\n",count,id,pageno);
	fout<<"Page reference : ("<<count<<","<<id<<","<<pageno<<")\n";
		
	if(sm3ptr[id].m < pageno || pageno < 0)
	{
		//Invalid reference
		fout<<"Invalid Page Reference : ("<<id<<","<<pageno<<")\n";
		// fprintf(fout,"Invalid Page Reference : (%d, %d)\n",id,pageno);
		//send msg on mq3 of type id + MMUTOPRO with INVALID_PAGE_REF as frame no
		mq3_st m;
		m.mtype = id + MMUTOPRO;
		m.frameno = INVALID_PAGE_REF;
		fout<<"\n"<<id<<" told about invalid page reference.\n";
		msgsnd(mq3, &m, sizeof(mq3_st) - sizeof(long), 0);
		// printf("Process %d: TRYING TO ACCESS INVALID PAGE REFERENCE %d\n", id, pageno);
		fout<<"Process "<<id<<": TRYING TO ACCESS INVALID PAGE REFERENCE "<<pageno<<"\n";
		freepages(id);
		//send message of type TERMINATED on mq2
		mq2_t mt;
		mt.mtype=TERMINATED;
		ktemp--;
		fout<<"\n"<<id<<" terminated sent to sched\n";
		msgsnd(mq2, &mt, sizeof(mq2_t) - sizeof(long), 0);

	}
	else
	{
		//valid message
		if (sm1ptr[i*m + pageno].valid == 0)
		{
			//PAGE FAULT
			// printf("Page Fault : (%d, %d)\n",id,pageno);
			fout<<"Page Fault : ("<<id<<","<<pageno<<")\n";
			//send msg on mq3 of type id + MMUTOPRO with -1 as frame no
			mq3_st mtt;
			mtt.mtype = id + MMUTOPRO;
			mtt.frameno = -1;

			rk[id][pageno]++;
			
			fout<<"\n"<<id<<" told about page fault.\n";
			msgsnd(mq3, &mtt, sizeof(mq3_st) - sizeof(long), 0);
			// printf("Process %d: TRYING TO ACCESS INVALID PAGE REFERENCE %d\n", id, pageno);
			fout<<"Process :"<<id<<" PAGE FAULT:"<<pageno<<"\n";
			
			//get fno = handlePageFault(id, pageno) which is substitute frame received
			int fno = handlePageFault(id, pageno);
			sm1ptr[i*m + pageno].valid = 1;
			sm1ptr[i*m + pageno].frame_no = fno;
			//send message of type PAGEFAULT_HANDLED on mq2
			fout<<"\nPage fault handled\n";
			mq2_t mt;
			mt.mtype = PAGEFAULT_HANDLED;
			msgsnd(mq2, &mt, sizeof(mq2_t) - sizeof(long), 0);
			fout<<"\nscheduler informed about"<<id<<" s pause.\n";
		}
		else
		{
			fout<<"Page Found\n";
			//send msg on mq3 of type id + MMUTOPRO with foundframeno as frame no
			mq3_st mt;
			mt.mtype = id + MMUTOPRO;
			mt.frameno = sm1ptr[i*m+pageno].frame_no;
			fout<<"\n"<<id<<" told about the frame no.\n";
			msgsnd(mq3, &mt, sizeof(mq3_st) - sizeof(long), 0);
			
		}
	}
}



int main(int argc, char const *argv[])//msgq2id msgq3id ptbid freelid pcbid m k
{
	//convert all arguments to proper type;
	mq2 = atoi(argv[1]);
	mq3 = atoi(argv[2]);
	sm1 = atoi(argv[3]);
	sm2 = atoi(argv[4]);
	sm3 = atoi(argv[5]);
	m = atoi(argv[6]);
	k = atoi(argv[7]);
	ktemp=k;
	
	signal(SIGUSR2, handletgerm);
	signal(SIGTERM, handletgerm);

	//2d array indexed by processno,pageno
	rk = new int*[k];

	for(int i=0; i<k; i++)
		rk[i] = new int[m];

	int** resultkeeper;
	resultkeeper = rk;
	for(int i=0; i<k; i++)
		for(int j=0; j<m; j++)
			rk[i][j] = 0; 


	printf("\nAbout to enter while loop\n");
	while(!finished)
	{
		serviceMRequest();
	}
	printf("\nOut of while loop\n");

	//output all results
	fout<<"\n\nHere are the results, process no on horizontal axis\n\n";
	for(int i=0;i<k;i++)
	{
		fout<<"\n";
		for(int j=0;j<m;j++)
			fout<<"\t"<<rk[i][j]; 
	}
	fout.close();
	return 0;
}
