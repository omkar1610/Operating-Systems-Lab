#include<bits/stdc++.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<fcntl.h> 
#define MAX 100
using namespace std;


int main(){
	int i, j, k, N, flag;
	int status=0;
	int stat=0;
	char comd[MAX];
	char *name;
	pid_t wpid;

	printf("Welcome to SHELL:\n");

	while(1){
		// Taking the input from prompt
		printf(">>> ");
		gets(comd);

		// Comparing if the input is exit or not
		if(!strcmp(comd, "exit")){
			printf("Exiting from SHELL\n");
			break;
		}
		
		// Checking the presence of '&', which denotes background process
		flag = 0;
		for(i=0; 1; i++){
			if(comd[i] == '\0')
				break;
			if(comd[i] == '&')
				flag = 1;
		}

		// Checking if '&' is present at the end or not. If not, then its an error
		char *word;
		word = strtok(comd, "&");
		i = 0;
		while (word != NULL){
			name = word;
			i++;
			word = strtok(NULL, "&");
		}
		if(i>1){
			printf("Not a valid command\n");
			continue;
		}


		// Fork a child and execute the input command
		if((stat=fork()) == 0){
			char *piped[MAX];
			char *pipesep;

			// Tokenizing the input command wrt pipe('|')
			pipesep = strtok(name, "|");
			N = 0;
			while (pipesep != NULL){
				piped[N++] = pipesep;
				pipesep = strtok(NULL, "|");
			}

			// If there are N piped commands, then create N-1 pipes 
			int p[N-1][2];
			for(j = 0; j < N-1; j++)
				if(pipe(p[j]) < 0){
					printf("Pipe Creation failed\n");
					exit(0);
				}
	
			// Now for each of the N commands, do the following
			for(j=0; j<N; j++){
				char *args[MAX];
				char *argn[MAX];
				char *file_in;
				char *file_out;
				char *next_instr;
				int fd_in, fd_out, skip = 0;
			
				// Tokeinizing each command wrt space an tab
				word = strtok (piped[j]," \t");
				i=0;
				while (word != NULL){
					args[i++] = word;
				    word = strtok (NULL, " \t");
				}
				// Append a NULL in the end
				args[i] = NULL;

				// Fork a child process to execute each of the N piped commands
				if((status=fork()) == 0){
					// Duplicate STDIN with the appropriate pipe and close the read and write end accordingly
					if(j>0){
						close(p[j-1][1]);
						dup2(p[j-1][0], 0);
						close(p[j-1][0]);
					}
					// Duplicate STDOUT with the appropriate pipe and close the read and write end accordingly
					if(j != N-1){
						close(p[j][0]);
						dup2(p[j][1], 1);
						close(p[j][1]);
					}

					// Loop to execute input/output redirection
					for(k=0; args[k]!=NULL; k++){
						// Input redirection	
						if(!strcmp(args[k], "<")){
							file_in = args[k+1];
							k++;
							if((fd_in = open(file_in, O_RDONLY)) < 0){
								perror("Couldn't Open File");
								exit(0);
							}
							// Duplicate the file with STDIN
							dup2(fd_in, 0);
							close(fd_in);
						}
						// Output redirection
						else if(!strcmp(args[k], ">")){
							file_out = args[k+1];
							k++;
							if((fd_out = open(file_out, O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU)) < 0){
								perror("Couldn't Open File");
								exit(0);
							} 
							// Duplicate the file with STDOUT
							dup2(fd_out, 1);
							close(fd_out);
						}
						else{
							argn[skip++]=args[k];
						}				
					}	
					argn[skip]=NULL;

					// Execute the command using execvp() call
					execvp(argn[0], argn);
					printf("Not a valid command\n"); 
					exit(0);
					  
				}
				else{
					// Close the already used pipes in the parent process
					for(i=0; i<j; i++){
						close(p[i][0]);
						close(p[i][1]);
					}
				}
			}
			// Wait for all the processes to end
			while((wpid = wait(&status)) > 0);
			exit(0);
		}
		else{
			// If there is no '&', then wait for the process to end
			if(!flag){
				while((wpid = wait(&stat)) > 0);
			}
			usleep(10000);
		}
	}
	return 0;
}