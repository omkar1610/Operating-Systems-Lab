#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>

using namespace std;

int main(){
	int k, m, f, i;
	cout<<"Enter no. of processes: ";
	cin>>k;
	cout<<"Enter max no. of pages per process: ";
	cin>>m;
	cout<<"Enter total no. of frames in main memory: ";
	cin>>f;

	key_t key1 = ftok("/", 65);
	int sm1 = shmget(key1, m, IPC_CREAT|0666);

	key_t key2 = ftok("/", 66);
	int sm2 = shmget(key2, f, IPC_CREAT|0666);

	key_t key3 = ftok("/", 67);
	int mq1 = msgget(key3, IPC_CREAT|0666);

	key_t key4 = ftok("/", 68);
	int mq2 = msgget(key4, IPC_CREAT|0666);

	key_t key5 = ftok("/", 69);
	int mq2 = msgget(key5, IPC_CREAT|0666);


	pid_t pid1, pid2;

	if((pid1=fork())==0){
		//create scheduler
	}
	else{
		if((pid2=fork())==0){
			//create MMU
			//pass mq2, mq3, sm1, sm2
			execvp("MMU.cpp")
		}
		else{
			for(i=0; i<k; i++){
				if(fork()==0){
					//passes page reference string, mq1, mq3
					execvp("process.cpp");
				}
				usleep(250000);
			}
		}
		int w, status;
		while((w = wait(&status)) > 0);
		kill(pid1, SIGKILL);
		kill(pid2, SIGKILL);

		exit(0);
	}

	return 0;
}
