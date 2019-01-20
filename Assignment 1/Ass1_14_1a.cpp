#include<bits/stdc++.h>
#include<unistd.h>
using namespace std;

int main(){
	int i;
	pid_t pid;
	int p1[2];
	int p2[2];
	int p3[2];
	int p4[2];

	// Creation of Pipes
	if(pipe(p1) < 0)
		cout << "Pipe creation failed\n";
	if(pipe(p2) < 0)
		cout << "Pipe creation failed\n";
	if(pipe(p3) < 0)
		cout << "Pipe creation failed\n";
	if(pipe(p4) < 0)
		cout << "Pipe creation failed\n";

	pid = fork();		// Forking the child processes
	if(pid < 0)
		cout << "Fork failed\n";

	if(pid == 0){
		pid_t pid1;
		pid1 = fork();
		if(pid1 < 0)
			cout << "Fork failed\n";

		if(pid1 == 0){

			pid_t  pid2;
			pid2 = fork();		// Forking to get process A and B
			if(pid2 < 0)
				cout << "Fork failed\n";

			if(pid2 == 0){			// Process A
				srand(getpid());
				int array[50];
				for(i=0; i<50; i++)
					array[i] = rand()%500;	// Generating random nos. uptil 500

				sort(array, array+50);
				close(p1[0]);
				write(p1[1], &array, sizeof(int)*50);	// Writing to pipe p1
			}
			else{					// Process B
				srand(getpid());
				int array[50];
				for(i=0; i<50; i++)
					array[i] = rand()%500;

				sort(array, array+50);
				close(p2[0]);
				write(p2[1], &array, sizeof(int)*50);	// Writing to pipe p2
			}	
		}
		else{
									// Process C
			srand(getpid());
			int array[50];
			for(i=0; i<50; i++)
				array[i] = rand()%500;

			sort(array, array+50);
			close(p3[0]);
			write(p3[1], &array, sizeof(int)*50);	// Writing to pipe p3
		}
	}
	else{

		pid_t pid3;
		pid3 = fork();
		if(pid3 < 0)
			cout << "Fork failed\n";

		if(pid3 == 0){		// Process D
			int arr1[50];
			int arr2[50];
			int arr[100];

			close(p1[1]);	// Closing the write ends of the pipe
			close(p2[1]);
			
			read(p1[0], &arr1, sizeof(int)*50);		// Reading from the pipes p1, and p2
			read(p2[0], &arr2, sizeof(int)*50);

			for(i=0; i<50; i++){
				arr[i] = arr1[i];
				arr[50+i] = arr2[i];
			}
			sort(arr, arr+100);
			close(p4[0]);
			write(p4[1], &arr, sizeof(int)*100);	// Writing to pipe p4
		}
		else{					// Process E
			int arr1[50];
			int arr2[100];
			int arr[150];

			close(p3[1]);	// Closing the write ends of the pipes
			close(p4[1]);
			
			read(p3[0], &arr1, sizeof(int)*50);		// Reading from the pipes p3 and p4
			read(p4[0], &arr2, sizeof(int)*100);

			for(i=0; i<50; i++)
				arr[i] = arr1[i];
			for(i=50; i<150; i++)
				arr[i] = arr2[i-50];

			sort(arr, arr+150);		// Sorting the array

			cout << "\nSorted Series: \n\n";
			for(i=0; i<150; i++)
				cout << arr[i] << " ";
			cout << endl;
		}		
	}
	return 0;
}