#include<bits/stdc++.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<fcntl.h> 
#define MAX 100
using namespace std;


int main(){
	int i, j, k, p[MAX][2], flag;
	char name[MAX];
	// char cwd[256];

	// getcwd(cwd, sizeof(cwd));
	for(int w=0;w<MAX;w++)
		pipe(p[w]);
	printf("Welcome to BAsh\n");

	while(1)
	{
		printf(">>> ");
		gets(name);

		if(!strcmp(name, "exit"))
			break;
		
		flag = 0;
		for(i=0; 1; i++)
		{
			if(name[i] == '\0')
				break;
			if(name[i] == '&')
			{
				printf("Yes\n");
				flag = 1;
			}
		}
		char *piped[MAX];
		char *pipesep;
		pipesep = strtok (name,"|");
		k = 0;
		while (pipesep != NULL)
		{
			piped[k++] = pipesep;
			pipesep=strtok(NULL,"|");
		}
		int r=0;
		for(r=0;piped[r]!=NULL;r++)
			cout<<"\n"<<r<<":"<<piped[r];
		cout<<"\n"<<r;
		p[0][0]=STDIN_FILENO;
		p[r][1]=STDOUT_FILENO;
		
		for(int l=0; piped[l]!=NULL;l++)
		{
			
			if(fork() == 0)
			{
				
				char *args[MAX];
				char *argn[MAX];
				
				char *file_in;
				char *file_out;
				char *next_instr;
				int fd_in, fd_out, skip = 0;
				
				char *word;
			

			
				strcpy(name,piped[l]);
				word = strtok (name," \t");
				i=0;
				while (word != NULL)
				{
					args[i++] = word;
				    	word = strtok (NULL, " \t");
				}
				args[i] = NULL;

				for(j=0; args[j]!=NULL; j++)
				{
				
					dup2(p[l][0],STDIN_FILENO);
					dup2(p[l+1][1],STDOUT_FILENO);
					if(!strcmp(args[j], "<"))
					{
						file_in = args[j+1];
						j++;
						if((fd_in = open(file_in, O_RDONLY)) < 0)
						{
							perror("Couldn't Open File");
							exit(0);
						}
						dup2(fd_in, p[l][0]);
					}
					else if(!strcmp(args[j], ">"))
					{
						file_out = args[j+1];
						j++;
						if((fd_out = open(file_out, O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU)) < 0)
						{
							perror("Couldn't Open File");
							exit(0);
						} 
						dup2(fd_out, p[l+1][1]);
					}
					else
					{
						argn[skip++]=args[j];
					}				
				}	
				argn[skip]=NULL;

				execvp(argn[0], argn);
				printf("Not a valid command\n");
				kill(getpid(),SIGTERM);
			}
			else
			{
				// close(p1[1]);
				// int a = 0;
				// read(p1[0], &a, sizeof(int));

				wait(NULL);
				//continue;
			}
		}
	}
	return 0;
}
