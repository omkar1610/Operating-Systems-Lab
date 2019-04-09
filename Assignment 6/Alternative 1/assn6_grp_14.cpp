#include "alt1.h"

using namespace std;


int main()
{
	srand(time(NULL));

	int i;
	int file_size, block_size, n_blocks;

	cout<<"Enter file size in MB: ";
	cin>>file_size;
	cout<<"Enter block size in KB: ";
	cin>>block_size;

	// Calculating number of blocks
	n_blocks = (file_size*1024)/block_size;


	init(file_size, block_size, n_blocks);


	string filename;
	cout<<"Enter filename: ";
	cin>>filename;

	int fd = my_open(filename);
	
	string filename1;
	cout<<"Enter filename: ";
	cin>>filename1;
	int fd1 = my_open(filename1);

	
	char temp[7]="NIKHIL";
	temp[6]='\0';
	my_write(fd, temp, 7);

	char temp1[8] = "SARTHAK";
	temp1[7]='\0';
	my_write(fd1, temp1, 8);
	cout<<"\nContent in File "<<filename1<<":"<<endl;
	my_cat(fd1);
	cout<<endl;

	char readtemp[7];
	my_read(fd, readtemp, 7);
	cout<<"Content in File "<<filename<<":\n"<<readtemp<<endl<<endl;

	char buf[10000];
	my_read(fd, buf, 10000);
	cout<<buf<<endl;


	int linuxfd = open("temp.txt", O_RDONLY);
	my_copy(fd,linuxfd,0);
	cout<<"Content in File "<<filename<<" after copying text from temp.txt: "<<endl;
	my_cat(fd);
	cout<<endl;

	char buf1[10000];
	my_read(fd, buf1, 10000);
	cout<<buf1<<endl;

	int fd2 = my_open(filename1);
	int linuxfd1 = open("temp2.txt",O_WRONLY|O_CREAT|O_TRUNC, 0666);
	my_copy(fd2,linuxfd1,1);


	return 0;
}
