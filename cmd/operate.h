#ifndef _OPERATE_
#define _OPERATE_




char* FileType(int type);
char* FileMode(int mode);

void PrintSBInfo();
void PrintGroup(int index);

void PrintGroupsInfo(char* param1, char* param2);
void PrintInode(char* param2);

int ExportFile(char* filepath);
void FileStat(char* filepath);
void PrintlsHead();
void PrintFileInLine(char* filename, ext2_fstat* fstat);

void PrintHelp();
void PrintHelpInfo();
void PrintHelpls();
void PrintHelpFileStat();

void GetAbsolutePath(char* pwd, char* inputPath);	//获取绝对路径
#endif



