#ifndef BLOCK_IO
#define BLOCK_IO

#include <errno.h>

#include "ext_error.h"

int InitFS(char* imagefile);
void ClearFS();

inline ext2_super_block* GetSuperBlock();	//获取超级块
int BlockSize();	//文件系统块大小
int INodeSize();
int BlockGroupCount();	//块组数量
ext3_group_desc* GetGroupDesc(int index);	//获取块组描述

int BlockOccupied(int dataSize);	//计算size的数据量下占用多少个block
int ReadBlocks(int* blkIndexs, int count, char* buffer, int bufSize); //读取count数量的block数据到内存
int ReadInode(ext3_inode_t* inode, int index);	//获得某序号的inode

#define GET_COMMON_ERR  (strerror(errno))

//test


#endif
