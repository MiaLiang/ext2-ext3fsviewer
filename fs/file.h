#ifndef EXT2_FILE
#define EXT2_FILE

typedef struct FILE_s {  
    int inode;                  /* inode number */  
    int offset;            /*file pointer, offset of the file*/
	int type ;
	char name[256];
	
}FILE_t;  


int OpenFile(FILE_t* file, char* path);
int ReadFile(FILE_t* file, char* buffer, int len);
int GetFileStat(int inode, ext2_fstat* stat);



#endif
