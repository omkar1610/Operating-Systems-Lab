#include<bits/stdc++.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/msg.h>
#include<errno.h>

using namespace std;


struct mq1_t{
	long mtype;
	int id;
};


int main(int argc, char *argv[]){
	
	shmctl(2195473, IPC_RMID, NULL);
	shmctl(2228242, IPC_RMID, NULL);
	shmctl(2261011, IPC_RMID, NULL);
	shmctl(3309588, IPC_RMID, NULL);
	shmctl(2588693, IPC_RMID, NULL);
	shmctl(2621462, IPC_RMID, NULL);
	shmctl(4489239, IPC_RMID, NULL);
	shmctl(3342360, IPC_RMID, NULL);
	shmctl(3375129, IPC_RMID, NULL);
	shmctl(4522011, IPC_RMID, NULL);
	shmctl(4554780, IPC_RMID, NULL);

	return 0;

}