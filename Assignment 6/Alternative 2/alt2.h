#ifndef ALT2_H
#define ALT2_H

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
int my_mkdir(char *);
int my_rmdir(char *);
int my_chdir(char *);

int cur_dir;

struct super_block{
	int file_sys_size, block_size, n_blocks, n_inodes;
	string name;
	int free_block;
	vector<bool> free_inode;
};

struct inode{
	bool free;
	bool type;	// Type false means dir, true means regular
	long int file_size;
	vector<int> dp(5);
	int sip;
	int dip;
};

struct block1{
	inode *iNode;
};

struct data_block{
	int next;
	int type;	// Type 0 = data block, Type 1 = directory content block, Type 2 = SIP, DIP Block
	char *data;
};


struct dir_block{
	char name[14];
	short int inode;
};


struct file_sys{
	super_block sp;
	block1 b_inode;
	data_block *b;
};


struct FD_t
{
	int valid;
	int dir_no;
	int inode_no;
	int pointer_type; // DP, SIP or DIP
	int index_sip; // Index for first level table where current block is present
	int index_dip; // Index for second level table where current block is present
	int current_block;
	int current_offset;
	int current_wblock;
	int current_woffset;
};