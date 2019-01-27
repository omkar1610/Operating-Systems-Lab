#include<bits/stdc++.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<fcntl.h> 
#define MAX 100
using namespace std;


int main(){
	int i, j, k, flag;
	int status=0;
	int stat=0;
	char name[MAX];
	pid_t wpid;

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

		if((stat=fork()) == 0){
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
				char *args[MAX];
				char *argn[MAX];
				int f = 0;
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

				if((status=fork()) == 0){
					if(l>0){
						close(p[l-1][1]);
						dup2(p[l-1][0], 0);
						close(p[l-1][0]);
					}
					if(l != k-1){
						close(p[l][0]);
						dup2(p[l][1], 1);
						close(p[l][1]);
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
							close(fd_in);
						}
						else if(!strcmp(args[j], ">")){
							file_out = args[j+1];
							j++;
							if((fd_out = open(file_out, O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU)) < 0){
								perror("Couldn't Open File");
								exit(0);
							} 
							dup2(fd_out, 1);
							close(fd_out);
						}
						else{
							argn[skip++]=args[j];
						}				
					}	
					argn[skip]=NULL;

					execvp(argn[0], argn);
					printf("Not a valid command\n"); 
					exit(0);
					  
				}
				else{
					for(int w=0; w<l; w++){
						close(p[w][0]);
						close(p[w][1]);
					}
				}
			}
			wait(NULL);
			exit(0);
		}
		else{
			if(!flag){
				while((wpid = wait(&stat)) > 0);
			}
			usleep(10000);
		}
	}
	return 0;
}