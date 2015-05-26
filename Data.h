typedef unsigned char Status;

#define ROLLING	61
#define DOOR	63
#define HUMIDIFIER	62
#define	FAN		60
#define SETTHR	64
#define CTLSTA	65
/*
#define SIG_AUTO	40
#define SIG_MANU	41
#define SIG_GET		42
#define SIG_SETTHR	43
#define SIG_SETSEN	44
*/
#define SIG_SET	SIGUSR1
#define SIG_GET	SIGUSR2

#define OPEN	1
#define CLOSE	2

#define AUTO	0
#define MANU	1

/*
#define RUNNING	1
#define	WARN	2
#define ERROR	3
#define CLOSE	4
*/

#define WARNLINE	6

typedef struct g_Sensor_status {
	int id;
	int cmd;
	int illumi;
	int temp;	
	int humid;	
	int battery;	
	unsigned char rfid[8];
}g_Sensor_status;

typedef struct Thres	
{
	unsigned char tempH;
	unsigned char tempL;
	unsigned char illumiH;
	unsigned char illumiL;
	unsigned char humidH;
	unsigned char humidL;
}Thres;
typedef struct Env
{
	unsigned char temp;
	unsigned char illumi;
	unsigned char humid;
	Thres	thres;
}Env;


typedef struct Sensor
{
	unsigned char id;
	Status relaySta;	
	int counter;
}Sensor;

typedef struct Exchange
{
	long time;
	unsigned char temp;
	unsigned char humid;
	unsigned char illumi;
}Exchange;
