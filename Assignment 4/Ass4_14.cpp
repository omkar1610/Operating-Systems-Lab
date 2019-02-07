#include<unistd.h>
#include<sys/types.h>
#include<bits/stdc++.h>
#include<pthread.h>
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)
using namespace std;
#define MAXSIZE 5000

int N;
int buf[MAXSIZE];
int status[50];


void sig_handler(int signo){
	if(signo == SIGUSR1)
		pause();
}


void *producer(void *parameters){
	signal(SIGUSR1, sig_handler);
	signal(SIGUSR2, sig_handler);
	srand(gettid());
	int  j = 0, num;

	for(int i=0; i<1000; i++){
		num = rand()%1000;
		for(j=0; j<MAXSIZE; j++){
			if(buf[j] == -1)
				break;
		}
		while(j == MAXSIZE);
		buf[j] = num;
	}
	pthread_exit(0);		
}


void *consumer(void *parameters){
	signal(SIGUSR1, sig_handler);
	signal(SIGUSR2, sig_handler);
	srand(gettid());
	int isempty = 1;

	while(1){
		isempty = 1;		
		for (int i=0; i<MAXSIZE; i++){
			if (buf[i] != -1){
				isempty = 0;
				buf[i] = -1;
				break;
			}	
		}
		while(isempty);
	}
	pthread_exit(0);
}


void *schedule(void *parameters){
	int i, t=0;
	pthread_t *tid = (pthread_t *)parameters;
	
	while(1){
		for(i=0; i<N; i++){
			if(t != i){
				pthread_kill(tid[i], SIGUSR1);
				status[i] = 0;
			}
		}
		pthread_kill(tid[t], SIGUSR2);
		status[t] = 1;

		sleep(1);
		t = (t+1)%N;
	}
}


void *report(void *parameters){
	int i, count;

	while(1){
		count = 0;
		for(i=0; i<MAXSIZE; i++){
			if(buf[i] != -1)
				count++;
		}
		cout<<"No. of elements in buffer = "<<count<<endl;

		for(i=0; i<N; i++){
			if(status[i] == 1)
				cout<<"Thread "<<i<<" is running"<<endl;
		}
		sleep(1);
	}

}


int main(int argc, char *argv[]){
	int i;
	N = atoi(argv[1]);
	pthread_t thread[N];
	pthread_t scheduler, reporter;

	for(i=0; i<N; i++)
		status[i] = 0;

	for(i=0; i<MAXSIZE; i++)
		buf[i] = -1;
	
	
	srand(time(0));
	for (i=0; i<N; i++){
		if(rand()%2)
			pthread_create(&(thread[i]), NULL, producer, (void*)NULL);
		else
			pthread_create(&(thread[i]), NULL, consumer, (void*)NULL);
	}

	pthread_create(&scheduler, NULL, schedule, (void*)thread);
	pthread_create(&reporter, NULL, report, (void*)NULL);

	pthread_join(scheduler, NULL);
	pthread_join(reporter, NULL);
	for(i=0; i<N; i++)
		pthread_join(thread[i], NULL);

	return 0;
}
