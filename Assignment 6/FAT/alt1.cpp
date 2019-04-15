#include "alt1.h"

using namespace std;


// Declerations
int file_size, block_size, n_blocks;
file_sys file;
vector<FD_t> FD;



////////////////////////////////////////////////////////////////////////
void init(int a, int b, int c){
	::file_size = a;
	::block_size = b;
	::n_blocks = c;

	int i;

	// Allocate memory for blocks
	file.b = (block *)malloc((::n_blocks-3)*sizeof(block));
	// Allocate memory in data blocks
	for(i=0;i<(::n_blocks)-3;i++)
		file.b[i].data=(char *)malloc(::block_size*1024*sizeof(char));

	file.sp.file_size = ::file_size;
	file.sp.block_size = ::block_size;
	file.sp.n_blocks = ::n_blocks;
	file.sp.n_files = 0;
	file.sp.name = "FAT";
	file.sp.free_block = (int *)malloc(::n_blocks*sizeof(int));
	file.b1.fat = (int *)malloc((::n_blocks-3)*sizeof(int));
	file.sp.ptr = &(file.b2);
	int blocksbyfat=sizeof(file.b1.fat)/::block_size;

	// Initialize bit vector
	file.sp.free_block[0] = 1;
	file.sp.free_block[1] = 1;
	file.sp.free_block[2] = 1;
	int j;
	for(j=0;j<blocksbyfat-1;j++)
		file.sp.free_block[3+j]=1;
	
	for(i=j; i<(::n_blocks); i++)
		file.sp.free_block[i] = 0;
	
	// Initialize FAT table
	for(i=0; i<(::n_blocks)-3; i++)
		file.b1.fat[i] = -1;
	
}

//////////////////////////////////////////////////////////////////////////////////////////////
int my_open(string str)
{
	/* If there is already something in the directory, check if the entered filename matches.
	   It yes, return index
	*/
	if(file.sp.n_files)
	{
		int i;
		int found_location=-1;
		for(i=0; i<file.b2.dir.size(); i++)
		{
			directory d = file.b2.dir[i];
			if(d.name == str)
			{
				found_location=i;
				break;
			}
			
		}
		// if the file is found, change the file descriptor
		if(found_location!=-1)
		{
			for(int i=0;i<FD.size();i++)
			{
				// If there is an invalid file descriptor, use that file descriptor
				if(FD[i].valid==0)
				{
					FD[i].valid=1;
					FD[i].dir_no=found_location;
					FD[i].current_block=file.b2.dir[found_location].index;
					FD[i].current_offset=0;
					FD[i].current_wblock=file.b2.dir[found_location].index;
					FD[i].current_woffset=0;
					FD[i].my_no=i;
					return i;
				}
			}
			// If there is no invalid file descriptor, create a new fd
			FD_t tempfd;
			tempfd.valid=1;
			tempfd.dir_no=found_location;
			tempfd.current_block=file.b2.dir[found_location].index;
			tempfd.current_offset=0;
			tempfd.current_wblock=file.b2.dir[found_location].index;
			tempfd.current_woffset=0;
			tempfd.my_no=FD.size();
			FD.push_back(tempfd);
			return (tempfd.my_no);
		}
	}

 	// If there is no existing file, create a new file and update file descriptor
	int n = file.sp.n_files;

	// Randomly select an index
	int r;
	do
	{
		r = rand()%(file.sp.n_blocks-3);
	}while(file.sp.free_block[r+3]!=0);

	directory d;
	d.name=str;
	d.index = r;
	file.b2.dir.push_back(d);
	// Increase number of files
	file.sp.n_files++;
	file.sp.free_block[r+3]=1;

	// Update the file descriptors
	for(int i=0;i<FD.size();i++)
	{
		if(FD[i].valid==0)
		{
			FD[i].valid=1;
			FD[i].dir_no=file.b2.dir.size()-1;
			FD[i].current_block=file.b2.dir[file.b2.dir.size()-1].index;
			FD[i].current_offset=0;
			FD[i].current_wblock=file.b2.dir[file.b2.dir.size()-1].index;
			FD[i].current_woffset=0;
			FD[i].my_no=i;
			return i;
		}
	}
	FD_t tempfd;
	tempfd.valid=1;
	tempfd.dir_no=file.b2.dir.size()-1;
	tempfd.current_block=file.b2.dir[tempfd.dir_no].index;
	tempfd.current_offset=0;
	tempfd.current_wblock=file.b2.dir[tempfd.dir_no].index;
	tempfd.current_woffset=0;
	tempfd.my_no=FD.size();

	FD.push_back(tempfd);
	return (tempfd.my_no);

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int my_read(int fd, char *buf, int count)
{
	
	// Check if the fd is valid, if not return -1
	if(fd>=FD.size() || fd<0 || FD[fd].valid==0)
	{
		return -1;
	}
	//read from fp to count or till end of file and store in location pointed to by buffer
	int i=0;
	int bufpointer=0;
	for(i;i<count;i++)
	{
		buf[bufpointer++]=file.b[FD[fd].current_block].data[FD[fd].current_offset];
		if(buf[bufpointer-1]=='\0') break;
		FD[fd].current_offset++;
		if(FD[fd].current_offset>=block_size)
		{
		//change current block and current offset
			if(file.b1.fat[FD[fd].current_block]==-1)
			{
				break;
			}
			else
			{
				int nextblock=file.b1.fat[FD[fd].current_block];
				FD[fd].current_block=nextblock;
				FD[fd].current_offset=0;
				if(file.sp.free_block[nextblock+3]==0)
					break;
				continue;
			}
		}
	}
	//return no of bytes read
	return bufpointer;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int my_write(int fd, const char *buf, size_t count)
{
	// Check if the fd is valid, if not return -1
	if(fd>=FD.size() || fd<0 || FD[fd].valid==0)
	{
		return -1;
	}
	//read from location pointed to by buffer(count no of characters and store in fp to ahead
	int i=0;
	int bufpointer=0;
	for(i;i<count;i++)
	{
		file.b[FD[fd].current_wblock].data[FD[fd].current_woffset] = buf[bufpointer++];
		if(buf[bufpointer-1]=='\0') break;
		file.sp.free_block[FD[fd].current_wblock+3]=1;
		FD[fd].current_woffset++;
		if(FD[fd].current_woffset>=block_size)
		{
		//change current block and current offset
			if(file.b1.fat[FD[fd].current_wblock]==-1)
			{
				int r;
				do
				{
					r = rand()%(file.sp.n_blocks-3);
				}while(file.sp.free_block[r+3]!=0);
				file.b1.fat[FD[fd].current_wblock]=r;
				FD[fd].current_wblock=r;
				FD[fd].current_woffset=0;
				continue;
			}
			else
			{
				int nextblock=file.b1.fat[FD[fd].current_wblock];
				FD[fd].current_wblock=nextblock;
				FD[fd].current_woffset=0;
				continue;
			}
		}
	}
	//return no of bytes written
	return bufpointer;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int my_close(int fd)
{
	//invalidate the file descriptor, return 1, if no such file descriptor return -1
	if(fd>=FD.size() || fd<0 || FD[fd].valid==0)
	{
		return -1;
	}
	else
	{
		FD[fd].valid=0;
		return 1;
	
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
int my_cat(int fd)
{
	if(fd>=FD.size() || fd<0 || FD[fd].valid==0)
	{
		return -1;
	}

	int current_block=file.b2.dir[FD[fd].dir_no].index;
	int current_offset=0;
	while(1)
	{
		
		cout<<file.b[current_block].data[current_offset];
		if(file.b[current_block].data[current_offset]=='\0') break;
		current_offset++;
		if(current_offset>=block_size)
		{
		//change current block and current offset
			if(file.b1.fat[current_block]==-1)
			{
				break;
			}
			else
			{
				int nextblock=file.b1.fat[current_block];
				current_block=nextblock;
				current_offset=0;
				if(file.sp.free_block[nextblock+3]==0)
					break;
				continue;
			}
		}
	}
	cout<<endl;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int my_copy(int fd,int linuxfd,int flag)//if flag is 0 then copy from linuxfd to myfd and vice versa for flag 1
{
	if (flag==0)
	{
		if(fd>=FD.size() || fd<0 || FD[fd].valid==0)
		{
			return -1;
		}
		//read from location pointed to by buffer(count no of characters and store in fp to ahead
		int i=0;
		int bufpointer=0;
		int on=1;
		char ch[1];
		int c;
		while(on)
		{
			c=read(linuxfd,ch,1);
			if(c<1)
				break;
			file.b[FD[fd].current_wblock].data[FD[fd].current_woffset] = ch[0];
			//if(buf[bufpointer-1]=='\0') break;
			file.sp.free_block[FD[fd].current_wblock+3]=1;
			FD[fd].current_woffset++;
			if(FD[fd].current_woffset>=block_size)
			{
			//change current block and current offset
				if(file.b1.fat[FD[fd].current_wblock]==-1)
				{
					int r;
					do
					{
						r = rand()%(file.sp.n_blocks-3);
					}while(file.sp.free_block[r+3]!=0);
					file.b1.fat[FD[fd].current_wblock]=r;
					FD[fd].current_wblock=r;
					FD[fd].current_woffset=0;
					continue;
				}
				else
				{
					int nextblock=file.b1.fat[FD[fd].current_wblock];
					FD[fd].current_wblock=nextblock;
					FD[fd].current_woffset=0;
					continue;
				}
			}
		}
		//return no of bytes written
		return bufpointer;
	
	}
	else if (flag==1)
	{
		//go through the FD to find the entry, if not found return -1
		if(fd>=FD.size() || fd<0 || FD[fd].valid==0)
		{
			return -1;
		}
		//read from fp to count or till end of file and store in location pointed to by buffer
		int i=0;
		int bufpointer=0;
		int c;
		char ch;
		while(1)
		{
			ch= file.b[FD[fd].current_block].data[FD[fd].current_offset];
			if(ch == '\0') break;
			c = write(linuxfd, &ch, 1);
			if(c<1)
				break;
			FD[fd].current_offset++;
			if(FD[fd].current_offset>=block_size)
			{
			//change current block and current offset
				if(file.b1.fat[FD[fd].current_block]==-1)
				{
					break;
				}
				else
				{
					int nextblock=file.b1.fat[FD[fd].current_block];
					FD[fd].current_block=nextblock;
					FD[fd].current_offset=0;
					if(file.sp.free_block[nextblock+3]==0)
						break;
					continue;
				}
			}
		}
		//return no of bytes read
		return bufpointer;
	
	}
	else
	{
		return -1;
	}

}
