#include "alt2.h"

using namespace std;


int main()
{
	srand(time(NULL));

	int i;
	long long int file_size, block_size, n_blocks;

	cout<<"Enter file size in MB: ";
	cin>>file_size;
	cout<<"Enter block size in KB: ";
	cin>>block_size;

	// Calculating number of blocks
	n_blocks = (file_size*1024)/block_size;

	// cout<<"Here......"<<endl;
	init(file_size*1024*1024, block_size*1024, n_blocks);

	char filename[100];
	cout<<"Enter filename: ";
	cin>>filename;

	int fd = my_open(filename);

	char msg[100] = "Hello there";
	my_write(fd, msg, 14);
	cout<<"\nContent in File "<<filename<<":"<<endl;
	my_cat(fd);
	cout<<endl<<endl;

	char buf[100];
	int n=my_read(fd, buf, 12);
	buf[n]='\0';
	cout<<buf<<endl<<endl;


	char lol[14];
	strcpy(lol,"abc\0");
	int a = my_mkdir(lol);
	int b = my_chdir(lol);

	int fd1 = my_open(filename);
	char buf1[100];

	char msg1[100] = "Hi there";
	my_write(fd1, msg1, 12);
	int n1=my_read(fd1, buf1, 100);
	buf1[n1]='\0';
	cout<<buf1<<endl;
	cout<<endl;


	int a1 = my_mkdir(lol);
	my_rmdir(lol);
	int b1 = my_chdir(lol);


	int linuxfd = open("temp.txt", O_RDONLY);
	my_copy(fd1,linuxfd,0);
	cout<<"Content in File "<<filename<<" after copying text from temp.txt: "<<endl;
	my_cat(fd1);
	cout<<endl<<endl;

	int fd2 = my_open(filename);
	char ab[100] = "THIS IS SPIDERMAN";
	my_write(fd2, ab, 100);
	int linuxfd1 = open("temp2.txt",O_WRONLY|O_CREAT|O_TRUNC, 0666);
	my_copy(fd2,linuxfd1,1);

	my_close(fd);

	return 0;
}