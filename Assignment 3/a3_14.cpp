#include<bits/stdc++.h>
#include<string>
#include<random>
#include<unistd.h>
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

bool compareCPU(Process a, Process b){
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
		sort(vec.begin(), vec.end(), compareCPU);
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


int psjf(Process p[], int N){
	int i, j, k, tn = 0, t = 0;
	queue <Process> q;
	Process a;
	vector <Process> vec;
	vector <Process>::iterator it;

	i=0;
	j=0;
	while(1){
		vec.clear();
		if(i >= N)
			break;
		for(k=j; k<N; k++){
			if(t >= p[k].arr_time)
				q.push(p[k]);
			else
				break;
		}
		j=k;
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
			if(a.cpu_burst == 1){
				tn += (t-a.arr_time) + 1;
				t++;
				i++;
			}
			else{
				a.cpu_burst -= 1;
				tn += (t-a.arr_time) + 1;
				t++;
				a.arr_time = t;
				q.push(a);
			}
		}
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


bool comparePriority(const pair<Process, int> &a, const pair<Process, int> &b){
	double rRatio1 = ((double)(a.second - a.first.arr_time) + a.first.cpu_burst) / a.first.cpu_burst;
	double rRatio2 = ((double)(b.second - b.first.arr_time) + b.first.cpu_burst) / b.first.cpu_burst;
	return rRatio1>rRatio2;
}
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
		while(!q.empty()){
			vec.push_back(make_pair(q.front(), t));
			q.pop();
		}
		sort(vec.begin(), vec.end(), comparePriority);
		for(it=0; it<vec.size(); it++){
			q.push(vec[it].first);
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


int main(int argc, char *argv[]){
	int N, t = 0, i, tn_fcfs, tn_npsjf, tn_psjf, tn_rr, tn_hrn;
	random_device rdb, rda;
	mt19937 gen1(rdb());
	mt19937 gen2(rda());
  	std::uniform_int_distribution<> unidist(1,20);
  	std::uniform_real_distribution<> unidist_d(0.0,1.0);
	double r;
	N = atoi(argv[1]);

	srand(getpid());
	Process p[N];
	for(i=0; i<N; i++){
		p[i].pid = i;
		p[i].arr_time = t;
		p[i].cpu_burst = unidist(gen1);
		r = unidist_d(gen2);
		t += (int)((-1.0 / MEAN)*log(1-r));
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

	tn_psjf = psjf(p, N);
	cout<<tn_psjf<<endl;

	tn_rr = rr(p, N);
	cout<<tn_rr<<endl;

	tn_hrn = hrn(p, N);
	cout<<tn_hrn<<endl;

	return 0;
}