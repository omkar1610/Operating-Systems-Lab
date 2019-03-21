#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <time.h>

using namespace std;

struct mesg_buffer{ 
    long mesg_type; 
    pid_t pid; 
} message;

int main(){
	int k, m, f, i, j;
	pid_t pid1, pid2;

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
	int mq3 = msgget(key5, IPC_CREAT|0666);

	
	
	if((pid1=fork()) == 0){
		//creates scheduler
		//pass mq1, mq2

		char *c1 = new char[to_string(key3).length() + 1];	// MQ1
		strcpy(c1, to_string(key3).c_str());
		char *c2 = new char[to_string(key4).length() + 1];	// MQ2
		strcpy(c2, to_string(key4).c_str());

		char *argv[] = {"./sched", c1, c2, (char *)NULL};
		execvp(argv[0], argv);
		exit(0);
	}
	else{
		if((pid1=fork()) == 0){
			//creates MMU
			//pass mq2, mq3, sm1, sm2

			char *c1 = new char[to_string(key4).length() + 1];	// MQ2
			strcpy(c1, to_string(key4).c_str());
			char *c2 = new char[to_string(key5).length() + 1];	// MQ3
			strcpy(c2, to_string(key5).c_str());
			char *c3 = new char[to_string(key1).length() + 1];	// SM1
			strcpy(c3, to_string(key1).c_str());
			char *c4 = new char[to_string(key2).length() + 1];	// SM2
			strcpy(c4, to_string(key2).c_str());

			char *argv[] = {"./MMU", c1, c2, c3, c4, (char *)NULL};
			execvp(argv[0], argv);
			exit(0);
		}
		else{
			if(fork()==0){
				for(i=0; i<k; i++){
					if(fork()==0){
						srand(getpid());

						int mi = rand() % m + 1;
						int x = rand() % (10*mi - 2*mi) + 2*mi;
						int page_ref[x];

						for(j=0; j<x; j++)
							page_ref[j] = rand()%mi;

						for(j=0; j<x; j++)
							cout<<page_ref[j]<<" ";
						cout<<endl;

						pid_t pid = getpid();
						message.mesg_type = 1;
						message.pid = pid;
						cout<<"MASTER: "<<pid<<endl;
						msgsnd(mq1, &message, sizeof(message), 0);

						// exit(0);
						//passes page reference string, mq1, mq3

						char *c1 = new char[to_string(pid1).length() + 1];	// PID of MMU
						strcpy(c1, to_string(pid1).c_str());
						char *c2 = new char[to_string(key3).length() + 1];	// MQ1
						strcpy(c2, to_string(key3).c_str());
						char *c3 = new char[to_string(key5).length() + 1];	// MQ3
						strcpy(c3, to_string(key5).c_str());

						char *argv[] = {"./process", c1, c2, c3, (char *)NULL};
						execvp(argv[0], argv);
					}
					cout<<endl;
					usleep(250000);
				}
				exit(0);
			}
			else{
				int w, status;
				while((w = wait(&status)) > 0);
				exit(0);
			}
		}
	}

	return 0;
}
