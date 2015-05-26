#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>

void InitDaemon(void)
{
	pid_t pid;
	
	if((pid = fork()) < 0)
	{
	}
	else if(pid != 0)
	{
		exit(0);	
	}

	
	setsid();	
	
	if((pid = fork()) < 0)
	{
		exit(1);
	}
	else if(pid != 0)
	{
		exit(0);	
	}

	
	umask(0);
	
	int i = 0;	
	for(i = 0;i < NOFILE;i++)
	{
		close(i);
	}
	

	
	return;
}
