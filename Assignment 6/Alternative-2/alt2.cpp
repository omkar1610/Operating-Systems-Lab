#include "alt2.h"

#define SAME_DIR -5
#define REGULAR 1
#define DIRECTORY 2

using namespace std;


// Declerations
long long int file_size, block_size, n_blocks;
file_sys file;
vector<FD_t> FD;

int cur_dir;
int prev_dir;


/////////////////////////////////////////////////////////////////////////////////////////
void init(long long int a, long long int b, long long int c)
{
	::file_size = a;
	::block_size = b;
	::n_blocks = c;

	int i;

	file.sp.file_sys_size = ::file_size;
	file.sp.block_size = ::block_size;
	file.sp.n_blocks = ::n_blocks;
	file.sp.n_inodes = 2 * (::block_size/sizeof(inode));
	file.sp.name = "INODE";
	file.sp.first_free_block = 3;
	
	file.b_inode.iNode = (inode *)malloc(file.sp.n_inodes * sizeof(inode));
	file.sp.free_inode=(bool*)malloc(file.sp.n_inodes * sizeof(bool));
	// for(i=0; i<file.sp.n_inodes; i++)
	// 	file.b_inode.iNode[i].free = true;
	// Populate free_inode list
	bool topush=true;
	
	for(i=0; i<file.sp.n_inodes; i++)
		file.sp.free_inode[i]=true;
	
	file.b = (data_block *)malloc((::n_blocks-3)*sizeof(data_block));
	
	for(i=0; i<(::block_size-3); i++)
	{
		file.b[i].next = i+4;
		file.b[i].data = (char *)malloc(::block_size*1024*sizeof(char));
	}
	
	// file.b_inode.iNode[0].free = false;
	// file.b_inode.iNode[0].type = false;
	
	file.sp.free_inode[0] = false;
	//cout<<"\nHere..."<<endl;
	int tp=-1;
	//for(int i=0; i<5; i++)
	//	file.b_inode.iNode[0].dp.push_back(tp);

	// LAST DATA BLOCK NOT INITIALIZED
	file.b_inode.iNode[0].dp.resize(5,tp);
	file.b_inode.iNode[0].dp[0] = SAME_DIR;
	file.b_inode.iNode[0].sip=file.b_inode.iNode[0].dip=-1;
	cur_dir = 0;
	prev_dir=-1;
}

/////////////////////////////////////////////////////////////////////////////////////////
int getpointertonextentry(int inodeno, int moveby, int* currentdatablockno, int* currentdatablockoffset, int* indirectiontype, int* directoffset, int* sipblockoffset, int* dipblockoffset1, int* dipblockoffset2 )
{

	if(*currentdatablockoffset+moveby<block_size)
	{
		*currentdatablockoffset=*currentdatablockoffset+moveby;
		return (*indirectiontype);
	}
	else if(*currentdatablockno==file.b_inode.iNode[inodeno].last_data_block) // YE TO HOGA HI
	{
		return -1;
	}
	else if (*indirectiontype==0 && *directoffset<4)
	{
		*directoffset++;
		*currentdatablockno=file.b_inode.iNode[inodeno].dp[*directoffset];
		*currentdatablockoffset=0;
		return 0;
	}
	else if (*indirectiontype==0 && *directoffset==4)
	{
		*indirectiontype=1;
		*sipblockoffset=0;
		int* ptr=(int*)file.b[file.b_inode.iNode[inodeno].sip-3].data;
		*currentdatablockno=*ptr;
		*currentdatablockoffset=0;
		return (*indirectiontype);
	}
	else if(*indirectiontype==1 && (*sipblockoffset+sizeof(int))<block_size)
	{
		*sipblockoffset+=sizeof(int);
		int* ptr=(int*)((char*)file.b[file.b_inode.iNode[inodeno].sip-3].data+sizeof(int));
		*currentdatablockno=*ptr;
		*currentdatablockoffset=0;
		return (*indirectiontype);
	}
	else if(*indirectiontype==1 && (*sipblockoffset+sizeof(int))>=block_size)
	{
		*indirectiontype=2;
		*dipblockoffset1=0;
		*dipblockoffset2=0;
		int dipblockno=file.b_inode.iNode[inodeno].dip;
		int* tempptr=(int*)((char*)file.b[dipblockno-3].data+*dipblockoffset1);
		int actualsearchinblock=*tempptr;
		int* tempptr2=(int*)((char*)file.b[actualsearchinblock-3].data+*dipblockoffset2);
		*currentdatablockno=*tempptr2;
		*currentdatablockoffset=0;
		return (*indirectiontype);
	}
	else if(*indirectiontype==2 && (*dipblockoffset2+sizeof(int))<block_size)
	{
		*dipblockoffset2+=sizeof(int);
		int dipblockno=file.b_inode.iNode[inodeno].dip;
		int* tempptr=(int*)((char*)file.b[dipblockno-3].data+*dipblockoffset1);
		int actualsearchinblock=*tempptr;
		int* tempptr2=(int*)((char*)file.b[actualsearchinblock-3].data+*dipblockoffset2);
		*currentdatablockno=*tempptr2;
		*currentdatablockoffset=0;
		return (*indirectiontype);
	}
	else if(*indirectiontype==2 && (*dipblockoffset2+sizeof(int))>=block_size && (*dipblockoffset1+sizeof(int))<block_size)
	{
		*dipblockoffset1+=sizeof(int);
		*dipblockoffset2=0;
		int dipblockno=file.b_inode.iNode[inodeno].dip;
		int* tempptr=(int*)((char*)file.b[dipblockno-3].data+*dipblockoffset1);
		int actualsearchinblock=*tempptr;
		int* tempptr2=(int*)((char*)file.b[actualsearchinblock-3].data+*dipblockoffset2);
		*currentdatablockno=*tempptr2;
		*currentdatablockoffset=0;
		return (*indirectiontype);
	}
	else
	{
		cout<<"The file system is full"<<endl;
		return -2;
	}


}

/////////////////////////////////////////////////////////////////////////////////////////
int my_mkdir(char *str)
{
	// int notfound=0;
	// int found=0;
	// int toaddindirtype=0;
	// int emptyposblock=-1;
	// int emptyposoffset=-1;

	int inodeno = cur_dir; //jobhi inode we are working on
	int currentdatablockno = file.b_inode.iNode[cur_dir].dp[1];  //initialize to dp[0] wala block no
	int currentdatablockoffset = 0; //initially 0
	int indirectiontype = 0; //initially 0
	int directoffset = 0; //initially 0
	int sipblockoffset = 0; //initially 0
	int dipblockoffset1 = 0; //initially 0
	int dipblockoffset2 = 0; //initially 0

	//search if same named file exists, if yes return -1
	if(!strcmp(str, ".") || !strcmp(str, "..")){
		return -1;
	}
	int i;
	int block;
	for(i=2; i<5; i++){
		currentdatablockno = file.b_inode.iNode[cur_dir].dp[i];
		if(currentdatablockno == -1)
			break;

		char data[14];
		memcpy(data, file.b[currentdatablockno].data + currentdatablockoffset, 14);
		if(!strcmp(data, str))
			return -1;

		indirectiontype = getpointertonextentry(cur_dir, 14, &currentdatablockno, &currentdatablockoffset, &indirectiontype, &directoffset, &sipblockoffset, &dipblockoffset1, &dipblockoffset2);
		if(indirectiontype == -2)
			return -1;
		if(indirectiontype == -1)
			break;
	}

	//if no, 'add' an entry to the current directory blocks data
	//find an empty inode
	short int freeinode = -1;
	for(i=0; i<file.sp.n_inodes; i++){
		if(file.sp.free_inode[i] == true){
			freeinode = (short int)i;
			break;
		}
	}
	if(freeinode == -1)
		return -1;


	if(file.sp.block_size - currentdatablockoffset >= 14){
		memcpy(file.b[currentdatablockno].data + currentdatablockoffset, str, 14);
		indirectiontype = getpointertonextentry(cur_dir, 14, &currentdatablockno, &currentdatablockoffset, &indirectiontype, &directoffset, &sipblockoffset, &dipblockoffset1, &dipblockoffset2);
		if(indirectiontype == -2)
			return -1;
	}
	else{
		int freeblock = file.sp.first_free_block;
		file.sp.first_free_block = file.b[freeblock].next;

		file.b_inode.iNode[cur_dir].dp[directoffset+1] = freeblock;
		file.b_inode.iNode[cur_dir].last_data_block = freeblock;
		// Do we need to change directoffset to directoffset+1 ?
		memcpy(file.b[currentdatablockno].data + currentdatablockoffset, str, 14);
		indirectiontype = getpointertonextentry(cur_dir, 14, &currentdatablockno, &currentdatablockoffset, &indirectiontype, &directoffset, &sipblockoffset, &dipblockoffset1, &dipblockoffset2);
		if(indirectiontype == -2)
			return -1;
	}


	if(file.sp.block_size - currentdatablockoffset >= 2){
		memcpy(file.b[currentdatablockno].data + currentdatablockoffset, (char *)freeinode, 2);
		indirectiontype = getpointertonextentry(cur_dir, 2, &currentdatablockno, &currentdatablockoffset, &indirectiontype, &directoffset, &sipblockoffset, &dipblockoffset1, &dipblockoffset2);
		if(indirectiontype == -2)
			return -1;
	}
	else{
		int freeblock = file.sp.first_free_block;
		file.sp.first_free_block = file.b[freeblock].next;

		file.b_inode.iNode[cur_dir].dp[directoffset+1] = freeblock;
		file.b_inode.iNode[cur_dir].last_data_block = freeblock;
		// Do we need to change directoffset to directoffset+1 ?
		memcpy(file.b[currentdatablockno].data + currentdatablockoffset, (char *)freeinode, 2);
		indirectiontype = getpointertonextentry(cur_dir, 2, &currentdatablockno, &currentdatablockoffset, &indirectiontype, &directoffset, &sipblockoffset, &dipblockoffset1, &dipblockoffset2);
		if(indirectiontype == -2)
			return -1;
	}
	// memcpy(file.b[currentdatablockno].data + currentdatablockoffset, str, 14); //doubt

	// indirectiontype = getpointertonextentry(cur_dir, 14, &currentdatablockno, &currentdatablockoffset, &indirectiontype, &directoffset, &sipblockoffset, &dipblockoffset1, &dipblockoffset2);
	// if(indirectiontype == -2)
	// 	return -1;

	// if(indirectiontype == -1){
	// 	int freeblock = file.sp.first_free_block;
	// 	file.sp.first_free_block = file.b[freeblock].next;

	// 	file.b_inode.iNode[cur_dir].dp[directoffset+1] = freeblock;
	// 	file.b_inode.iNode[cur_dir].last_data_block = freeblock;
	// 	// Do we need to change directoffset to directoffset+1 ? 
	// 	indirectiontype = getpointertonextentry(cur_dir, 14, &currentdatablockno, &currentdatablockoffset, &indirectiontype, &directoffset, &sipblockoffset, &dipblockoffset1, &dipblockoffset2);
	// }
	// memcpy(file.b[currentdatablockno].data + currentdatablockoffset, (char *)freeinode, 2);
	// indirectiontype = getpointertonextentry(cur_dir, 2, &currentdatablockno, &currentdatablockoffset, &indirectiontype, &directoffset, &sipblockoffset, &dipblockoffset1, &dipblockoffset2);
	// if(indirectiontype == -2)
	// 	return -1;

	// if(indirectiontype == -1){
	// 	int freeblock = file.sp.first_free_block;
	// 	file.sp.first_free_block = file.b[freeblock].next;

	// 	file.b_inode.iNode[cur_dir].dp[directoffset+1] = freeblock;
	// 	file.b_inode.iNode[cur_dir].last_data_block = freeblock;
	// 	// Do we need to change directoffset to directoffset+1 ?
	// 	indirectiontype = getpointertonextentry(cur_dir, 2, &currentdatablockno, &currentdatablockoffset, &indirectiontype, &directoffset, &sipblockoffset, &dipblockoffset1, &dipblockoffset2);
	// }

	//in the empty inode, add the file name. Find an empty block and set that as startin block. Put . and .. info.
	file.b_inode.iNode[freeinode].last_data_block = file.sp.first_free_block;
	file.b_inode.iNode[freeinode].dp[0] = SAME_DIR;
	file.b_inode.iNode[freeinode].sip = file.b_inode.iNode[freeinode].dip=-1;
	file.b_inode.iNode[freeinode].dp[1] = cur_dir;

	//return 1
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////
int my_rmdir(char *str)
{
	int inodeno = cur_dir; //jobhi inode we are working on
	int currentdatablockno = file.b_inode.iNode[cur_dir].dp[1];  //initialize to dp[0] wala block no
	int currentdatablockoffset = 0; //initially 0
	int indirectiontype = 0; //initially 0
	int directoffset = 0; //initially 0
	int sipblockoffset = 0; //initially 0
	int dipblockoffset1 = 0; //initially 0
	int dipblockoffset2 = 0; //initially 0

	//search if same named file exists, if no return -1
	//go thru all the files associated with this and dealocate said files' blocks and inodes
	//remove this folder entry from current directory
	
}

////////////////////////////////////////////////////////////////////////////////////////
int my_chdir(char *str)
{
	int inodeno = cur_dir; //jobhi inode we are working on
	int currentdatablockno = file.b_inode.iNode[cur_dir].dp[1];  //initialize to dp[0] wala block no
	int currentdatablockoffset = 0; //initially 0
	int indirectiontype = 0; //initially 0
	int directoffset = 0; //initially 0
	int sipblockoffset = 0; //initially 0
	int dipblockoffset1 = 0; //initially 0
	int dipblockoffset2 = 0; //initially 0

	//search if same named file exists, if no return -1
	if(!strcmp(str, "."))
		return cur_dir;

	if(!strcmp(str, "..")){
		prev_dir = cur_dir;
		cur_dir = file.b_inode.iNode[cur_dir].dp[1];
		return cur_dir;
	}

	int i;
	int block;
	int foundblock = -1;
	for(i=2; i<5; i++){
		currentdatablockno = file.b_inode.iNode[cur_dir].dp[i];
		if(currentdatablockno == -1)
			break;

		char data[14];
		memcpy(data, file.b[currentdatablockno].data + currentdatablockoffset, 14);
		if(!strcmp(data, str))
			foundblock = currentdatablockno;

		indirectiontype = getpointertonextentry(cur_dir, 14, &currentdatablockno, &currentdatablockoffset, &indirectiontype, &directoffset, &sipblockoffset, &dipblockoffset1, &dipblockoffset2);
		if(indirectiontype == -2)
			return -1;
		if(indirectiontype == -1)
			return -1;

		if(foundblock != -1){
			char inode[2];
			memcpy(inode, file.b[currentdatablockno].data + currentdatablockoffset, 2);
			int inode_no = *(short int *)inode;
			indirectiontype = getpointertonextentry(cur_dir, 1, &currentdatablockno, &currentdatablockoffset, &indirectiontype, &directoffset, &sipblockoffset, &dipblockoffset1, &dipblockoffset2);
			if(indirectiontype == -2)
				return -1;
			if(indirectiontype == -1)
				return -1;
			indirectiontype = getpointertonextentry(cur_dir, 1, &currentdatablockno, &currentdatablockoffset, &indirectiontype, &directoffset, &sipblockoffset, &dipblockoffset1, &dipblockoffset2);


			//update current and prev directory.
			prev_dir = cur_dir;
			cur_dir = inode_no;
			return cur_dir;
		}
	}
	
}

/////////////////////////////////////////////////////////////////////////////////////////
int my_open(char *str)
{
	int inodeno = cur_dir; //jobhi inode we are working on
	int currentdatablockno = file.b_inode.iNode[cur_dir].dp[1];  //initialize to dp[0] wala block no
	int currentdatablockoffset = 0; //initially 0
	int indirectiontype = 0; //initially 0
	int directoffset = 0; //initially 0
	int sipblockoffset = 0; //initially 0
	int dipblockoffset1 = 0; //initially 0
	int dipblockoffset2 = 0; //initially 0

	//search if same named file exists, if no return -1
	//if that file is a dir, return -2
	//else 'make' a new FD table entry and return its index
}

/////////////////////////////////////////////////////////////////////////////////////////
int my_read(int fd, char* buf, int count)
{
	int inodeno = FD[fd].inode_no; //jobhi inode we are working on
	int currentdatablockno = FD[fd].current_block;  //initialize to dp[0] wala block no
	int currentdatablockoffset = FD[fd].current_offset; //initially 0
	int indirectiontype = FD[fd].indirection_type; //initially 0
	int directoffset = FD[fd].directoffset; //initially 0
	int sipblockoffset = FD[fd].index_sip; //initially 0
	int dipblockoffset1 = FD[fd].index_dip1; //initially 0
	int dipblockoffset2 = FD[fd].index_dip2; //initially 0

	//if fd is not valid then return -1
	if(fd>=FD.size() || fd<0 || FD[fd].valid==0)
	{
		return -1;
	}

	//else read till count or eof
	int i;
	int bufpointer = 0;
	for(i=0; i<count; i++){
		buf[bufpointer++] = file.b[FD[fd].current_block].data[FD[fd].current_offset];
		if(buf[bufpointer-1]  = '\0')
			break;

		indirectiontype = getpointertonextentry(inodeno, 1, &currentdatablockno, &currentdatablockoffset, &indirectiontype, &directoffset, &sipblockoffset, &dipblockoffset1, &dipblockoffset2);		
		
		FD[fd].current_block = currentdatablockno;
		FD[fd].current_offset = currentdatablockoffset;
		FD[fd].indirectiontype = indirectiontype;
		FD[fd].directoffset = directoffset;
		FD[fd].index_sip = sipblockoffset;
		FD[fd].index_dip1 = dipblockoffset1;
		FD[fd].index_dip2 = dipblockoffset2;
		
		if(indirectiontype == -2)
			return -1;
		if(indirectiontype == -1){
			int freeblock = file.sp.first_free_block;
			file.sp.first_free_block = file.b[freeblock].next;

			file.b_inode.iNode[inodeno].dp[directoffset+1] = freeblock;
			file.b_inode.iNode[inodeno].last_data_block = freeblock;

			indirectiontype = getpointertonextentry(inodeno, 1, &currentdatablockno, &currentdatablockoffset, &indirectiontype, &directoffset, &sipblockoffset, &dipblockoffset1, &dipblockoffset2);

			FD[fd].current_block = currentdatablockno;
			FD[fd].current_offset = currentdatablockoffset;
			FD[fd].indirectiontype = indirectiontype;
			FD[fd].directoffset = directoffset;
			FD[fd].index_sip = sipblockoffset;
			FD[fd].index_dip1 = dipblockoffset1;
			FD[fd].index_dip2 = dipblockoffset2;
		}
		
	}
	return bufpointer;
}

////////////////////////////////////////////////////////////////////////////////////////
int my_write(int fd, char *buf, size_t count)
{
	int inodeno = FD[fd].inode_no; //jobhi inode we are working on
	int currentdatablockno = FD[fd].current_wblock;  //initialize to dp[0] wala block no
	int currentdatablockoffset = FD[fd].current_woffset; //initially 0
	int indirectiontype = FD[fd].windirection_type; //initially 0
	int directoffset = FD[fd].wdirectoffset; //initially 0
	int sipblockoffset = FD[fd].windex_sip; //initially 0
	int dipblockoffset1 = FD[fd].windex_dip1; //initially 0
	int dipblockoffset2 = FD[fd].windex_dip2; //initially 0

	//if fd is not valid then return -1
	if(fd>=FD.size() || fd<0 || FD[fd].valid==0)
	{
		return -1;
	}

	//else write till count
	int i;
	int bufpointer = 0;
	for(i=0; i<count; i++){
		if(file.sp.block_size - currentdatablockoffset >= 1){
			file.b[FD[fd].current_block].data[FD[fd].current_woffset] = buf[bufpointer++];
			if(buf[bufpointer-1] == '\0')
				break;
			indirectiontype = getpointertonextentry(inodeno, 1, &currentdatablockno, &currentdatablockoffset, &indirectiontype, &directoffset, &sipblockoffset, &dipblockoffset1, &dipblockoffset2);		
		
			FD[fd].current_wblock = currentdatablockno;
			FD[fd].current_woffset = currentdatablockoffset;
			FD[fd].windirectiontype = indirectiontype;
			FD[fd].wdirectoffset = directoffset;
			FD[fd].windex_sip = sipblockoffset;
			FD[fd].windex_dip1 = dipblockoffset1;
			FD[fd].windex_dip2 = dipblockoffset2;
			
			if(indirectiontype == -2)
				return -1;
		}
		else{
			int freeblock = file.sp.first_free_block;
			file.sp.first_free_block = file.b[freeblock].next;

			file.b_inode.iNode[inodeno].dp[directoffset+1] = freeblock;
			file.b_inode.iNode[inodeno].last_data_block = freeblock;

			file.b[FD[fd].current_block].data[FD[fd].current_woffset] = buf[bufpointer++];
			if(buf[bufpointer-1] == '\0')
				break;
			indirectiontype = getpointertonextentry(inodeno, 1, &currentdatablockno, &currentdatablockoffset, &indirectiontype, &directoffset, &sipblockoffset, &dipblockoffset1, &dipblockoffset2);
			
			FD[fd].current_wblock = currentdatablockno;
			FD[fd].current_woffset = currentdatablockoffset;
			FD[fd].windirectiontype = indirectiontype;
			FD[fd].wdirectoffset = directoffset;
			FD[fd].windex_sip = sipblockoffset;
			FD[fd].windex_dip1 = dipblockoffset1;
			FD[fd].windex_dip2 = dipblockoffset2;

			if(indirectiontype == -2)
				return -1;

		}
	}
	return bufpointer;
}

/////////////////////////////////////////////////////////////////////////////////////////
int my_close(int fd)
{
	int inodeno = cur_dir; //jobhi inode we are working on
	int currentdatablockno = file.b_inode.iNode[cur_dir].dp[1];  //initialize to dp[0] wala block no
	int currentdatablockoffset = 0; //initially 0
	int indirectiontype = 0; //initially 0
	int directoffset = 0; //initially 0
	int sipblockoffset = 0; //initially 0
	int dipblockoffset1 = 0; //initially 0
	int dipblockoffset2 = 0; //initially 0

	//if fd is not valid then return -1
	if(fd>=FD.size() || fd<0 || FD[fd].valid==0)
	{
		return -1;
	}
	//else drop the entry
	FD[fd].valid = 0;
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////
int my_cat(int fd){
	
}
