#ifndef __TIMER_H
#define __TIMER_H

#define MIllSECOND(x)							(x)
#define SECOND(x)								(x)*10
#define MINUTE(x)								(x)*(SECOND(60))
#define TIMER_TABLE_MAX							6
////////////////////HOST TIMER/////////////////////////////////////
#define HOST_BEEP_TONE_TIMER 					"H0"
#define HOST_DELAY_ARM_TIMER 					"H1"
#define HOST_DELAY_ALARM_TIMER 					"H2"
#define HOST_RF_REGISTER_TIMER 					"H4"
#define HOST_SIREN_TIMER						"H5"
#define HOST_STARTUP_PROMPT_TIMER				"H6"
////////////////////WIFI TIMER///////////////////////////////////////////////
#define WIFI_SMARTLINK_TIMER					"W1"
#define WIFI_CONFIG_TIMER						"W2"



typedef struct{
	//Quene_ID_t QueneID;
	//MSG_Q_t *pQuene;
	MSG_t Msg;
	uint iTimeOut;
	unsigned char *pName;
}  TimerSever_t;

void TimerInit(void);
TaskTimeout_t SysTimerUnitTask(void);
void TimerUnitDel(unsigned char *pName);
void SetTimeout(unsigned char *pName, MSG_t Msg, TimeOutVal_t Timeout);
#endif

