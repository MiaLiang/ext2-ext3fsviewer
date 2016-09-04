
#include <stdio.h>        
#include <stdlib.h>       
#include <string.h>   
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include "../include/ext2_head.h"
#include "BlockIO.h"

static int s_file = 0;
static int s_imageSize = 0;

static ext2_super_block s_superBlock;	//超级块
static int s_firstDataBlock = 0;	//文件系统开始块,在超级块 后的那一块
ext3_group_desc* s_blockGroupDesc;	//文件块组描述 大小为BlockGroupCount()

int ext_errno = 0;	

//初始化超级块
int InitSuperBlock()
{

	lseek(s_file, 0, SEEK_SET);
	char buffer[sizeof(ext2_super_block) * 4];
	read(s_file, buffer, sizeof(buffer));

	ext2_super_block* sb = (ext2_super_block*)buffer;
	int i;
	for(i = 0; i < 4; i++)
	{
			if(sb->s_magic == 0xEF53)
			{
					break;	
			}		
				
			sb++;				
	}
		
	if(sb->s_magic != 0xEF53)
	{
			printf("this is not a ext2/3 file system image\n");	
			return 0;
	}		
	memcpy((void*)&s_superBlock, (void*)sb, sizeof(ext2_super_block));
		
	int blockSize = BlockSize();
	s_firstDataBlock = ((int)sb - (int)buffer) / blockSize + 1;
	printf("start block is %d\n", s_firstDataBlock);
		
	return 1;	
}

//初始化inode组描述
int InitBlockGroup()
{
	char buffer[1024 * 128];
	
	int sizeOfDesc = sizeof(ext3_group_desc);
	int count = BlockGroupCount();
	int blkNum = BlockOccupied(sizeOfDesc * count);

	int blkIndexs[1024];
	int i = 0;
	for(i = 0; i < blkNum; i++)
	{
		blkIndexs[i] = s_firstDataBlock + i;
	}
	
	ReadBlocks(blkIndexs, blkNum, buffer, sizeof(buffer));
	int size = sizeof(ext3_group_desc) * count;
	s_blockGroupDesc = (ext3_group_desc*)malloc(size);
	memcpy((void*)s_blockGroupDesc, (void*)buffer, size);

	return 1;
}

ext3_group_desc* GetGroupDesc(int index)
{
	int count = BlockGroupCount();
	if(index >= count)
	{
			return NULL;	
	}
	
	return &s_blockGroupDesc[index];
}

int InitFS(char* imagefile)
{
	printf("init fs %s\n", imagefile);
	s_file = open(imagefile, 0);
	if(s_file == -1)
	{
	 	printf("can not open image file: %s \n", GET_COMMON_ERR);	
		return 0;
	}

	int len = lseek(s_file, 0, SEEK_END);
	s_imageSize = len;

	if(len > 0)
	{
		printf("image size %d \n", s_imageSize);
	}
	
	if(!InitSuperBlock())
	{
			return 0;	
	}
		
	if(!InitBlockGroup())
	{
			return 0;	
	}


	ext_errno = 0;			
	return 1;		
}

void ClearFS()
{
	close(s_file);
			
}

int BlockSize()
{
	static int blockSize = 0;
	if(!blockSize)
	{	
		ext2_super_block* sb = GetSuperBlock();
		blockSize = 1 << (sb->s_log_block_size + 10);
	}	
		
	return blockSize;
}

int INodeSize()
{
	return GetSuperBlock()->s_inode_size;
}


int BlockGroupCount()	//块组数量
{
	ext2_super_block* sb = GetSuperBlock();
	return (sb->s_blocks_count - s_firstDataBlock - 1) / sb->s_blocks_per_group + 1;
}


ext2_super_block* GetSuperBlock()
{
	return &s_superBlock;
}

int BlockOccupied(int dataSize)
{
	return dataSize / BlockSize() + 1;
}

int SeekBlock(int index, int blockSize)
{
	long long offset = (long long)index * blockSize;
	int pos = 0;

	//一般情况
	if(offset <= INT_MAX)
	{
		pos = lseek(s_file, offset, SEEK_SET);
		return pos == offset;
	}

	//处理跳转的位移大于INT_MAX 的情况
	int time = offset / INT_MAX;
	int mod = offset % INT_MAX;
	pos = lseek(s_file, 0, SEEK_SET);

	int i = 0;
	for(i = 0; i < time; i++)
	{
		pos = lseek(s_file, INT_MAX, SEEK_CUR);	
	}

	pos = lseek(s_file, mod, SEEK_CUR);		
	return 1;
	
}

int ReadBlocks(int* blkIndexs, int count, char* buffer, int bufSize)
{
	int blockSize = BlockSize();
	if(blockSize * count > bufSize)
	{
		printf("read block fatal, buffer over flow\n");
		exit(1);	
		return 0;
	}

	int i = 0;
	char* p = buffer;
	for(i = 0; i < count; i++)
	{
		int index = blkIndexs[i];
		if(!SeekBlock(index, blockSize))
		{
			printf("read block fatal, SeekBlock error\n");
			exit(1);	
		}
		
		int ret = read(s_file, p, blockSize);
		if(ret != blockSize)
		{
			printf("read block fatal\n");
			exit(1);				
		}
		
		p += blockSize;
	}
	
	
}

int ReadInode(ext3_inode_t* inode, int index)
{
	ext2_super_block* sb = GetSuperBlock();
	int inode_per_group = sb->s_inodes_per_group;
		
	int block_group = (index - 1) / inode_per_group;
	int offset = (index - 1) % inode_per_group;

	ext3_group_desc* desc = GetGroupDesc(block_group);
	if(!desc)
	{
		ext_errno = GROUP_DESC_NOT_FIND;
		return 0;
	}
	
	int node_table = desc->bg_inode_table;
	int block_offset = INodeSize() * offset / BlockSize();
	char buffer[1024 * 32];
//	memset(buffer, sizeof(buffer), 0);
	int blkIndex = node_table + block_offset;
	ReadBlocks(&blkIndex, 1, buffer, sizeof(buffer));
	int data_offset = INodeSize() * offset % BlockSize();

	memcpy(inode, buffer + data_offset, sizeof(ext3_inode_t));
	return 1;
}

