#include<bits/stdc++.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<fcntl.h> 
#define MAX 100
using namespace std;


int main(){
	int i, j, k, flag;
	int *status;
	char name[MAX];

	printf("Welcome to SHELL:\n");

	while(1){
		printf(">>> ");
		gets(name);

		if(!strcmp(name, "exit")){
			printf("Exiting from SHELL\n");
			break;
		}
		
		flag = 0;
		for(i=0; 1; i++){
			if(name[i] == '\0')
				break;
			if(name[i] == '&'){
				flag = 1;
			}
		}

		if(fork() == 0){
			char *piped[MAX];
			char *pipesep;
			pipesep = strtok(name, "|");
			k = 0;
			while (pipesep != NULL){
				piped[k++] = pipesep;
				pipesep = strtok(NULL, "|");
			}

			int p[k-1][2];
			for(int w=0; w < k-1; w++)
				pipe(p[w]);

			
			for(int l=0; l<k; l++){

				if(fork() == 0){
					char *args[MAX];
					char *argn[MAX];
					
					char *file_in;
					char *file_out;
					char *next_instr;
					int fd_in, fd_out, skip = 0;
					char *word;
				
					word = strtok (piped[l]," \t");
					i=0;
					while (word != NULL){
						args[i++] = word;
					    word = strtok (NULL, " \t");
					}
					args[i] = NULL;
					
					if(l>0){
						close(p[l-1][1]);
						dup2(p[l-1][0], 0);
					}
					if(l != k-1){
						close(p[l][0]);
						dup2(p[l][1], 1);
					}

					for(j=0; args[j]!=NULL; j++){	
						if(!strcmp(args[j], "<")){
							file_in = args[j+1];
							j++;
							if((fd_in = open(file_in, O_RDONLY)) < 0){
								perror("Couldn't Open File");
								exit(0);
							}
							dup2(fd_in, 0);
						}
						else if(!strcmp(args[j], ">")){
							file_out = args[j+1];
							j++;
							if((fd_out = open(file_out, O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU)) < 0){
								perror("Couldn't Open File");
								exit(0);
							} 
							dup2(fd_out, 1);
						}
						else{
							argn[skip++]=args[j];
						}				
					}	
					argn[skip]=NULL;

					execvp(argn[0], argn);
					printf("Not a valid command\n");   
				}
				else{
					usleep(1000);
				}
			}
			exit(1);
		}
		else{
			if(!flag){
				wait(NULL);
			}
		}
	}
	return 0;
}