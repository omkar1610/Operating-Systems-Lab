#include<bits/stdc++.h>
#include<string>
#include<random>
#include<unistd.h>
#define MEAN 1.0
using namespace std;

// Structure defined for a Process, which contains Process no., arrival time and CPU burst
typedef struct{
	int pid;
	int arr_time;
	int cpu_burst;
}Process;


// First Come First Serve
int fcfs(Process p[], int N){
	int i, j, k, tn = 0, t = 0;
	Process a;
	queue <Process> q;

	i=0;
	j=0;
	// i denotes the number of processes completed
	while(i < N){
		// Push the processes that has arrived uptil now in Job Queue
		for(k=j; k<N; k++){
			if(t >= p[k].arr_time)
				q.push(p[k]);
			else
				break;
		}
		j=k;
		// If the queue in not empty, them take the first process and change the time and turnaround time accordingly
		if(!q.empty()){
			a = q.front();
			q.pop();
			i++;
			tn += (t-a.arr_time) + a.cpu_burst; 
			t += a.cpu_burst;
		}
		// If the queue is empty but all the processes have not been completed, increase time
		else if(i < N){
			t++;
			continue;
		}
		else
			break;
	}
	// Return Average Turnaround Time
	return tn/N;
}


// Function to compare the CPU burst of two processes
bool compareCPU(Process a, Process b){
	return a.cpu_burst<b.cpu_burst;
}
// Non Preemptive Shortest Job First
int npsjf(Process p[], int N){
	queue <Process> q;
	vector <Process> vec;
	vector <Process>::iterator it;
	Process a;
	int i, j, k, tn = 0, t = 0;

	i=0;
	j=0;
	// i denotes the number of processes that has been completed till now
	while(i < N){
		// Clear the vector
		vec.clear();
		// Push the processes in the job queue
		for(k=j; k<N; k++){
			if(t >= p[k].arr_time){
				q.push(p[k]);
			}
			else
				break;
		}
		j=k;
		// Put all the jobs in the queue in vector, sort it and put it back in the queue
		while(!q.empty()){
			vec.push_back(q.front());
			q.pop();
		}
		sort(vec.begin(), vec.end(), compareCPU);
		for(it=vec.begin(); it<vec.end(); it++){
			q.push(*it);
		}
		// If queue is not empty, take the first job in the queue (now the shortest one) and complete the process changing the turnaround time accordingly
		if(!q.empty()){
			a = q.front();
			q.pop();
			i++;
			tn += (t-a.arr_time) + a.cpu_burst; 
			t += a.cpu_burst;
		}
		// If the queue is empty, and all the processes has not been completed, then increase the time by 1
		else if(i < N){
			t++;
			continue;
		}
		else
			break;
	}
	// Return average time burst
	return tn/N;
}


// Preemptive Shortest Job First
int psjf(Process p[], int N){
	int i, j, k, tn = 0, t = 0;
	queue <Process> q;
	Process a;
	vector <Process> vec;
	vector <Process>::iterator it;

	i=0;
	j=0;
	// while time increases
	while(1){
		vec.clear();
		// Break if all the processes has been completed
		if(i >= N)
			break;
		// Push the processes in the job queue
		for(k=j; k<N; k++){
			if(t >= p[k].arr_time)
				q.push(p[k]);
			else
				break;
		}
		j=k;
		// Sort the processes by their CPU burst
		while(!q.empty()){
			vec.push_back(q.front());
			q.pop();
		}
		sort(vec.begin(), vec.end(), compareCPU);
		for(it=vec.begin(); it<vec.end(); it++){
			q.push(*it);
		}
		if(!q.empty()){
			a = q.front();
			q.pop();
			// If the CPU burst left is 1, then increase i by 1 and turnaorund time accordingly
			if(a.cpu_burst == 1){
				tn += (t-a.arr_time) + 1;
				t++;
				i++;
			}
			// If the CPU burst left is more than 1, then decrease the burst by 1, and push the process again in Queue
			else{
				a.cpu_burst -= 1;
				tn += (t-a.arr_time) + 1;
				t++;
				a.arr_time = t;
				q.push(a);
			}
		}
	}
	return tn/N;
}


// Round Robin
int rr(Process p[], int N){
	queue <Process> q;
	Process a;
	int i, j, k, tn = 0, t = 0, delta = 2; // Time quantum = 2

	i=0;
	j=0;
	// While all the process has not been completed
	while(i < N){
		for(k=j; k<N; k++){
			if(t >= p[k].arr_time)
				q.push(p[k]);
			else
				break;
		}
		j=k;
		if(!q.empty()){
			a = q.front();
			q.pop();
			// If busrt > time quantum, then decrease the burst by delta and push the process in Queue after pushiing all the processes that has come in between its start and forced stop
			if(a.cpu_burst>delta){
				a.cpu_burst -= delta;
				tn += (t-a.arr_time) + delta;
				t += delta;
				a.arr_time = t;
				for(k=j; k<N; k++){
					if(t >= p[k].arr_time)
						q.push(p[k]);
					else
						break;
				}
				j=k;
				q.push(a);
			}
			// If burst <= delta, then 1 process is completed
			else{
				i++;
				tn += (t-a.arr_time) + a.cpu_burst;
				t += a.cpu_burst;
			}
		}
		else if(i < N){
			t++;
			continue;
		}
		else
			break;
	}
	return tn/N;
}



// Compares the Respose Ratio of two processes
bool comparePriority(const pair<Process, int> &a, const pair<Process, int> &b){
	double rRatio1 = ((double)(a.second - a.first.arr_time) + a.first.cpu_burst) / a.first.cpu_burst;
	double rRatio2 = ((double)(b.second - b.first.arr_time) + b.first.cpu_burst) / b.first.cpu_burst;
	return rRatio1>rRatio2;
}
// Highest Response Ratio Next
int hrn(Process p[], int N){
	queue <Process> q;
	vector < pair<Process, int> > vec;
	Process a;
	int i, j, k, tn = 0, t = 0, it;

	i=0;
	j=0;
	while(i < N){
		vec.clear();
		for(k=j; k<N; k++){
			if(t >= p[k].arr_time){
				q.push(p[k]);
			}
			else
				break;
		}
		j=k;
		// Sort all the jobs according to their priority
		while(!q.empty()){
			vec.push_back(make_pair(q.front(), t));
			q.pop();
		}
		sort(vec.begin(), vec.end(), comparePriority);
		for(it=0; it<vec.size(); it++){
			q.push(vec[it].first);
		}
		// If the queue is not empty, take the first job (highest priority) and complete it
		if(!q.empty()){
			a = q.front();
			q.pop();
			i++;
			tn += (t-a.arr_time) + a.cpu_burst; 
			t += a.cpu_burst;
		}
		else if(i < N){
			t++;
			continue;
		}
		else
			break;
	}
	return tn/N;
}


int main(int argc, char *argv[]){
	int N, t = 0, i, tn_fcfs, tn_npsjf, tn_psjf, tn_rr, tn_hrn;
	random_device rdb, rda;
	mt19937 gen1(rdb());
	mt19937 gen2(rda());
  	std::uniform_int_distribution<> unidist(1,20);	// Generates an integer uniformly between 1 and 20
  	std::uniform_real_distribution<> unidist_d(0.0,1.0);	// Generates a real number uniformly between 1 and 1
	double r;
	// Taking input from the command line argument
	N = atoi(argv[1]);

	srand(getpid()); // Seed the random numbbers with pid
	Process p[N];
	// Generates the arrival time and CPU burst for the N processes
	for(i=0; i<N; i++){
		p[i].pid = i;
		p[i].arr_time = t;
		p[i].cpu_burst = unidist(gen1);
		r = unidist_d(gen2);
		t += (int)((-1.0 / MEAN)*log(1-r));
	}

	// Saves the data in a file
	string file = "Process_"+to_string(N)+"_"+to_string(getpid())+".txt";
	ofstream f (file);
	if(f.is_open()){
		f<<"PROCESS ID\tARRIVAL TIME\tCPU BURST"<<endl;
		for(i=0; i<N; i++){
			f<<p[i].pid<<"\t\t"<<p[i].arr_time<<"\t\t"<<p[i].cpu_burst<<endl;
		}
		f.close();
	}
	
	tn_fcfs = fcfs(p, N);
	cout<<tn_fcfs<<endl;

	tn_npsjf = npsjf(p, N);
	cout<<tn_npsjf<<endl;

	tn_psjf = psjf(p, N);
	cout<<tn_psjf<<endl;

	tn_rr = rr(p, N);
	cout<<tn_rr<<endl;

	tn_hrn = hrn(p, N);
	cout<<tn_hrn<<endl;

	return 0;
}