#ifndef ALT_1
#define ALT_1
#include <bits/stdc++.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

void init(int, int, int);
int my_open(string);
int my_read(int, char*, int);
int my_write(int, const char*, size_t);
int my_close(int);
int my_cat(int);
int my_copy(int, int, int);


// Directory is a table of name of file and first data block where it is present
struct directory
{
	string name;
	int index;
};

// Block1 contains a table known as FAT
struct block1
{
	int *fat;
};

// Block2 contains the directory
struct block2
{
	vector<directory> dir;
};

// Data blocks contain characters
struct block
{
	char *data;
};

// Super Block (Contains file size, block size, bit vector and a pointer to the block containing directory)
struct super_block
{
	int file_size, block_size, n_blocks, n_files;
	int *free_block;
	string name;
	block2 *ptr;
};

// File system contains a super block, block1(FAT table), block2(directory table) and other data blocks
struct file_sys
{
	super_block sp;
	block1 b1;
	block2 b2;
	block *b;
};


//There is a FD table local to the program
//Each entry in the file descriptor contains the fp(pointer to where it is inside the file) and it contains a way to reach the directory entry
struct FD_t
{
	int valid;
	int dir_no;
	int my_no;
	int current_block;
	int current_offset;
	int current_wblock;
	int current_woffset;
};


#endif
