#include<bits/stdc++.h>
#include<string>
#define MEAN 1.0
using namespace std;

typedef struct{
	int pid;
	int arr_time;
	int cpu_burst;
}Process;


int fcfs(Process p[], int N){
	int i, j, k, tn = 0, t = 0;
	Process a;
	queue <Process> q;

	i=0;
	j=0;
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
	return tn;
}

bool compare(Process a, Process b){
	return a.cpu_burst<b.cpu_burst;
}
int npsjf(Process p[], int N){
	queue <Process> q;
	vector <Process> vec;
	vector <Process>::iterator it;
	Process a;
	int i, j, k, tn = 0, t = 0;

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
		while(!q.empty()){
			vec.push_back(q.front());
			q.pop();
		}
		sort(vec.begin(), vec.end(), compare);
		for(it=vec.begin(); it<vec.end(); it++){
			q.push(*it);
		}
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
	return tn;
}


int rr(Process p[], int N){
	queue <Process> q;
	Process a;
	int i, j, k, tn = 0, t = 0, delta = 2;

	i=0;
	j=0;
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
	return tn;
}


int main(int argc, char *argv[]){
	int N, t = 0, i, tn_fcfs, tn_npsjf, tn_psjf, tn_rr, tn_hrn;
	double r;
	N = atoi(argv[1]);

	srand(time(0));
	Process p[N];
	for(i=0; i<N; i++){
		p[i].pid = i;
		p[i].arr_time = t;
		p[i].cpu_burst = 1 + rand()%20;
		r = ((double)rand() / RAND_MAX);
		t += (int)((-1.0 / MEAN)*log(1-r)) % 10;
	}

	ofstream f ("process.txt");
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

	tn_rr = rr(p, N);
	cout<<tn_rr<<endl;

	return 0;
}