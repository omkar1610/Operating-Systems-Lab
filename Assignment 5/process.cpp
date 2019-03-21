#include<bits/stdc++.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>

using namespace std;

void sig_handler(int signo){
	if(signo==SIGUSR1) ;
}

int main(int argc, char *argv[]){
	signal(SIGUSR1, sig_handler);

	pause();
	cout<<"RETURNED\n";

	pid_t pid = atoi(argv[1]);
	kill(pid, SIGUSR1);

	cout<<"PROCESS\n";
	return 0;
}