#include "alt2.h"

using namespace std;


// Declerations
int file_size, block_size, n_blocks;
file_sys file;
vector<FD_t> FD;


/////////////////////////////////////////////////////////////////////////////////////////
void init(int a, int b, int c){
	::file_size = a;
	::block_size = b;
	::n_blocks = c;

	int i;

	file.sp.file_size = ::file_size;
	file.sp.block_size = ::block_size;
	file.sp.n_blocks = ::n_blocks;
	file.sp.n_inodes = 2 * (::block_size/sizeof(inode));
	file.sp.name = "INODE";
	file.sp.free_block = 3;

	file.b_inode.iNode = (inode *)malloc(file.sp.n_inodes * sizeof(inode));

	for(i=0; i<file.sp.n_inodes; i++)
		file.b_inode.iNode[i].free = true;
	// Populate free_inode list
	for(i=0; i<file.sp.n_inodes; i++)
		file.sp.free_inode.push_back(true);

	file.b = (data_block *)malloc((::n_blocks-3)*sizeof(data_block));

	for(i=0; i<(::block_size-3); i++){
		file.b[i].next = i+4;
		file.b[i].data = (char *)malloc(::block_size*1024*sizeof(char));
	}

	file.b_inode.iNode[0].free = false;
	file.b_inode.iNode[0].type = false;
	file.sp.free_inode[0] = false;
	for(int i; i<file.b_inode.iNode[0].dp.size(); i++)
		file.b_inode.iNode[0].dp.push_back(-1);


	cur_dir = 0;

}

/////////////////////////////////////////////////////////////////////////////////////////
int my_mkdir(char *str){
	// If str is '.' or '..', return -1 
	if(!strcmp(str, ".") || !strcmp(str, ".."))
		return -1;

	// Get current directory information from cur_dir
	// check the index that is empty (scan through DP first (Use dp.size(), then SIP, then DIP), If none, return -1
	int i;
	int empty_index = -1;
	int pointer_type = -1;
	for(i=0; i<file.b_inode.iNode[cur_dir].dp.size(); i++){
		if(file.b_inode.iNode[0].dp[i] == -1){
			empty_index = i;
			pointer_type = 0;
			break;
		}
	}// Need to do for SIP and DIP
	if(empty_index == -1)
		return -1;

	// Now select the first free block and write the name of the directory and the inode number
	int freeblock = file.sys.free_block;
	int freeinode = -1;
	dir_block d;
	strcpy(d.name, str);
	// Find free inode
	for(i=0; i<file.sp.n_inodes; i++){
		if(file.sp.free_inode[i] == true){
			freeinode = i;
			break;
		}
	}
	if(freeinode == -1)
		return -1;

	////////// DID NOT DO THE POSITION OF THE (CHAR *) WHERE THE INFO ABOUT THE NEW DIR CAN BE WRITTEN////////
	//////// I GUESS WE NEED TO FIRST ALLOCATE THAT MANY NUMBER OF DIR_BLOCK IN CHAR FORMAT AND THEN MAKE A TABLE AND STORE IT IN THE TABLE //////////

	d.inode = (short int)freeinode;

	file.sp.free_block = file.b[freeblock].next;  // New first free block stored in super block
	file.b[freeblock].next = -1;
	file.b[freeblock].data = (char *)d;

	// Fill the necessary info in the selected inode and assign it as a directory type inode. Also mark the inode as not free
	if(pointer_type == 0){
		file.b_inode.iNode[cur_dir].dp[empty_index] = freeblock;
	}
	file.b_inode.iNode[freeinode].free = false;
	file.b_inode.iNode[freeinode].type = false;
	file.sp.free_inode[freeinode] = false;
	for(i; i<file.b_inode.iNode[freeinode].dp.size(); i++)
		file.b_inode.iNode[0].dp.push_back(-1);

	// Fill two DP with '.' and '..' that would point to its own inode and the inode of cur_dir
	freeblock = file.sys.free_block;
	strcpy(d.name, "..");
	d.inode = (short int)cur_dir; // Parent directory for the newly formed dir is the current directory

	file.sp.free_block = file.b[freeblock].next;
	file.b[freeblock].next = -1;
	file.b[freeblock].data = (char *)d;

	// return inode number
	return freeinode;
}

/////////////////////////////////////////////////////////////////////////////////////////
int my_rmdir(char *str){
	// Check if str is '.' or '..' , return -1
	if(!strcmp(str, ".") || !strcmp(str, ".."))
		return -1;
	// in the current directory, scan through all the blocks pointed by DP, SIP and DIP until the name of the directory matches str
	
	// Get the corresponding inode number of the directory.
	// Remove the entry from the block and if the block becomes free, add the free block to the linked llist of free block.
	// Empty all the fields of the inode and also all the directories or files that were present in that directory
	// On success return cur_dir (inode number of directory where the hypothetical pointer is)
}

////////////////////////////////////////////////////////////////////////////////////////
int my_chdir(char *str){
	//////////// INITIALLY LETS DO NO SUPPORT FOR COMPLEX str, LIKE "../abc/def/ghi"///////////////
	/////////// TO SUPPORT THIS WE NEED TO TOKENIZE STRING AND RUN THE FOLLOWING CODE IN A LOOP////////////

	// Check if str is '.', if yes return cur_dir that stores the inode number
	// If str is '..', go to the DP where it is present(should be known beforehand), and find the inode number.
	// Change the cur_dir to this inode and return cur_dir

	// In the current directory, scan through all the blocks pointed by DP, SIP, DIP until str matches
	// Get the corresponding inode number and see if the type field of inode is false (dir), If no return -1
	// If yes, change the cur_dir with the inode value and return it
}

/////////////////////////////////////////////////////////////////////////////////////////
int my_open(char *str){
	//current directory information is stored in cur_dir
	// In the current directory, scan through all the blocks pointed by DP, SIP, DIP until str matches
	// If str matches
		// Get the inode number of the entry.
		// Check if the type field of the inode entry is false (means dir), if yes return -1
		// If the inode is a file type, update the FD table.
		// Return the index of the FD table
	// If str does not match
		// Find the index that is empty (scanning through DP, SIP, DIP; store it from the previous search in step 2), If none return -1
		// Go to the corressponding block and make an entry and assign an inode number for the file.
		// Fill the necessary details in the inode.
		// Update the FD table, current block of the file descriptor will be the first empty block
		// Return the index of the FD table
}

/////////////////////////////////////////////////////////////////////////////////////////
int my_read(int fd, char* buf, int count){
	//check if FD[fd] is valid or fd <= FD.size or fd=-1, if no return -1

	// For i from 0 to count, do
		//Go to the current_block of fd and start reading data from the current offset., and write it to buff
		//Break if null char read
		// Increment current_offset
		// if current offset is larger than or equal to the block size(should be block size - next),
			//get the next block from either DP, SIP or DIP
			// If -1 break, else change the current_block to the new and current offset to 0

	// Return no. of bytes read
}

////////////////////////////////////////////////////////////////////////////////////////
int my_write(int fd, char *buf, size_t count){
	//check if FD[fd] is valid or fd <= FD.size or fd=-1, if no return -1

	// for i from 0 to count, do
		// write to the current block at current offset
		// break if null character is written
		// Increment offset
		// Simultaneously, update the file sixe field of the inode
		// If current offset is larger than or equal to (block size-sizeof(next))
			// change the current block and current offset (Select the first free block using the free block pointer of super block),
			// add this info in the corresponding inode and change the status of the new block as not free

	// return number of bytes written
}

/////////////////////////////////////////////////////////////////////////////////////////
int my_close(int fd){
	// invalidate the valid field in fd and return 1, if no such fd then return -1
}
