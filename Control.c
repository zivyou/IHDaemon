#include "Data.h"
#include <stdio.h>
#include <malloc.h>

unsigned char doorKey[8] = {0xe0,0x4,0x01,0x00,0x5b,0x29,0x39,0x02} ;

void UpdateEnv(g_Sensor_status * sstatus,Env * env)
{
	env->temp = sstatus->temp % 256;
	env->humid = sstatus->humid % 256;
	env->illumi = sstatus->illumi % 256;
}

int CheckKey(unsigned char k[8])
{
	int i = 0;
	for(i = 0;i < 8;i++)
	if(k[i] != doorKey[i]) break;
	if(i == 8) return 1;
	else return 0;
}

void AutoControl(g_Sensor_status * sstatus,Env * env,Sensor * s[4])
{
	UpdateEnv(sstatus,env);	

	switch(sstatus->id)
	{
		case FAN:
			if(s[2] == NULL)
			{
				s[2] = (Sensor *) malloc(sizeof(Sensor));
				s[2]->id = sstatus->id;
				s[2]->relaySta = CLOSE;
				s[2]->counter = 0;										
			}
	
			if(env->temp > env->thres.tempH)
			{
				if(s[2]->relaySta == CLOSE)	
				{
					system("/web/zigbex_tosRF_Ubi 60 01");  
					s[2]->relaySta = OPEN;			
				}	
			}
			else if(env->temp < env->thres.tempL)
			{
				if(s[2]->relaySta == OPEN)	
				{
					system("/web/zigbex_tosRF_Ubi 60 02");  
					s[2]->relaySta = CLOSE;			
				}	
				
			}
			break;
		case HUMIDIFIER:
			if(s[3] == NULL)
			{
				s[3] = (Sensor *) malloc(sizeof(Sensor));
				s[3]->id = sstatus->id;
				s[3]->relaySta = CLOSE;
				s[3]->counter = 0;										
			}
	
			if(env->humid > env->thres.humidH)
			{
				if(s[3]->relaySta == CLOSE)	
				{
					system("/web/zigbex_tosRF_Ubi 62 01");  
					s[3]->relaySta = OPEN;			
				}	
			}
			else if(env->humid < env->thres.humidL)
			{
				if(s[3]->relaySta == OPEN)	
				{
					system("/web/zigbex_tosRF_Ubi 62 02");  
					s[3]->relaySta = CLOSE;			
				}	
				
			}
			break;
		case DOOR:
			if(s[1] == NULL)
			{
				s[1] = (Sensor *) malloc(sizeof(Sensor));
				s[1]->id = sstatus->id;
				s[1]->relaySta = CLOSE;
				s[1]->counter = 0;										
			}
	
			if(CheckKey(sstatus->rfid))
			{
				if(s[1]->relaySta == CLOSE)	
				{
					system("/web/zigbex_tosRF_Ubi 63 01");  
					s[1]->relaySta = OPEN;			
				}	
			}
			else if(!CheckKey(sstatus->rfid))
			{
				if(s[1]->relaySta == OPEN)	
				{
					system("/web/zigbex_tosRF_Ubi 63 02");  
					s[1]->relaySta = CLOSE;			
				}	
				
			}
			break;
		case ROLLING:
			if(s[0] == NULL)
			{
				s[0] =(Sensor *)malloc(sizeof(Sensor));
				s[0]->id = sstatus->id;
				s[0]->relaySta = CLOSE;
				s[0]->counter = 0;										
			}
	
			if(env->illumi > env->thres.illumiH)
			{
				if(s[0]->relaySta == OPEN)	
				{
					system("/web/zigbex_tosRF_Ubi 61 02");  
					s[0]->relaySta = CLOSE;			
				}	
			}
			else if(env->illumi < env->thres.illumiL)
			{
				if(s[0]->relaySta == CLOSE)	
				{
					system("/web/zigbex_tosRF_Ubi 61 01");  
					s[0]->relaySta = OPEN;			
				}	
				
			}
			break;
		default:
			break;
	
	}
}
