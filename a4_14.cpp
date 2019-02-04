#include<unistd.h>
#include<bits/stdc++.h>
using namespace std;
#define MAXSIZE 5000
void *producer(void* parameters)
{
	srand(gettid());
	int* buffer=(int*)parameters;
	int j=0,randomno;
	for(int i=0;i<1000;i++)
	{
		randomno=rand()%1000;
		for(j=0;j<MAXSIZE;j++)
		{
			if(buf[j]==-1)
				break;
		}
		if (j==MAXSIZE)
		{
			//sleep till empty space available in buf
			//continue
		}
		else
		{
			buf[j]=randomno;
		}
	}
		
	return;	
}

void *consumer(void* parameters)
{
	int* buffer=(int*)parameters;
	srand(gettid());
	int isempty=1;
	while(1)
	{
		isempty=1;		
		for (int i=0;i<MAXSIZE;i++)
		{
			if (buf[i]!=-1)
			{
				isempty=0;
				buf[i]=-1;
				break;
			}	
		}
		if (isempty)
		{
			//sleep till emptyness not present;
		}	
	}
}

int main(int argc,char *argv[])
{
	int N,i;
	int buf[MAXSIZE];
	N=atoi(argv[1]);
	pthread_t thread[N];
	random device rd;
	mt19937 gen(rd());
	binomial_distribution<> d(1,0.5);
	for(i=0;i<MAXSIZE;i++)
		buf[i]=-1;
	
	for (i=0;i<N;i++)
	{
		if(d(gen))
			pthread_create(&(thread[i]),NULL,producer,(void*)buf);
		else
			pthread_create(&(thread[i]),NULL,consumer,(void*)buf);
	}
	return 0;
}
