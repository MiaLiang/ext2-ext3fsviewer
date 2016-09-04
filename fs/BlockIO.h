#ifndef BLOCK_IO
#define BLOCK_IO

#include <errno.h>

#include "ext_error.h"

int InitFS(char* imagefile);
void ClearFS();

inline ext2_super_block* GetSuperBlock();	//��ȡ������
int BlockSize();	//�ļ�ϵͳ���С
int INodeSize();
int BlockGroupCount();	//��������
ext3_group_desc* GetGroupDesc(int index);	//��ȡ��������

int BlockOccupied(int dataSize);	//����size����������ռ�ö��ٸ�block
int ReadBlocks(int* blkIndexs, int count, char* buffer, int bufSize); //��ȡcount������block���ݵ��ڴ�
int ReadInode(ext3_inode_t* inode, int index);	//���ĳ��ŵ�inode

#define GET_COMMON_ERR  (strerror(errno))

//test


#endif
