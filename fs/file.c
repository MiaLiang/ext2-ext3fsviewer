
#include <stdio.h>        
#include <stdlib.h>       
#include <string.h>   
#include "../include/ext2_head.h"
#include "BlockIO.h"
#include "file.h"
#include "dir.h"

extern int ext_errno ;	


int ReadFile(FILE_t* file, char* buffer, int len)
{
	ext3_inode_t newNode;
	if(!ReadInode(&newNode, file->inode))
	{
		ext_errno = INODE_NOT_EXIST;
		return -1;
	}
	
	int offset = file->offset;
	if(len + offset > newNode.i_size)
	{
		len = newNode.i_size - offset;
	}

	int size = (len / BlockSize() + 1) * sizeof(__u32);
	int* block = (int*)malloc(size);
	int count = 0;
	GetInodeDataBlock(&newNode, file->offset, len, block, &count);
	if(count <= 0)
	{
		ext_errno = READ_FILE_BLOCK_ERROR;	
		return -2;
	}

	//读第一个块区的数据
	int blockSize = BlockSize();
	char tmp[1024 * 16];
	ReadBlocks(block, 1, tmp, sizeof(tmp));
	char* p = buffer;
	int firstLen = blockSize - offset % blockSize;
	memcpy(p, tmp + offset % blockSize, (firstLen > len) ? len : firstLen);	
	p += blockSize - offset % blockSize;

	//读最后一个块之前的块区
	if(count > 2)
	{
		ReadBlocks(block + 1, count - 2, p, (count - 2) * blockSize);
		p += (count - 2) * blockSize;
	}

	//读最后一个区
	if(count > 1)
	{
		ReadBlocks(block + count - 1, 1, tmp, blockSize);
		memcpy(p, tmp,(offset + len) % blockSize );
	}
	
	free(block);
	return len;
}

int DirectBlockCount()
{
	return EXT3_NDIR_BLOCKS;
}

int InDirectBlockCount()
{
	return BlockSize() / sizeof(__u32);
}

int DoubleInDirectBlockCount()
{
	int num_per_block = BlockSize() / sizeof(__u32);
	return num_per_block * num_per_block;
}

int ReadDirectBlock(ext3_inode_t* inode, int* blocks, int start_block, int block_num)
{
	int index = 0;
	while(block_num > 0 && (start_block + index) < EXT3_NDIR_BLOCKS)
	{
		blocks[index] = inode->i_block[start_block + index];

		index++;
		block_num--;
	}

	return index;
}

int ReadInDirectBlock(ext3_inode_t* inode, int* blocks, int start_block, int block_num)
{
	int blockSize = BlockSize();  
	int num_per_block = blockSize / sizeof(__u32);
	int index = 0;

	int indirect_block_data[1024 * 4];
	ReadBlocks(&inode->i_block[EXT3_IND_BLOCK], 1, (char*)indirect_block_data, sizeof(indirect_block_data));

	while(block_num > 0 && (start_block + index) < EXT3_NDIR_BLOCKS + num_per_block)
	{
		int offset = start_block + index - EXT3_NDIR_BLOCKS;
		blocks[index] = indirect_block_data[offset];
		

		index++;
		block_num--;	
	}
			

	return index;
}


int ReadDoubleInDirectBlock(ext3_inode_t* inode, int* blocks, int start_block, int block_num)
{
	int blockSize = BlockSize();  
	int num_per_block = blockSize / sizeof(__u32);
	int limit = DoubleInDirectBlockCount() + InDirectBlockCount() + DirectBlockCount() - start_block;
	
	if(block_num > limit)
	{
		block_num = limit;
	}
	
	char* p = (char*)blocks;
	char* end = p + sizeof(__u32) * block_num;
	

	int double_indirect_block[1024 * 4];
	ReadBlocks(&inode->i_block[EXT3_DIND_BLOCK], 1, (char*)double_indirect_block, sizeof(double_indirect_block));

	int i = 0;	
	for(i = 0; i < block_num / num_per_block; i++)
	{
		int block_group = double_indirect_block[i];
		ReadBlocks(&block_group, 1, p, end - p);
		
		p += blockSize;
	}

	char tmp[1024 * 16];
	int mod = block_num % num_per_block;
	if(mod)
	{
		ReadBlocks(&double_indirect_block[i], 1, tmp, sizeof(tmp));
		memcpy(p, tmp, mod * sizeof(__u32));
	}

	return block_num;

}

int GetInodeDataBlock(ext3_inode_t* inode, int offset, int len, int* blocks, int* count)	//获得某文件内容偏移值所在区域
{
	int blockSize = BlockSize();  
	len += offset % BlockSize();
	*count = len / blockSize;
	if(len % blockSize)
	{
		(*count)++;
	}
	
	int start_block = offset / blockSize;
	int block_num = *count;

	//直接块区
	int ret = ReadDirectBlock(inode, blocks, start_block, block_num);
	block_num -=  ret;
	start_block += ret;
	blocks += ret;
	if(!block_num)
	{
		return 1;		//success
	}

	//一级间接块区
	ret = ReadInDirectBlock(inode, blocks, start_block, block_num);
	block_num -=  ret;
	start_block += ret;
	blocks += ret;	
	if(!block_num)
	{
		return 1;		//success
	}
	
	
	//2级间接块区
	ret = ReadDoubleInDirectBlock(inode, blocks, start_block, block_num);
	block_num -=  ret;
	start_block += ret;
	blocks += ret;	
	if(!block_num)
	{
		return 1;		//success
	}
		
	//3级间接区块 还没写
	
	printf("not write now \n");
	exit(1);
	
}



int OpenFile(FILE_t* file, char* path)
{
	DIR_t dir;
	if(!OpenDir(&dir, path))
	{
		return 0;
	}

	file->inode = dir.d_ino;
	file->offset = 0;
	file->type = dir.type;
	strcpy(file->name, dir.name);
	
	return 1;
}

int GetFileStat(int inode, ext2_fstat* stat)
{
	ext3_inode_t node;
	if(!ReadInode(&node, inode))
	{
		ext_errno = INODE_NOT_EXIST;
		return 0;
	}

	
	ext2_super_block* sb = GetSuperBlock();

	stat->st_atim = node.i_atime;
	stat->st_dev = 0;
	stat->st_ino = inode;
	stat->st_mode = node.i_mode;
	stat->st_nlink = node.i_links_count;
	stat->st_uid = node.i_uid;
	stat->st_gid = node.i_gid;
	stat->st_rdev = 0;
	stat->st_size = node.i_size;
	stat->st_blksize = BlockSize();
	stat->st_blocks = node.i_size / BlockSize() + 1;
	stat->st_mtim = node.i_mtime;
	stat->st_atim = node.i_atime;
	stat->st_ctim = node.i_ctime;
	
	return 1;
	
}


