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


	init(file_size*1024*1024, block_size*1024, n_blocks);

	return 0;
}