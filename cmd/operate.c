
#include <stdio.h>        
#include <stdlib.h>       
#include <string.h>       
#include <fcntl.h>
#include <time.h>
#include "../include/ext2_head.h"
#include "../fs/BlockIO.h"
#include "../fs/dir.h"
#include "../fs/file.h"

#include "cmd.h"
#include "operate.h"

char g_pwd[2048];

char* FileType(int type)
{
	static char type_str[128];
	
	switch(type)
	{
		case EXT2_FT_REG_FILE:
			strcpy(type_str, "FILE");
			break;

		case EXT2_FT_DIR:
			strcpy(type_str, "DIR");
			break;

		case EXT2_FT_CHRDEV:
			strcpy(type_str, "CHRDEV");
			break;

		case EXT2_FT_BLKDEV:
			strcpy(type_str, "BLKDEV");
			break;

		case EXT2_FT_FIFO:
			strcpy(type_str, "FIFO");
			break;

		case EXT2_FT_SOCK:
			strcpy(type_str, "SOCK");
			break;

		case EXT2_FT_SYMLINK:
			strcpy(type_str, "LINK");
			break;

		default:
			strcpy(type_str, "UNKNOW");
			break;

	}

	return type_str;
}

char* FileMode(int mode)
{
	static char mode_str[32];

	memset(mode_str, '-', 10);

	if(S_ISLNK(mode))
	{
		mode_str[0] = 'l';
	}
	else if(S_ISREG(mode))
	{
		mode_str[0] = '-';
	}
	else if(S_ISDIR(mode))
	{
		mode_str[0] = 'd';
	}
	else if(S_ISBLK(mode))
	{
		mode_str[0] = 'b';
	}
	else if(S_ISCHR(mode))
	{
		mode_str[0] = 'c';
	}
	

	if(mode & S_IRUSR)
	{
		mode_str[1] = 'r';
	}

	if(mode & S_IWUSR)
	{
		mode_str[2] = 'w';
	}

	if(mode & S_IXUSR)
	{
		mode_str[3] = 'x';
	}

	if(mode & S_IRGRP)
	{
		mode_str[4] = 'r';
	}

	if(mode & S_IWGRP)
	{
		mode_str[5] = 'w';
	}

	if(mode & S_IXGRP)
	{
		mode_str[6] = 'x';
	}

	if(mode & S_IROTH)
	{
		mode_str[7] = 'r';
	}

	if(mode & S_IWOTH)
	{
		mode_str[8] = 'w';
	}

	if(mode & S_IXOTH)
	{
		mode_str[9] = 'x';
	}

		
	mode_str[10] = '\0';
	return mode_str;
}

void PrintSBInfo()
{
	ext2_super_block* sb = GetSuperBlock();
	printf("\n");
	printf("inode count: %d \n", sb->s_inodes_count);
	printf("block count: %d \n", sb->s_blocks_count);
	printf("reserver block count: %d \n", sb->s_r_blocks_count);
	printf("free block count: %d \n", sb->s_free_blocks_count);
	printf("free inode count: %d \n", sb->s_free_inodes_count);
	printf("block size: %d \n", BlockSize());
	printf("block per group: %d \n", sb->s_blocks_per_group);
	printf("inode per group: %d \n", sb->s_inodes_per_group);	
	printf("block group count: %d \n", BlockGroupCount());
	printf("first data block: %d \n", sb->s_first_data_block);
	time_t time = sb->s_mtime;
	char* timeStr = asctime(gmtime(&time));
	printf("mount time: %s", timeStr);
	
	time = sb->s_wtime;
	timeStr = asctime(gmtime(&time));
	printf("write time: %s", timeStr);	
	time = sb->s_lastcheck;
	timeStr = asctime(gmtime(&time));
	printf("last check time: %s", timeStr);

	printf("first inode: %d\n", sb->s_first_ino);
	printf("inode size: %d\n", sb->s_inode_size);
	printf("block group of super block: %d\n", sb->s_block_group_nr);
	char uuid[17];
	memcpy(uuid, sb->s_uuid, 16);
	uuid[16] = '\0';
	printf("uuid: %s\n", uuid);

	char volume_name[17];
	memcpy(volume_name, sb->s_volume_name, 16);
	volume_name[16] = '\0';
	printf("volume name: %s\n", volume_name);	
	
	char last_mount_dir[128];
	memcpy(last_mount_dir, sb->s_last_mounted, 64);
	last_mount_dir[64] = '\0';
	printf("last mounted dir: %s\n", last_mount_dir);	
	
	
	printf("%d percent used\n",(sb->s_blocks_count - sb->s_free_blocks_count) * 100 / 
		(sb->s_blocks_count));
	printf("\n");
		
}

void PrintGroup(int index)
{
	ext3_group_desc* desc = GetGroupDesc(index);
	if(!desc)
	{
		printf("group desc %d not exist\n", index);
		return;
	}
	
	printf("group desc %d: \n", index);
	printf("  block bitmap: %d\n", desc->bg_block_bitmap);
	printf("  inode bitmap: %d\n", desc->bg_inode_bitmap);
	printf("  inode table: %d\n", desc->bg_inode_table);		
	printf("  free block count: %d\n", desc->bg_free_blocks_count);
	printf("  free inode count: %d\n", desc->bg_free_inodes_count);
	printf("  used dir count: %d\n", desc->bg_used_dirs_count);

}

void PrintGroupsInfo(char* param1, char* param2)
{
	int count = BlockGroupCount();
	int i = 0;

	int start = 0;
	int end = count - 1;

	if(param1 != NULL)
	{
		start = atoi(param1);
	}

	//ŽòÓ¡ÌØ¶š×éÃèÊö
	if(param1 != NULL && param2 == NULL)
	{
		PrintGroup(start);
		return;
	}
	else if(param1 != NULL && param2 != NULL)
	{
		end = atoi(param2);
	}
	
	if(end <= start)
	{
		printf("end group index must be bigger than start group index \n");
		return;
	}

	if(start >= count - 1)
	{
		printf("start index too big \n");
		return;
	}

	if(end >= count)
	{
		printf("end index too big \n");
		return;
	}

	printf("\n");
	
	for(i = start; i <= end; i++)
	{
		PrintGroup(i);
	}

	printf("\n");

}

void PrintInode(char* param2)
{
	if(!param2)
	{
		printf("usage: info inode index\n");
		return;
	}

	int inode = atoi(param2);

	ext3_inode_t node;
	memset(&node, 0, sizeof(ext3_inode_t));
	if(!ReadInode(&node, inode))
	{
		printf("inode %d not exist !\n", inode);
		return ;
	}
	
	printf("inode %d:\n", inode);
	printf("  mode: %s\n", FileMode(node.i_mode));
	printf("  uid: %d\n", node.i_uid);
	printf("  gid: %d\n", node.i_gid);
	printf("  file size: %d\n", node.i_size);
	printf("  link count: %d\n", node.i_links_count);
	printf("  blocks: %d\n", node.i_blocks);
	printf("  file flag: %d\n", node.i_flags);
	
	time_t time = node.i_atime;
	char* timeStr = asctime(gmtime(&time));
	printf("  access time: %s", timeStr);

	time = node.i_ctime;
	timeStr = asctime(gmtime(&time));
	printf("  create time: %s", timeStr);

	time = node.i_mtime;
	timeStr = asctime(gmtime(&time));
	printf("  modify time: %s", timeStr);

	time = node.i_dtime;
	timeStr = asctime(gmtime(&time));
	printf("  delete time: %s", timeStr);

	printf("  first 8 block:\n");
	int i = 0;
	for(i = 0; i < 8; i++)
	{
		printf("    %d\n", node.i_block[i]);
	}
	
}


int ExportFile(char* filepath)
{
	FILE_t file;
	if(!OpenFile(&file, filepath))
	{
		printf("can not open file\n");
		return 0;
	}

	ext2_fstat fstat;
	if(!GetFileStat(file.inode, &fstat))
	{
		printf("can not get file state\n");
		return 0;
	}

	file.offset = 0;
	int max_size = fstat.st_size;
	
	char* buffer = (char*)malloc(max_size);
	int ret = ReadFile(&file, buffer, max_size);
//	printf("%d readed\n", ret);

	int newfile = open("export_file", O_RDWR | O_CREAT | O_TRUNC, 0777);
	if(!newfile)
	{
		printf("can not create export file: %s \n", GET_COMMON_ERR);
		return 0;
	}

	write(newfile, buffer, ret);
	close(newfile);
	free(buffer);

	return ret;
	
}

void FileStat(char* filepath)
{
	FILE_t file;
	if(!OpenFile(&file, filepath))
	{
		printf("can not open file\n");
		return;
	}

	ext2_fstat fstat;
	if(!GetFileStat(file.inode, &fstat))
	{
		printf("can not get file stat\n");
		return;

	}

	printf("file stat:\n");
	printf("size : %d\n", fstat.st_size);
	printf("blocks : %d\n", fstat.st_blocks);
	
	
}

void PrintlsHead(char* dir)
{
	printf("\n%s\n", dir);
	printf("----------------------------------------------------------\n");	
	printf("   mode     l     inode           size            name\n");
	printf("----------------------------------------------------------\n");
}

void PrintFileInLine(char* filename, ext2_fstat* fstat)
{
	char info[512];
	char tmp[64];
	
	memset(info, ' ', sizeof(info));

	char* p = info;
	const char* str = FileMode(fstat->st_mode);
	memcpy(p, str, strlen(str));
	p += 12;

	sprintf(tmp, "%d", fstat->st_nlink);
	memcpy(p, tmp, strlen(tmp));
	p += 6;

	sprintf(tmp, "%d", fstat->st_ino);
	memcpy(p, tmp, strlen(tmp));
	p += 16;

	sprintf(tmp, "%d", fstat->st_size);
	memcpy(p, tmp, strlen(tmp));
	p += 16;

	
	memcpy(p, filename, strlen(filename));
	p += 30;

	*p++ = '\n';
	*p = '\0';

	printf("%s", info);
}

void PrintHelp()
{
	printf("\n    check_extfs version 0.5\n"
			"    This is a small program which can show base info of a ext2\\ext3 file system,\n"
			"  such as print out super block, directory, file info, inode info. \n"
			"  Type `help name' to find out more about the cmd `name'.\n\n"
							);

	printf("  info [sb|superblock|groups|inode] \n");
	printf("  ls [dir] \n");
	printf("  export [filepath] \n");
	printf("  filestat [filepath] \n");
	printf("  cd [filepath] \n");
	printf("  pwd \n");
	printf("  quit\n");
	
	printf("\n");
}

void PrintHelpInfo()
{
	printf("\n");
	printf("  print out base info of the file system:\n\n");

	printf("    info inode [num] : print out the inode \n");
	printf("    info groups : print out all the inode group desc \n");
	printf("    info groups [index]: print out the inode group desc of specify index\n");
	printf("    info groups [start_idx] [end_idx]: print out the inode group desc from start index to end index\n");
	printf("    info [sb|superblock] : print out superblock of the filesystem\n");


}

void PrintHelpCd()
{
	printf("\n");
	printf("  change current directory by path:\n\n");

	printf("    cd [path]\n\n");

}

void PrintHelpPwd()
{
	printf("\n");
	printf("  print out current directory \n\n");

	printf("    pwd\n\n");

}

void PrintHelpls()
{
	printf("\n");
	printf("  print out content of a directory specify by path:\n\n");

	printf("    ls [path]\n\n");
}

void PrintHelpExport()
{
	printf("\n");
	printf("  export specify file content into your locale dir, name as export_file:\n\n");

	printf("    export [filepath]\n\n");
}

void PrintHelpFileStat()
{
	printf("\n");
	printf("  print out status of a file:\n\n");

	printf("    filestat [filepath]\n\n");
}

void GetAbsolutePath(char* pwd, char* inputPath)	//»ñÈ¡Ÿø¶ÔÂ·Ÿ¶
{
	if(!inputPath)
	{
		strcpy(pwd, g_pwd);
		return;
	}

	if(inputPath[0] == '/')
	{
		strcpy(pwd, inputPath);
		return;
	}

	int len = strlen(inputPath);
	sprintf(pwd, "%s%s", g_pwd, inputPath);

	
}

/* tree开始 */
void PrintTree(char* filename, ext2_fstat* fstat, int depth)
{
	char info[512];
	char tmp[64];
	
	memset(info, ' ', sizeof(info));

	char* p = info;

	memcpy(p, filename, strlen(filename));
	p += 30;

	*p = '\0';

	 printf("%s%*s%s/\n","|",depth,"|--",info);
	
	
		
}
/* tree结束 */
