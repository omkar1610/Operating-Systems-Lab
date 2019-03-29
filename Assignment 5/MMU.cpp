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
struct sm1_t{
	int frame_no;
	bool valid;
};


// Free frame list
struct sm2_t{
	int current;	// current free frames available
	int framelist[];
};


// PCB
struct sm3_t{
	pid_t pid;
	int m;
	int f_cnt;
	int f_allo;
};

struct mq3_rt 
{
	long    mtype;          /* Message type */
	int id;
	int pageno;
};

struct mq3_st 
{
	long    mtype;          /* Message type */
	int frameno;
};

struct mq2_t {
	long    mtype;
	char mbuf[1];
} ;
//all pointers and keys and ids
int sm1,sm2,sm3,mq1,mq2,mq3;
key_t sm1k,sm2k,sm3k,mq1k,mq2k,mq3k;
sm1_t* sm1ptr;
sm2_t* sm2ptr;
sm3_t* sm3ptr;

int m,k;

int handlePageFault(int id, int pageno)
{
	int i;
	//if (free frame available ie current not -1)
	if (sm2ptr->current!=-1)
	{
		//select an free frame
		int freeframeindex=-1, maxwt=-1;
		int p=0;
		for (;p<k;p++)
		{
			if(sm2ptr->framelist[p]==-1)
			{
				break;
			}
		}
		//store 1 more than the greatest no in freeframe indexes here
		int frametoinvalidate=p;
		//all pages with this frame number to be invalidated
		for(int i=0;i<k;i++)
			for(int j=0;j<m;j++)
			{
				if(sm1ptr[i*m+j].valid==1 && sm1ptr[i*m+j].frame_no==frametoinvalidate)
					sm1ptr[i*m+j].valid=0;
			
			}
					
		//validate the (page id,page no) with current frame no
		sm1ptr[id*m+pageno].valid=1;
		sm1ptr[id*m+pageno].frame_no=p;
		//reduce current available frame no
		sm2ptr->current--;
	}
	else
	{
		int maxwtindex=0, maxwt=-1;
		for (int p=0;p<k;p++)
		{
			if(sm2ptr->framelist[p]>maxwt)
			{
				maxwt=sm2ptr->framelist[p];	
				maxwtindex=p;
			}
		}
		//store 1 more than the greatest no in freeframe indexes here
		int frametoinvalidate=maxwtindex;
		//all pages with this frame number to be invalidated
		for(int i=0;i<k;i++)
			for(int j=0;j<m;j++)
			{
				if(sm1ptr[i*m+j].valid==1 && sm1ptr[i*m+j].frame_no==frametoinvalidate)
					sm1ptr[i*m+j].valid=0;
			
			}
					
		//validate the (page id,page no) with current frame no
		sm1ptr[id*m+pageno].valid=1;
		sm1ptr[id*m+pageno].frame_no=maxwtindex;
		
	}
}

void freepages(int i)
{

	int k = 0;
	//for (all pt entries associated with process i)
	for (int p=0;p<m;p++)
	{
		if (sm1ptr[i * m + p].valid==1)
		{
			//mark associated frame as free
			sm2ptr->framelist[(sm1ptr[i * m + p].frame_no)]=-1;
			sm2ptr->current++;
			//invalidate the page
			sm1ptr[i * m + p].valid=0;
		}
	}
}


int finished = 0;
void handletgerm(int sig)
{
	finished = 1;
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
	int rst = msgrcv(mq3, &mttt, sizeof(mq3_rt)-sizeof(long), PSEND_TYPE, 0);
	if (rst == -1)
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
	if (pageno == PROCESS_OVER)
	{
		freepages(id);
		//send message of type TERMINATED on mq2
		mq2_t mt;
		mt.mtype=TERMINATED;
		msgsnd( mq2, &mt, sizeof(mq2_t) - sizeof(long), 0);
		return 0;
	}
	//stuff to print
	count ++;
	printf("Page reference : (%d,%d,%d)\n",count,id,pageno);
	//fprintf(resultf,"Page reference : (%d,%d,%d)\n",count,id,pageno);
	if (sm3ptr[id].m < pageno || pageno < 0)
	{
		//Invalid reference
		printf("Invalid Page Reference : (%d %d)\n",id,pageno);
		//fprintf(resultf,"Invalid Page Reference : (%d %d)\n",id,pageno);
		//send msg on mq3 of type id + MMUTOPRO with INVALID_PAGE_REF as frame no
		mq3_st m;
		m.mtype=id + MMUTOPRO;
		m.frameno=INVALID_PAGE_REF;
		msgsnd( mq3, &m, sizeof(mq3_st) - sizeof(long), 0);
		printf("Process %d: TRYING TO ACCESS INVALID PAGE REFERENCE %d\n", id, pageno);
		freepages(id);
		//send message of type TERMINATED on mq2
		mq2_t mt;
		mt.mtype=TERMINATED;
		msgsnd( mq2, &mt, sizeof(mq2_t) - sizeof(long), 0);

	}
	else
	{
		//valid message
		if (sm1ptr[i*m + pageno].valid == 0)
		{
			//PAGE FAULT
			printf("Page Fault : (%d, %d)\n",id,pageno);
			//fprintf(resultf,"Page Fault : (%d, %d)\n",id,pageno);
			//send msg on mq3 of type id + MMUTOPRO with -1 as frame no
			mq3_st mtt;
			mtt.mtype=id + MMUTOPRO;
			mtt.frameno=-1;
			msgsnd( mq3, &mtt, sizeof(mq3_st) - sizeof(long), 0);
			printf("Process %d: TRYING TO ACCESS INVALID PAGE REFERENCE %d\n", id, pageno);
			//get fno = handlePageFault(id, pageno) which is substitute frame received
			int fno = handlePageFault(id, pageno);
			sm1ptr[i*m + pageno].valid = 1;
			sm1ptr[i*m + pageno].frame_no = fno;
			//send message of type PAGEFAULT_HANDLED on mq2
			mq2_t mt;
			mt.mtype=PAGEFAULT_HANDLED;
			msgsnd( mq2, &mt, sizeof(mq2_t) - sizeof(long), 0);
		}
		else
		{
			//send msg on mq3 of type id + MMUTOPRO with foundframeno as frame no
			mq3_st mt;
			mt.mtype=id + MMUTOPRO;
			mt.frameno=sm1ptr[i*m+pageno].frame_no;
			msgsnd( mq3, &mt, sizeof(mq3_st) - sizeof(long), 0);
		}
	}
}

ofstream fout;
int** rk;
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
	
	signal(SIGUSR2, handletgerm);
	//2d array indexed by processno,pageno
	rk=new int*[k];
	for(int i=0;i<k;i++)
		rk[i]=new int[m];
	int** resultkeeper;
	resultkeeper=rk;
	for(int i=0;i<k;i++)
		for(int j=0;j<m;j++)
			rk[i][j]=0; 
	//open "result.txt" for write
	fout.open("result.txt");
	while(!finished)
	{
		serviceMRequest();
	}
	//output all results
	cout<<"\n\nHere are the results, process no on horizontal axis\n\n";
	for(int i=0;i<k;i++)
	{
		cout<<"\n";
		for(int j=0;j<m;j++)
			cout<<"\t"<<rk[i][j]; 
	}
	return 0;
}
