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
	cout<<"Here......"<<endl;
	char filename[100];
	cout<<"Enter filename: ";
	cin>>filename;

	int fd = my_open(filename);
	cout<<"fd = "<<fd<<endl;

	char msg[100] = "Hello there";
	my_write(fd, msg, 14);
	// cout<<"DONE"<<endl;

	char buf[100];
	int n=my_read(fd, buf, 12);
	buf[n]='\0';
	cout<<buf<<endl;


	// int fd1 = my_open(filename);
	// char buf1[100];
	// my_read(fd1, buf1, 100);
	// cout<<buf<<endl;
	char lol[14];
	strcpy(lol,"abc\0");
	int a = my_mkdir(lol);
	int b = my_chdir(lol);

	int fd1 = my_open(filename);
	char buf1[100];

	char msg1[100] = "Hello there";
	my_write(fd1, msg1, 14);
	int n1=my_read(fd, buf1, 100);
	buf1[n1]='\0';
	cout<<buf<<endl;


	return 0;
}