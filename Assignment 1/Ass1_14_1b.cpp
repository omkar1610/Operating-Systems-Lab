#include<bits/stdc++.h>
#include<unistd.h>
#define MAX 100
using namespace std;


int main(){
	int i;
	char name[MAX];

	// Taking the name of executable program in a loop until "quit"
	while(1){
		printf("Enter name of executable program: ");
		gets(name);

		if(!strcmp(name, "quit"))		// If "quit", then get out of the loop
			break;

		if(fork() == 0){		// Forking a child process to run the executable program
			i = 0;
			char *args[MAX];
			char *word;

			word = strtok (name," \t");	// Splitting the string to get the name of the program and the command line arguments if any
			while (word != NULL){
				args[i++] = word;
			    word = strtok (NULL, " \t");
			}
			args[i] = NULL;		// Appending the arguments array with NULL

			execvp(args[0], args);		// execvp call to run the program
			kill(getpid(),SIGTERM);		// Kill the child process if not getting executed
		}
		else{
			wait(NULL);		// Waiting the parent process.
		}
	}
	return 0;
}