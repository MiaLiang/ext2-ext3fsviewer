
#include <stdio.h>        
#include <stdlib.h>       
#include <string.h>       
#include <fcntl.h>

#include "cmd/cmd.h"


int DebugFS(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("usage: check_extfs image\n");
		return 0;
	}
	char* imagepath = argv[1];
	
	if(!InitFS(imagepath))
	{

		return 0;
	}
	
	WaitCmd();


	ClearFS();
	return 1;

}

int Test(int argc, char **argv)
{
	if(!InitFS("data/image"))
	{

		return 0;
	}
	
	PrintSBInfo();
	printf("*************************\n");

	char* path = "books/sub/main.c";		
	FileStat(path);

	ClearFS();
	return 1;

}


int main(int argc, char **argv)
{
	if(1)
	{
		DebugFS(argc, argv);
	}
	else
	{
		Test(argc, argv);
	}
		
}
