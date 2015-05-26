#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <malloc.h>
#include "Data.h"
int sensorNum = 0;



Sensor * s[4];

int ctlSta = AUTO;
Env * env;

void InitDaemon(void);
void GetData(g_Sensor_status * sstatus);
void AutoControl(g_Sensor_status * sstatus,Env * env,Sensor * s[4]);

void Action(int sign)
{
	if(sign == SIG_SET)
	{
		int staT = ctlSta;
		ctlSta = MANU;
		
		/*	serverfifo应由client创建
		int re = 0;	
		re = mkfifo("serverfifo",0666 );
		if(re == 0) 
			fputs("serverfifo mkfifo success\n",flog);
		else 
			fputs("serverfifo mkfifo error!\n",flog);
		*/
		FILE * sf ;FILE * cf ;
		int count = 0;
		char buf[20];
		//struct Sensor * s;
	
		memset(buf,0,20);
 		sf = fopen("/web/serverfifo","r+");	
		if(sf < 0)
		{
			FILE * flog = fopen("/web/AutoMachine.log","ab+");
			fputs("fifo open error!\n",flog);	
			fclose(flog);
		}
		else
		{
			FILE * flog = fopen("/web/AutoMachine.log","ab+");
			fputs("fifo open ok\n",flog);
			fclose(flog);
		}
		count = fread(buf,1,20,sf);

		if(count == 0)
		{	
			FILE * flog = fopen("/web/AutoMachine.log","ab+");
			fputs("fifo read error\n",flog);
			fclose(flog);
		}
		else
		{
			FILE * flog = fopen("/web/AutoMachine.log","ab+");
			int i = 0;			
			for(i = 0;i < count;i++)
				fprintf(flog,"%x ",buf[i]);
			fprintf(flog,"notice---------------------------------------\n");
			fclose(flog);
			switch(buf[0])
			{
				case FAN:
					if((buf[1] == OPEN) && (s[2]->relaySta == CLOSE))
					{
						system("/web/zigbex_tosRF_Ubi 60 01");
						s[2]->relaySta = OPEN;
					}
					else if((buf[1] == CLOSE) && (s[2]->relaySta == OPEN))
					{
						system("/web/zigbex_tosRF_Ubi 60 02");
						s[2]->relaySta = CLOSE;
					}
					break;
				case ROLLING:
					if((buf[1] == OPEN) && (s[0]->relaySta == CLOSE))
					{
						system("/web/zigbex_tosRF_Ubi 61 01");
						s[0]->relaySta = OPEN;
					}
					else if((buf[1] == CLOSE) && (s[0]->relaySta == OPEN))
					{
						system("/web/zigbex_tosRF_Ubi 61 02");
						s[0]->relaySta = CLOSE;
					}
					break;
				case HUMIDIFIER:
					if((buf[1] == OPEN) && (s[3]->relaySta == CLOSE))
					{
						system("/web/zigbex_tosRF_Ubi 62 01");
						s[3]->relaySta = OPEN;
					}
					else if((buf[1] == CLOSE) && (s[3]->relaySta == OPEN))
					{
						system("/web/zigbex_tosRF_Ubi 62 02");
						s[3]->relaySta = CLOSE;
					}
					break;
				case DOOR:
					if((buf[1] == OPEN) && (s[1]->relaySta == CLOSE))
					{
						system("/web/zigbex_tosRF_Ubi 63 01");
						s[1]->relaySta = OPEN;
					}
					else if((buf[1] == CLOSE) && (s[1]->relaySta == OPEN))
					{
						system("/web/zigbex_tosRF_Ubi 63 02");
						s[1]->relaySta = CLOSE;
					}
					break;
				case CTLSTA:
					staT = buf[1];
					break;
				case SETTHR:
					env->thres.tempH = buf[1];
					env->thres.tempL = buf[2];
					env->thres.humidH = buf[3];
					env->thres.humidL = buf[4];
					env->thres.illumiH = buf[5];
					env->thres.illumiL = buf[6];		
					break;
				default:
					break;			
			}
		}

		fclose(sf);
		sf = NULL;
		//unlink("/web/serverfifo");
		ctlSta = staT;
		return;
	}
	if(sign == SIG_GET)
	{
		int staT = ctlSta;
		FILE * cf;
		ctlSta = MANU;

		cf = fopen("/web/clientfifo","w+");	
		if(cf < 0)
		{	
			FILE * flog = fopen("AutoMachine.log","ab+");
			fputs("clientfifo open error!\n",flog);
			fclose(flog);	
		}
		else
		{
			FILE * flog = fopen("AutoMachine.log","ab+");
			fputs("clientfifo open ok\n",flog);
			fclose(flog);
			char wbuf[14];
			memset(wbuf,0,14);
			wbuf[0] = env->temp;
			wbuf[1] = env->humid;
			wbuf[2] = env->illumi;

			wbuf[3] = s[2]->relaySta;
			wbuf[4] = s[3]->relaySta;
			wbuf[5] = s[0]->relaySta;
			wbuf[6] = s[1]->relaySta;

			wbuf[7] = env->thres.tempH;
			wbuf[8] = env->thres.tempL;
			wbuf[9] = env->thres.humidH;
			wbuf[10] = env->thres.humidL;
			wbuf[11] = env->thres.illumiH;			
			wbuf[12] = env->thres.illumiL;

			wbuf[13] = staT;		
			int count = fwrite(wbuf,1,14,cf);
			if(count >= 14) 
			{	
				FILE * flog = fopen("AutoMachine.log","ab+");			
				fputs("clientfifo write ok!\n",flog);
				fclose(flog);
			}			
			else 
			{	FILE * flog = fopen("AutoMachine.log","ab+");
				fputs("clientfifo write error!\n",flog);
				fclose(flog);
			}		
		}
		int re = fclose(cf);
		if(re == -1)
		{
				FILE * flog = fopen("AutoMachine.log","ab+");
				fputs("clientfifo close error!\n",flog);
				fclose(flog);
		}
		else if(re == 0)
		{
				FILE * flog = fopen("AutoMachine.log","ab+");
				fputs("clientfifo close ok!\n",flog);
				fclose(flog);
		}
		cf = NULL;
		ctlSta = staT;//恢复现场
		return;
	}

}

void WriteFile(char * name,Env * env)
{
	FILE * f = fopen(name,"ab+");
	Exchange * e = (Exchange *) malloc(sizeof(Exchange));

	time_t now;struct tm *timenow;
	time(&now);
	timenow = localtime(&now);
	long re = 0.0;long mi = timenow->tm_min/60.0;
	if(mi < 0.5) mi = 0.0;
	else mi = 0.5;
	re = timenow->tm_hour + mi;
	e->time = re;
	e->temp = env->temp;
	e->humid = env->humid;
	e->illumi = env->illumi;
	//fprintf(f,"%f",re);
	fprintf(f,"%x",e->temp);
	fprintf(f,"%x",e->humid);
	fprintf(f,"%x",e->illumi);
	fprintf(f,"\n");
	free(e);
	fclose(f);
}

void SynRelay()
{
	if(s[0]->relaySta == OPEN) system("/web/zigbex_tosRF_Ubi 61 01");
	else if(s[0]->relaySta == CLOSE) system("/web/zigbex_tosRF_Ubi 61 02");

	if(s[1]->relaySta == OPEN) system("/web/zigbex_tosRF_Ubi 63 01");
	else if(s[1]->relaySta == CLOSE) system("/web/zigbex_tosRF_Ubi 63 02");

	if(s[2]->relaySta == OPEN) system("/web/zigbex_tosRF_Ubi 60 01");
	else if(s[2]->relaySta == CLOSE) system("/web/zigbex_tosRF_Ubi 60 02");

	if(s[3]->relaySta == OPEN) system("/web/zigbex_tosRF_Ubi 62 01");
	else if(s[3]->relaySta == CLOSE) system("/web/zigbex_tosRF_Ubi 62 02");
}

void OutputEnv()
{
	FILE * f = fopen("env.txt","ab+");

	fprintf(f,"%x ",env->temp);
	fprintf(f,"%x ",env->humid);
	fprintf(f,"%x ",env->illumi);
	fprintf(f,"%x ",env->thres.tempH);
	fprintf(f,"%x ",env->thres.tempL);
	fprintf(f,"%x ",env->thres.humidH);
	fprintf(f,"%x ",env->thres.humidL);
	fprintf(f,"%x ",env->thres.illumiH);
	fprintf(f,"%x ",env->thres.illumiL);

	fprintf(f,"\n");
	fclose(f);
}

void OutputRFID(char * name,g_Sensor_status * sstatus)
{
	FILE * f = fopen(name,"ab+");
	int i = 0;
	for(i = 0;i < 8;i++)
		fprintf(f,"%x ",sstatus->rfid[i]);

	fprintf(f,"\n");
	fclose(f);
}

int main()
{
	InitDaemon();

	FILE * pf = fopen("PidStore.txt","w");
	fprintf(pf,"%d\n",getpid());
	fclose(pf);	

	s[0] = (Sensor *)malloc(sizeof(Sensor));
	s[1] = (Sensor *)malloc(sizeof(Sensor));
	s[2] = (Sensor *)malloc(sizeof(Sensor));
	s[3] = (Sensor *)malloc(sizeof(Sensor));
	s[4] = (Sensor *)malloc(sizeof(Sensor));
	s[0]->relaySta = s[1]->relaySta = s[2]->relaySta = s[3]->relaySta = CLOSE;
	ctlSta = AUTO;

	signal(SIG_SET,Action);
	signal(SIG_GET,Action);
	
	int counter = 0;
	
	env = (Env *) malloc(sizeof(Env));
	env->temp = env->humid = env->illumi = \
	env->thres.tempH = env->thres.tempL = env->thres.humidH = env->thres.humidL =\
	env->thres.illumiH = env->thres.illumiL = 0;
	while(1)
	{
		g_Sensor_status * sstatus = (g_Sensor_status *)malloc(sizeof(g_Sensor_status));
		FILE * flog = fopen("AutoMachine.log","ab+");
		fprintf(flog,"%dth loops...\n",counter);
		fclose(flog);
		GetData(sstatus);
		UpdateEnv(sstatus,env);

		counter++;
		/*
		if(counter % 1200 == 0)
		{
			WriteFile("exchange.txt",env);
			OutputEnv();	
		}
		else
		{
			if(counter >= 12000)
			{
				unlink("exchange.txt");
				counter = 0;
			}
		}*/
		if(counter < 180)
		{
			WriteFile("exchange.txt",env);
			OutputRFID("rfid.txt",sstatus);
			OutputEnv();	
		}
		else
		{
				unlink("exchange.txt");
				unlink("AutoMachine.log");
				unlink("env.txt");
				unlink("rfid.txt");
				counter = 0;
		}

		if(ctlSta == AUTO)
			AutoControl(sstatus,env,s);
	
		//SynRelay();
		free(sstatus);
			
		sleep(1);		
	}
	unlink("/web/clientfifo");
	unlink("/web/serverfifo");
	free(env);
	return 0;
}
