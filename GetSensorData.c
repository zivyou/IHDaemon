
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <termios.h>
#include "Data.h"

#define MODEMDEVICE "/dev/ttyS1"


static int fdttyS1;
static struct termios oldtio, newtio;


int init_ttyS1()
{
	fdttyS1 = open(MODEMDEVICE, O_RDWR | O_NOCTTY);
	
	if (fdttyS1 <0) {perror(MODEMDEVICE); return -1; }

	tcgetattr(fdttyS1,&oldtio); /* save current serial port settings */
	bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
	newtio.c_cflag = B57600 |  CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR | ICRNL;
	newtio.c_oflag = 0;
	newtio.c_lflag = ~(ICANON | ECHO | ECHOE | ISIG);
	newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */
	newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
	newtio.c_cc[VERASE]   = 0;     /* del */
	newtio.c_cc[VKILL]    = 0;     /* @ */
	newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
	newtio.c_cc[VTIME]    = 1;     /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 23;     /* blocking read until n character arrives */
	newtio.c_cc[VSWTC]    = 0;     /* '\0' */
	newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */
	newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
	newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
	newtio.c_cc[VEOL]     = 0;     /* '\0' */
	newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
	newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
	newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
	newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
	newtio.c_cc[VEOL2]    = 0;     /* '\0' */

	tcflush(fdttyS1, TCIFLUSH);
	tcsetattr(fdttyS1,TCSANOW,&newtio);
	
	return 1;
}

int readttyS1(struct g_Sensor_status *sstatus)
{
	int cnt,i,j;
	char rbuf[50];
	char temp[50];

	cnt = read(fdttyS1,rbuf,34);	
	
	if (cnt == 0){
		return 0;
		
	}
	
	else if(cnt<=33) {
		if(rbuf[0]!=0x7E)  return -2;	
		if(rbuf[5]!=0x27) return -4;	
		for(i=0;i<cnt;i++) {
			if(rbuf[i]==0x7E && rbuf[i+1]==0x42 && rbuf[i+9]==0x01){
			 // && rbuf[i+12]==0x05) { // header, protocol, cmd, sub
				sstatus->id = rbuf[i+7];
				sstatus->cmd= rbuf[i+9];
				sstatus->illumi = rbuf[i+12];
				sstatus->temp = rbuf[i+10];
				sstatus->humid = rbuf[i+11];
				
				for(j = 0;j < 8;j++)
					sstatus->rfid[j+i] = rbuf[16+j+i];
				/*	
				FILE * f = fopen("2rfid.txt","w+");			
				for(j = 0;j < 24;j++)
					fprintf(f,"%x ",rbuf[j]);
				fprintf(f,"\n");
				fclose(f); 
				*/				
				return 1;
			}
		}
	}	
	return 0;
}




void GetData(g_Sensor_status * sstatus)
{
	//struct g_Sensor_status sstatus;
	init_ttyS1();	
	readttyS1(sstatus);	
	return ;
}
