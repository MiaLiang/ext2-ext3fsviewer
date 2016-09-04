
#include <stdio.h>        
#include <stdlib.h>       
#include <string.h>   
#include "../include/ext2_head.h"
#include "file.h"
#include "dir.h"


extern int ext_errno ;	

int ReadDir(DIR_t* dir, ext2_dir_entry_2* entry)
{

	ext3_inode_t newNode;
	if(!ReadInode(&newNode, dir->d_ino))
	{
		ext_errno = INODE_NOT_EXIST;
		return 0;
	}

	if(dir->d_off >= newNode.i_size)
	{
		ext_errno = END_OF_DIRECTORY;
		return 0;
	}

	FILE_t file;
	file.inode = dir->d_ino;
	file.offset = dir->d_off;

	char buffer[1024 * 32];
	ReadFile(&file, buffer, sizeof(buffer));
	ext2_dir_entry_2* tmpEntry = (ext2_dir_entry_2*)(buffer);
	int len = tmpEntry->rec_len;
	if(len > sizeof(ext2_dir_entry_2))
	{
		len = sizeof(ext2_dir_entry_2);
	}
	
	memcpy(entry, tmpEntry, len);
	dir->d_off += tmpEntry->rec_len;
	

	return 1;	//success
}

//在某目录内查找文件并返回inode号
ext2_dir_entry_2* FindFileInDir(int inode, char* file)
{
	static ext2_dir_entry_2 entry;
	entry.inode = 0;
	DIR_t dir;
	dir.d_ino = inode;
	dir.d_off = 0;

	int len = strlen(file);

	while(ReadDir(&dir, &entry))
	{
		if(len == entry.name_len && strncmp(entry.name, file, entry.name_len) == 0)
		{
			return &entry;
		}
	}

	return NULL;	
}

char* PopDirectory(char** path)
{
	static char dir[1024];
	dir[0] = 0;
	char* pDir = dir;

	char* p = *path;
	if(*p == '/')
	{
		p++;
	}

	while(*p != '/' && *p != '\0')
	{
		*pDir = *p;
	
		pDir++;
		p++;
	}

	*pDir = 0;
	*path = p;
	return dir;
}

int OpenDir(DIR_t* dir, char* path)
{
	dir->d_ino = 0;
	dir->d_off = 0;

	if(strcmp(path, "/") == 0)
	{
		dir->d_ino = ROOT_INODE;
		dir->d_off = 0;
		strcpy(dir->name, "/");
		dir->type = EXT2_FT_DIR;
		return 1;
	}

	char** p = &path;
	int inode = ROOT_INODE;
	ext2_dir_entry_2* entry = NULL;
	while(1)
	{
		char* dir_name = PopDirectory(p);
		if(!dir_name || strcmp(dir_name, "") == 0)
		{
			break;
		}
		
		entry = FindFileInDir(inode, dir_name);
		if(!entry)
		{
			ext_errno = DIR_NOT_EXIST;
			return 0;
		}
		inode = entry->inode;
	
	}

	
	dir->d_ino = inode;
	dir->d_off = 0;
	dir->type = entry->file_type;
	if(entry->name_len > EXT2_NAME_LEN)
	{
		ext_errno = FILE_NAME_ERROR;		
		return 0;
	}
	
	memcpy(dir->name, entry->name, entry->name_len);
	dir->name[entry->name_len] = '\0';
	
	return 1;
}



