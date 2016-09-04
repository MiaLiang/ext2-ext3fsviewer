#ifndef EXT2_DIR
#define EXT2_DIR

#include "ext_error.h"

typedef struct DIR_s {  
    long d_ino;                  /* inode number */  
    off_t d_off;                  /* offset to this dirent */  
	__u8 type;		//file type
	char name[256];
}DIR_t;  



int ReadDir(DIR_t* dir, ext2_dir_entry_2* entry);
int OpenDir(DIR_t* dir, char* path);

#endif
