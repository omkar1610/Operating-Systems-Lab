#include<unistd.h>
#include<sys/types.h>
#include<bits/stdc++.h>
#include<pthread.h>
#include <sys/syscall.h>
#include<signal.h>
#define gettid() syscall(SYS_gettid)
using namespace std;
#define MAXSIZE 500000

int N;
int buf[MAXSIZE];
int status[50];
int currentWorker;
int full,empty;
int processNo;
bool processNoLock, statusLock;
int aliveproducers, aliveconsumers;



void sig_handler(int signo)
{
	if(signo == SIGUSR1)
	{
		while (statusLock==1);
		statusLock=1;
		status[currentWorker]=(status[currentWorker]==2)?2:0;
		statusLock=0;
		
	}
	if(signo == SIGUSR2)
	{
		int i;
		while (statusLock==1);
		statusLock=1;
		for(i=0;i<processNo;i++)
			status[i]=(status[i]==2)?2:0;
		status[currentWorker]=1;
		statusLock=0;
	}
	
}


void *producer(void *parameters)
{
	signal(SIGUSR1, sig_handler);
	signal(SIGUSR2, sig_handler);
	srand(gettid());
	int  j = 0, num;
	while (processNoLock==1);
	processNoLock=1;
	int tno=processNo;
	cout<<"Thread "<<processNo<<" is a producer"<<endl;
	processNo++;
	processNoLock=0;
	int nosGenerated=0;
	while(1)
	{
		if(nosGenerated==1000)
		{
			aliveproducers--;
			while (statusLock==1);
			statusLock=1;
			status[tno]=2;
			statusLock=0;
			//cout<<"..Thread "<<tno<<" is a producer and done"<<endl;
			pthread_exit(0);		
		}
		else if (status[tno]==0)
			continue;
		else if (empty==0)
			continue;
		else
		{
			num = rand()%1000;
			buf[full]=num;
			full++;
			empty--;
			nosGenerated++;
		}
	}
			
}


void *consumer(void *parameters)
{
	signal(SIGUSR1, sig_handler);
	signal(SIGUSR2, sig_handler);
	srand(gettid());
	//int isempty = 1;
	while (processNoLock==1);
	processNoLock=1;
	int tno=processNo;
	cout<<"Thread "<<processNo<<" is a consumer"<<endl;
	processNo++;
	processNoLock=0;
	while(1)
	{
		if (full==0 && aliveproducers==0)
		{
			while (statusLock==1);
			statusLock=1;
			status[tno]=2;
			statusLock=0;
			aliveconsumers--;
			//cout<<"...Thread "<<tno<<" is a consumer and done"<<endl;
			pthread_exit(0);
		
		}
		
		
		if(status[tno]==0)
			continue;
		else if (full==0)
		{
			if(aliveproducers!=0)
				continue;
			else
			{
				while (statusLock==1);
				statusLock=1;
				status[tno]=2;
				statusLock=0;
				aliveconsumers--;
				//cout<<"...Thread "<<tno<<" is a consumer and done"<<endl;
				pthread_exit(0);
			}
		}
		else
		{
			buf[full]=-1;
			full--;
			empty++;
			continue;
		}
	}
}


void *schedule(void *parameters)
{
	int i, t=0;
	pthread_t *tid = (pthread_t *)parameters;
	currentWorker=0;
	while(1)
	{
		/*for(i=0; i<N; i++)
		{
			if(t != i)
			{
				pthread_kill(tid[i], SIGUSR1);
				status[i] = 0;
			}
		}
		pthread_kill(tid[t], SIGUSR2);
		status[t] = 1;

		sleep(1);
		t = (t+1)%N;*/
		if(aliveproducers==0 && aliveconsumers==0)
			pthread_exit(0);
		currentWorker=(currentWorker+1)%processNo;
		if(status[currentWorker]==2)
			continue;
		else 
		{
			pthread_kill(tid[currentWorker], SIGUSR2);
			sleep(1);
			pthread_kill(tid[currentWorker], SIGUSR1);
		}
		
	}
}


void *report(void *parameters)
{
	int i;

	while(1)
	{
		
		cout<<"No. of elements in buffer = "<<full<<endl;

		for(i=0; i<N; i++)
		{
			//if(status[i] == 1)
				cout<<"Thread "<<i<<" is running with status "<<status[i]<<endl;
		}
		cout<<aliveproducers<<" producers are alive"<<endl;
		cout<<aliveconsumers<<" consumers are alive"<<endl;
		if (aliveproducers==0 && aliveconsumers==0)
			pthread_exit(0);
		
		sleep(1);
	}

}


int main(int argc, char *argv[])
{
	int i;
	empty=MAXSIZE;
	full=0;
	processNo=0;
	aliveproducers=0;
	aliveconsumers=0;
	N = atoi(argv[1]);
	pthread_t thread[N];
	pthread_t scheduler, reporter;
	processNoLock=0;
	statusLock=0;
	for(i=0; i<N; i++)
		status[i] = 0;

	for(i=0; i<MAXSIZE; i++)
		buf[i] = -1;
	
	
	srand(time(0));
	for (i=0; i<N; i++)
	{
		if(rand()%2)
		{
			pthread_create(&(thread[i]), NULL, producer, (void*)NULL);
			aliveproducers++;
			//cout<<"Thread "<<processNo<<" is a producer"<<endl;
		}
		else
		{
			pthread_create(&(thread[i]), NULL, consumer, (void*)NULL);
			aliveconsumers++;
			//cout<<"Thread "<<processNo<<" is a consumer"<<endl;
		}
	}
	
	pthread_create(&scheduler, NULL, schedule, (void*)thread);
	pthread_create(&reporter, NULL, report, (void*)NULL);

	pthread_join(scheduler, NULL);
	pthread_join(reporter, NULL);
	for(i=0; i<N; i++)
		pthread_join(thread[i], NULL);
	cout<<"\n This program is done"<<endl;
	return 0;
}




// #include<unistd.h>
// #include<sys/types.h>
// #include<bits/stdc++.h>
// #include<pthread.h>
// #include <sys/syscall.h>
// #define gettid() syscall(SYS_gettid)
// using namespace std;
// #define MAXSIZE 5000

// int N;
// int buf[MAXSIZE];
// int status[50];


// void sig_handler(int signo){
// 	if(signo == SIGUSR1)
// 		pause();
// }


// void *producer(void *parameters){
// 	signal(SIGUSR1, sig_handler);
// 	signal(SIGUSR2, sig_handler);
// 	srand(gettid());
// 	int  j = 0, num;

// 	for(int i=0; i<1000; i++){
// 		num = rand()%1000;
// 		for(j=0; j<MAXSIZE; j++){
// 			if(buf[j] == -1)
// 				break;
// 		}
// 		while(j == MAXSIZE);
// 		buf[j] = num;
// 	}
// 	pthread_exit(0);		
// }


// void *consumer(void *parameters){
// 	signal(SIGUSR1, sig_handler);
// 	signal(SIGUSR2, sig_handler);
// 	srand(gettid());
// 	int isempty = 1;

// 	while(1){
// 		isempty = 1;		
// 		for (int i=0; i<MAXSIZE; i++){
// 			if (buf[i] != -1){
// 				isempty = 0;
// 				buf[i] = -1;
// 				break;
// 			}	
// 		}
// 		while(isempty);
// 	}
// 	pthread_exit(0);
// }


// void *schedule(void *parameters){
// 	int i, t=0;
// 	pthread_t *tid = (pthread_t *)parameters;
	
// 	while(1){
// 		for(i=0; i<N; i++){
// 			if(t != i){
// 				pthread_kill(tid[i], SIGUSR1);
// 				status[i] = 0;
// 			}
// 		}
// 		pthread_kill(tid[t], SIGUSR2);
// 		status[t] = 1;

// 		sleep(1);
// 		t = (t+1)%N;
// 	}
// }


// void *report(void *parameters){
// 	int i, count;

// 	while(1){
// 		count = 0;
// 		for(i=0; i<MAXSIZE; i++){
// 			if(buf[i] != -1)
// 				count++;
// 		}
// 		cout<<"No. of elements in buffer = "<<count<<endl;

// 		for(i=0; i<N; i++){
// 			if(status[i] == 1)
// 				cout<<"Thread "<<i<<" is running"<<endl;
// 		}
// 		sleep(1);
// 	}

// }


// int main(int argc, char *argv[]){
// 	int i;
// 	N = atoi(argv[1]);
// 	pthread_t thread[N];
// 	pthread_t scheduler, reporter;

// 	for(i=0; i<N; i++)
// 		status[i] = 0;

// 	for(i=0; i<MAXSIZE; i++)
// 		buf[i] = -1;
	
	
// 	srand(time(0));
// 	for (i=0; i<N; i++){
// 		if(rand()%2)
// 			pthread_create(&(thread[i]), NULL, producer, (void*)NULL);
// 		else
// 			pthread_create(&(thread[i]), NULL, consumer, (void*)NULL);
// 	}

// 	pthread_create(&scheduler, NULL, schedule, (void*)thread);
// 	pthread_create(&reporter, NULL, report, (void*)NULL);

// 	pthread_join(scheduler, NULL);
// 	pthread_join(reporter, NULL);
// 	for(i=0; i<N; i++)
// 		pthread_join(thread[i], NULL);

// 	return 0;
// }
