#include<bits/stdc++.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>

using namespace std;

struct mesg_buffer{ 
    long mesg_type; 
    pid_t pid; 
}message;

int main(int argc, char *argv[]){
	key_t key1, key2;
	int i;

	key1 = atoi(argv[1]);
	key2 = atoi(argv[2]);

	int mq1 = msgget(key1, IPC_CREAT|0666);
	int mq2 = msgget(key2, IPC_CREAT|0666);


	while(msgrcv(mq1, &message, sizeof(message), 1, 0) != -1){
		cout<<"SCHED: "<<message.pid<<endl;
		kill(message.pid, SIGUSR1);
		cout<<"signal sent"<<endl;

		char str[100];
		msgrcv(mq2, str, sizeof(str), 0, 0);
		cout<<"done\n";
		if(!strcmp("TERMINATED", str)){
			continue;
		}
		else if(!strcmp("PAGE FAULT HANDLED", str)){
			msgsnd(mq1, &message, sizeof(message), 0);
		}
	}


	return 0;
}