#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#define NONE "0"
#define FILE_ERROR "1"
#define SENSOR_TIMES 10
#define charsize 2

#define SIG_AUTO	100
#define SIG_MANU	101
#define SIG_GET		102
#define SIG_SETTHR	103
#define SIG_SETSEN	104

#define OPEN	0
#define CLOSE	1

#define AUTO	0
#define MANU	1

int serverPid = 0;

void sent_threshold(unsigned char *threshold)
{	//设置阈值
	int re = 0;	
	re = mkfifo("serverfifo",0666 );
	if(re < 0) printf("serverfifo created error!\n");
	else printf("serverfifo created ok!\n");
	int sf = open("serverfifo",O_WRONLY,0);
	//int sf = open("serverfifo",O_RDWR,0);
	if(sf < 0) printf("serverfifo open error!\n");
	else printf("serverfifo open ok...\n");
	write(sf,threshold,6);
	kill(serverPid,SIG_SETTHR);	//切换为手动状态
	close(sf);
}
void get_threshold()
{	//获得当前设定的阈值的值
	unsigned char buf[15];
	memset(buf,0,15);

	kill(serverPid,SIG_GET);
	sleep(1);
	int cf = open("clientfifo",O_RDONLY,0);	
	//int cf = open("clientfifo",O_RDWR,0);	
	if(cf < 0) printf("serverfifo open error!\n");
	else printf("serverfifo open ok...\n");

	int n = read(cf,buf,13);
	close(cf);
	unlink("clientfifo");
}
void set_sensor(unsigned char ch[2])
{	//设定传感器的值
	int re = 0;	
	re = mkfifo("serverfifo",0666 );
	if(re < 0) printf("serverfifo created error!\n");
	else printf("serverfifo created ok!\n");
	int sf = open("serverfifo",O_WRONLY,0);
	//int sf = open("serverfifo",O_RDWR,0);
	if(sf < 0) printf("serverfifo open error!\n");
	else printf("serverfifo open ok...\n");

	write(sf,ch,2);
	kill(serverPid,SIG_SETSEN);
	close(sf);
}

void set_auto()
{//设为自动控制状态
	kill(serverPid,SIG_AUTO);
}
void set_manu()
{	//设为手动控制状态
	kill(serverPid,SIG_MANU);
}
void get_nowsensor()
{	//获取当前的传感器信息
	kill(serverPid,SIG_GET);
	sleep(1);
	unsigned char buf[15];
	memset(buf,0,15);
	int cf = open("clientfifo",O_RDONLY,0);	
	//int cf = open("clientfifo",O_RDONLY,0);	
	if(cf < 0) printf("clientfifo open error!\n");
	else printf("clientfifo open ok...\n");
	int n = read(cf,buf,13);
	close(cf);
	unlink("clientfifo");
}
int main()
{
	//获取守护进程的pid
	FILE * ft = fopen("PidStore.txt","r");
	fscanf(ft,"%d",&serverPid);
	fclose(ft);
	printf("serverpid=%d",serverPid);

	umask(0);

	unsigned char ch[] = {60,01};

	unsigned char threshold[] = {23,30,30,60,23,150};

	int input = 0;
	
	while(1)
	{
		printf(":");
		scanf("%d",&input);
		switch(input)
		{
			case 1:
				set_auto();
				break;
			case 2:
				set_manu();
				break;
			case 3:
				get_nowsensor();
				break;
			case 4:
				 set_sensor(ch);
				break;
			case 5:
				sent_threshold(threshold);
				break;
			default:
				break;
		}
	}
	return 0;
}
