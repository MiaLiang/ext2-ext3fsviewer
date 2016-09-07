
#include <stdio.h>        
#include <stdlib.h>       
#include <string.h>       
#include <fcntl.h>
#include <time.h>
#include "../include/ext2_head.h"
#include "../fs/BlockIO.h"
#include "../fs/dir.h"
#include "../fs/file.h"
#include "operate.h"
#include "cmd.h"

#define MAX_PARAMS 40

static int s_params_count = 0;
static char s_params[MAX_PARAMS][1024];
extern char g_pwd[2048];

int i = 0;
int k = 0;

int ParseCmd(char* cmd)
{
	int len = strlen(cmd);
	int i = 0;
	s_params_count = 0;

	while(i < len)
	{
		char* p = &s_params[s_params_count][0];
		while(cmd[i] != ' ')
		{
			*p = cmd[i];
			i++;
			p++;
		}
		*p = '\0';

		while(cmd[i] == ' ')
		{
			i++;
		}
		
		s_params_count++;
		
	}

}

int StrCaseCmp(char* source, char* str)
{
	if(!source || !str)
	{
		return 0;
	}

	return strcasecmp(source, str) == 0;
}

int IsQuitCmd()
{
	if(StrCaseCmp(s_params[0], "quit"))
	{
		return 1;
	}

	if(StrCaseCmp(s_params[0], "exit"))
	{
		return 1;
	}

	if(StrCaseCmp(s_params[0], "q"))
	{
		return 1;
	}

	
	return 0;
}

void CmdInfo(char* param1, char* param2, char* param3, char* param4)
{
	if(StrCaseCmp(param1, "sb") || StrCaseCmp(param1, "superblock"))
	{
		PrintSBInfo();
	}
	else if(StrCaseCmp(param1, "groups"))
	{
		PrintGroupsInfo(param2, param3);
	}
	else if(StrCaseCmp(param1, "inode"))
	{
		PrintInode(param2);
	}
	else 
	{
		printf("invalid info cmd. for help pls type 'help info'\n");
	}
	
}

void CmdExportFile(char* param1)
{
	if(param1 == NULL)
	{
		printf("export file without filename\n");
		return ;
	}	

	char path[2048];
	GetAbsolutePath(path, param1);

	int ret = ExportFile(path);
	if(ret == 0)
	{
		printf("export file failed\n");
		return;
	}

	printf("export to local file 'export_file' success, pls check.\n");
	printf("%d bytes write\n", ret);
	
}

void CmdFileState(char* param1)
{
	if(param1 == NULL)
	{
		printf("print file stat without filename\n");
		return ;
	}	

	char path[2048];
	GetAbsolutePath(path, param1);


	FILE_t file;
	if(!OpenFile(&file, path))
	{
		printf("can not open file %s\n", path);
		return ;
	}

	ext2_fstat fstat;
	if(!GetFileStat(file.inode, &fstat))
	{
		printf("can not get file stat\n");
		return ;
	}

	printf("\nfile stat of %s \n", path);
	printf("  type : %s\n", FileType(file.type));
	printf("  inode : %d\n", fstat.st_ino);	
	printf("  mode : %s\n", FileMode(fstat.st_mode));	
	printf("  link : %d\n", fstat.st_nlink);	
	printf("  uid : %d\n", fstat.st_uid);	
	printf("  gid : %d\n", fstat.st_gid);		
	printf("  size : %d\n", fstat.st_size);
	printf("  blocks : %d\n", fstat.st_blocks);

	time_t time = fstat.st_atim;
	char* timeStr = asctime(gmtime(&time));
	printf("  access time: %s", timeStr);

	time = fstat.st_mtim;
	timeStr = asctime(gmtime(&time));
	printf("  modify time: %s", timeStr);

	time = fstat.st_ctim;
	timeStr = asctime(gmtime(&time));
	printf("  create time: %s", timeStr);
	
	printf("\n");
	
}

void CmdListDir(DIR_t* dir, char* dirpath)
{
	PrintlsHead(dirpath);
	
	ext2_dir_entry_2 entry;

	char name[256];

	while(ReadDir(dir, &entry))
	{
		ext2_fstat fstat;
		if(!GetFileStat(entry.inode, &fstat))
		{
			continue;
		}

		memcpy(name, entry.name, entry.name_len);
		name[entry.name_len] = '\0';
		PrintFileInLine(name, &fstat);	
		
	}

	printf("\n");
	
}

void CmdList(char* param1)
{

	char path[2048];
	GetAbsolutePath(path, param1);

	DIR_t dir;
	if(!OpenDir(&dir, path))
	{
		printf("can not open path %s\n", path);
		return ;
	}

	if(dir.type == EXT2_FT_DIR)
	{
		CmdListDir(&dir, path);
	}
	else
	{
		ext2_fstat fstat;
		if(!GetFileStat(dir.d_ino, &fstat))
		{
			printf("can not get file stat\n");
			return ;
		}

		PrintlsHead();
		PrintFileInLine(dir.name, &fstat);
		printf("\n");

	}

	
}

void CmdChangeDirDown(char* param1)
{
	char path[2048];
	GetAbsolutePath(path, param1);

	DIR_t dir;
	if(!OpenDir(&dir, path))
	{
		printf("no such directory: %s\n\n", param1);
		return;
	}
	
	if(dir.type != EXT2_FT_DIR)
	{
		printf("%s is not a directory \n\n", param1);
		return;
	}

	strcpy(g_pwd, path);
	if(param1[strlen(param1) - 1] != '/')
	{
		strcat(g_pwd, "/");
	}

}

void CmdChangeDirUp()
{
	if(strcmp(g_pwd, "/") == 0)
	{
		return;
	}

	int len = strlen(g_pwd) - 2;
	for(; len >= 0; len--)
	{
		if(g_pwd[len] == '/')
		{
			g_pwd[len + 1] = '\0';
			break;
		}
	}
	
}

void CmdChangeDir(char* param1)
{
	if(!param1)
	{
		printf("%s/n", g_pwd);
		return;
	}

	if(strcmp(param1, "..") == 0)
	{
		CmdChangeDirUp();
	}
	else
	{
		CmdChangeDirDown(param1);
	}	
}


void CmdHelp(char* param1)
{
	if(!param1)
	{
		PrintHelp();
		return;
	}

	if(StrCaseCmp(param1, "ls"))
	{
		PrintHelpls();
	}
	else if(StrCaseCmp(param1, "export"))
	{
		PrintHelpExport();
	}
	else if(StrCaseCmp(param1, "filestat"))
	{
		PrintHelpFileStat();
	}
	else if(StrCaseCmp(param1, "info"))
	{
		PrintHelpInfo();
	}
	else if(StrCaseCmp(param1, "cd"))
	{
		PrintHelpCd();
	}
	else if(StrCaseCmp(param1, "pwd"))
	{
		PrintHelpPwd();
	}	
	else
	{
		printf("%s is not a valid cmd\n", param1);
	}

}

void ExeCmd()
{
	char* params[MAX_PARAMS];
	memset(params, 0, sizeof(params));

	if(!s_params_count)
	{
		return;
	}
	
	int i = 0;
	for(i = 0; i < s_params_count; i++)
	{
		params[i] = s_params[i];
	}
	
	if(StrCaseCmp(s_params[0], "info"))
	{
		CmdInfo(params[1], params[2], params[3], params[4]);
	}
	else if(StrCaseCmp(s_params[0], "export"))
	{
		CmdExportFile(params[1]);
	}
	else if(StrCaseCmp(s_params[0], "filestat"))
	{
		CmdFileState(params[1]);
	}	
	else if(StrCaseCmp(s_params[0], "ls"))
	{
		CmdList(params[1]);
	}
	else if(StrCaseCmp(s_params[0], "pwd"))
	{
		printf("%s\n\n", g_pwd);
	}
	else if(StrCaseCmp(s_params[0], "cd"))
	{
		CmdChangeDir(params[1]);
	}	
	else if(StrCaseCmp(s_params[0], "help"))
	{
		CmdHelp(params[1]);
	}
	else if(StrCaseCmp(s_params[0], "tree"))
	{
		CmdTree(params[1]);
	}
	else
	{
		printf("invaild cmd\n");
		printf("you can input: help\n\n");		
	}
	
}

void WaitCmd()
{
	char cmd[1024 * 16];
	strcpy(g_pwd, "/");
	
	printf("input cmd:\n\n");

	while(1)
	{
		printf("> ");
		gets(cmd);
		ParseCmd(cmd);	

		if(IsQuitCmd())
		{
			break;
		}

		ExeCmd();	
	}


}

/* tree开始 */
void CmdTree(char* param1){
	char path[2048];
	GetAbsolutePath(path, param1);

	DIR_t dir;
	if(!OpenDir(&dir, path))
	{
		printf("can not open path %s\n", path);
		return ;
	}

	if(dir.type == EXT2_FT_DIR)
	{ 
		CmdListTree(&dir, path,0);
		
	}
	
}

void CmdListTree(DIR_t* dir, char* dirpath, int depth)
{	
	i++;
	ext2_dir_entry_2 entry;
	
	char path[1024];//新路径
	char pat[1024];	
	char name[256];
	
	while(ReadDir(dir, &entry))
	{
		ext2_fstat fstat;
		if(!GetFileStat(entry.inode, &fstat))
		{
			continue;
		}

		memcpy(name, entry.name, entry.name_len);
		name[entry.name_len] = '\0';
		
		if(S_ISDIR(fstat.st_mode)&&(!StrCaseCmp(name, ".."))&&(!StrCaseCmp(name, "."))){
			memset(path,0,1024);
			strcpy(path, dirpath);
			strcat(path,name);
			strcat(path,"/");

			DIR_t dirr;
			if(!OpenDir(&dirr, path))
			{
				printf("can not open path %s\n", path);
				return ;
			}

			if(dirr.type == EXT2_FT_DIR)
			{ 
				printf("%s%*s%s/\n","|",depth,"|--",name);
				CmdListTree(&dirr, path, depth+7);
		
			}
			
			
		} else{
			PrintTree(name, &fstat, depth);	
		}
		
		
	}

	printf("\n");
	
	
}

/* tree结束 */

