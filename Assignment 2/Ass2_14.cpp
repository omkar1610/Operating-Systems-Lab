#include<bits/stdc++.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<fcntl.h> 
#define MAX 100
using namespace std;


int main(){
	int i, j, k, p1[2], flag;
	char name[MAX];
	// char cwd[256];

	// getcwd(cwd, sizeof(cwd));

	pipe(p1);
	printf("Welcome to BAsh\n");

	while(1){
		printf(">>> ");
		gets(name);

		if(!strcmp(name, "exit"))
			break;
		
		flag = 0;
		for(i=0; 1; i++){
			if(name[i] == '\0')
				break;
			if(name[i] == '&'){
				printf("Yes\n");
				flag = 1;
			}
		}

		if(fork() == 0){
			i = 0;
			char *args[MAX];
			char *argn[MAX];
			char *file_in;
			char *file_out;
			char *next_instr;
			int fd_in, fd_out, skip = 0;
			char *word;

			word = strtok (name," \t");
			while (word != NULL){
				args[i++] = word;
			    word = strtok (NULL, " \t");
			}
			args[i] = NULL;

			for(j=0; args[j]!=NULL; j++){
				if(!strcmp(args[j], "<")){
					file_in = args[j+1];
					j++;
					if((fd_in = open(file_in, O_RDONLY)) < 0){
						perror("Couldn't Open File");
						exit(0);
					}
					dup2(fd_in, STDIN_FILENO);
				}
				else if(!strcmp(args[j], ">")){
					file_out = args[j+1];
					j++;
					if((fd_out = open(file_out, O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU)) < 0){
						perror("Couldn't Open File");
						exit(0);
					} 
					dup2(fd_out, STDOUT_FILENO);
				}
				else if(!strcmp(args[j], "&")){
					// flag = 1;
					// close(p1[0]);
					// printf("FLAG: %d\n",flag);
					// write(p1[1], &flag, sizeof(int));
				}
				else if(!strcmp(args[j], "|")){
					next_instr = 
				}
				else{
					argn[skip++]=args[j];
				}				
			}	
			argn[skip]=NULL;

			execvp(argn[0], argn);
			printf("Not a valid command\n");
			kill(getpid(),SIGTERM);
		}
		else{
			// close(p1[1]);
			// int a = 0;
			// read(p1[0], &a, sizeof(int));

			if(flag == 0)
				wait(NULL);
			else
				usleep(10);
		}
	}
	return 0;
}