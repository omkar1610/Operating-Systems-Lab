#include<bits/stdc++.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<fcntl.h> 
#define MAX 100
using namespace std;


int main(){
	int i, j, k, p1[2];
	char name[MAX];
	// char cwd[256];

	// getcwd(cwd, sizeof(cwd));

	pipe(p1);
	printf("Welcome to BAsh\n");

	while(1){
		int flag = 0;
		printf(">>> ");
		gets(name);

		if(!strcmp(name, "exit"))
			break;

		if(fork() == 0){
			i = 0;
			char *words[MAX];
			char *args[MAX];
			char *file_in;
			char *file_out;
			int fd_in, fd_out;
			char *word;

			word = strtok (name," \t");
			while (word != NULL){
				words[i++] = word;
			    word = strtok (NULL, " \t");
			}
			words[i] = NULL;

			for(j=0; j<i; j++){
				if(!strcmp(words[j], "<")){
					file_in = words[j+1];
					if((fd_in = open(file_in, O_RDONLY, 0)) < 0){
						perror("Couldn't Open File");
						exit(0);
					}
					int k = j;
					while (words[k+1]) {
	                    words[k] = words[k+2];
	                    k++; 
	                }
					dup2(fd_in, STDIN_FILENO);
				}
				if(!strcmp(words[j], ">")){
					file_out = words[j+1];
					if((fd_out = open(file_out, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0){
						perror("Couldn't Open File");
						exit(0);
					}
					int k = j;
					while (words[k]) {
	                    words[k] = words[k+2];
	                    k++; 
	                }

					dup2(fd_out, STDOUT_FILENO);
				}
				if(!strcmp(words[j], "&")){
					flag = 1;
					close(p1[0]);
					printf("FLAG: %d\n",flag);
					write(p1[1], &flag, sizeof(int));
				}
			}	

			printf("FILE: %s\n", file_out);
			execvp(args[0], args);
			printf("Not a valid command\n");
			kill(getpid(),SIGTERM);
		}
		else{
			close(p1[1]);
			int a = 0;
			read(p1[0], &a, sizeof(int));

			if(!a)
				wait(NULL);
		}
	}
	return 0;
}