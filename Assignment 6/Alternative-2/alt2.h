#ifndef ALT2_H
#define ALT2_H

#include <bits/stdc++.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

void init(long long int, long long int, long long int);
int my_open(char *);
int my_read(int, char*, int);
int my_write(int, char*, int);
int my_close(int);
int my_cat(int);
int my_copy(int, int, int);
int my_mkdir(char *);
int my_rmdir(char *);
int my_chdir(char *);

// int cur_dir;

struct super_block
{
	long long int file_sys_size, block_size, n_blocks, n_inodes;
	char name[6];
	int first_free_block;
	bool *free_inode;
};

struct inode
{
	int type;
	int last_data_block;
	vector<int> dp;
	int sip;
	int dip;
};

struct block1
{
	inode *iNode;
};

struct data_block
{
	int next;
	char *data;
};

struct file_sys
{
	super_block sp;
	block1 b_inode;
	data_block *b;
};


struct FD_t
{
	int valid;
	int dir_inode_no;
	int inode_no;

	int indirection_type; // DP, SIP or DIP
	int directoffset;
	int index_sip; // Index for first level table where current block is present
	int index_dip1; // Index for second level table where current block is present
	int index_dip2;

	int windirection_type; // DP, SIP or DIP
	int wdirectoffset;
	int windex_sip; // Index for first level table where current block is present
	int windex_dip1; // Index for second level table where current block is present
	int windex_dip2;

	int current_block;
	int current_offset;
	int current_wblock;
	int current_woffset;
};

#endif
