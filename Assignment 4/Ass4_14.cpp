#include<unistd.h>
#include<sys/types.h>
#include<bits/stdc++.h>
#include<pthread.h>
#include <sys/syscall.h>
#include<signal.h>
#define gettid() syscall(SYS_gettid)
using namespace std;
#define MAXSIZE 50000

int N;
int buf[MAXSIZE];
int status[50];
int currentWorker;
int full;
bool statusLock;
int aliveproducers, aliveconsumers;
int i;
bool ilock, cwLock,CwLock;



void sig_handler(int signo)
{
	/* If signal is SIGUSR1, then sleep the thread only if it is not dead
	   If signal is SIGUSR2, then wake the thread
	*/
	if(signo == SIGUSR1)
		status[currentWorker]=(status[currentWorker]==2)?2:0;
	if(signo == SIGUSR2)
		status[currentWorker]=1;
	
}



void *producer(void *parameters){
	signal(SIGUSR1, sig_handler);
	signal(SIGUSR2, sig_handler);

	srand(gettid());
	int  j = 0, num;
	int tno=(i-1)%N;
	cout<<"Thread "<<tno<<" is a producer"<<endl;

	ilock=0;
	int nosGenerated=0;

	while(1){
		// If the producer is dead
		if(status[tno]==2){
			aliveproducers--;
			pthread_exit(0);
		}
		// If 1000 numbersare generated, set its status as dead
		if(nosGenerated>=1000){
			while(statusLock==1);
			statusLock=1;
			status[tno]=2;
			statusLock=0;		
		}
		// If thread is sleeping, continue in the loop
		else if (status[tno]==0)
			continue;
		// If the buffer is full
		else if (full==MAXSIZE)
			continue;
		// Else, generate random number and fill in the buffer
		else{
			num = rand()%1000;
			buf[full]=num;
			full++;
			nosGenerated++;
		}
	}
			
}



void *consumer(void *parameters){
	signal(SIGUSR1, sig_handler);
	signal(SIGUSR2, sig_handler);

	srand(gettid());
	int tno=(i-1)%N;
	cout<<"Thread "<<tno<<" is a consumer"<<endl;

	ilock=0;
	while(1){
		// If the buffer is empty and no producers are alive, then set the status of the consumer as dead
		// Else, continue in the loop
		if (full==0){
			if(aliveproducers!=0)
				continue;
			else{
				while(statusLock==1);
				statusLock=1;
				status[tno]=2;
				statusLock=0;
			}
		}
		// If the consumer is dead
		if(status[tno]==2){
			aliveconsumers--;
			pthread_exit(0);
		}
		// If the thread is sleeping, continue in the loop
		if(status[tno]==0)
			continue;
		// Else, remove the element
		else{
			buf[full]=-1;
			full--;
			continue;
		}
	}
}



void *schedule(void *parameters){
	int i, t=0;
	pthread_t *tid = (pthread_t *)parameters;
	currentWorker=0;

	while(1){
		// If no producers or consumers are alive, then exit
		if(aliveproducers==0 && aliveconsumers==0)
			pthread_exit(0);

		// If a current thread is working, no work is to be done
		while(cwLock==1);
		cwLock=1;

		// Sleep all threads initially, unless not dead
		for(int j=0;j<N;j++)
			status[j]=(status[j]==2)?2:0;

		// Scheule the current working thread in round robin fashion
		currentWorker=(currentWorker+1)%N;
		cwLock=0;
		// If current working thread is dead, then continue
		if(status[currentWorker]==2)
			continue;
		// Send a signla to wake the current thread and then sleep after a 1 sec(time quantum)
		else{
			pthread_kill(tid[currentWorker], SIGUSR2);
			sleep(1);
			pthread_kill(tid[currentWorker], SIGUSR1);
		}
		
	}
}



void *report(void *parameters){
	int i;

	while(1){
		cout<<"No. of elements in buffer = "<<full<<endl;

		for(i=0; i<N; i++)
			cout<<"Thread "<<i<<" has status "<<status[i]<<endl;

		cout<<aliveproducers<<" producers are alive"<<endl;
		cout<<aliveconsumers<<" consumers are alive"<<endl;
		cout<<currentWorker<<" is current worker"<<endl;

		// If all producers and consumers are dead, then exit
		if (aliveproducers==0 && aliveconsumers==0)
			pthread_exit(0);

		// If the status of all threads is 2, then all threads are dead
		int sum=0;
		for(i=0; i<N; i++)
			sum+=status[i];
		if(sum==2*N){
			aliveproducers=0;
			aliveconsumers=0;
		}
		sleep(1);
	}
}



int main(int argc, char *argv[]){
	full=0;
	aliveproducers=0;
	aliveconsumers=0;
	pthread_t scheduler, reporter;
	statusLock=0;
	cwLock=0;

	N = atoi(argv[1]);
	pthread_t thread[N];

	// Initializing status of each thread
	for(i=0; i<N; i++)
		status[i] = 0;

	// Initializing buffer
	for(i=0; i<MAXSIZE; i++)
		buf[i] = -1;
	

	// Creates producer and consumer threads with equal probability
	srand(time(0));
	ilock=0;
	for (i=0; i<N; i++){
		if(rand()%2){
			while(ilock==1);
			ilock=1;
			pthread_create(&(thread[i]), NULL, producer, (void*)NULL);
			aliveproducers++;
		}
		else{
			while(ilock==1);
			ilock=1;
			pthread_create(&(thread[i]), NULL, consumer, (void*)NULL);
			aliveconsumers++;
		}
	}
	
	// Creates Scheduler and reporter thread
	pthread_create(&scheduler, NULL, schedule, (void*)thread);
	pthread_create(&reporter, NULL, report, (void*)NULL);

	// Waits for the threads to end
	pthread_join(scheduler, NULL);
	pthread_join(reporter, NULL);
	for(i=0; i<N; i++)
		pthread_join(thread[i], NULL);

	cout<<"\nThis program is done. Successful Termination."<<endl;
	return 0;
}
