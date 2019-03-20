#include<bits/stdc++.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>

using namespace std;

int main(){
	key_t key = ftok("/", 65);
	cout<<key<<endl;


	return 0;
}