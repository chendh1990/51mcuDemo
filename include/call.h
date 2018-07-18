#ifndef __CALL_H
#define __CALL_H

#define CALL_AUTO_HOOK_ON_TIME							30
#define CALL_ALARM_CYCLE_TIME							3
#define CHECK_CALL_CONNECT_STATUS_INTERVAL				2
#define SIMPLE_DIAL_DURING_TIME							40
//////////////////DTMF///////////////////////////////////////////////////////////////////
#define DTMF_BUF_LENGH									4
#define DTMF_VERIFY_MAX_TIMES							3

#define DTMF_DISARMED									0x30	//'0'
#define DTMF_ARMED										0x31	//'1'   
#define DTMF_TALK_BACK 									0x33	//'3'  
#define DTMF_OPEN_ALARM    								0x39	//'9'   
#define DTMF_CLOSE_ALARM   								0x36	//'6' 
#define DTMF_MONITOR      								0x2A	//*
#define DTMF_HANG_UP      								0x23	//# 
/////////CALL STATUS///////////
enum
{
	CALL_FREE_STATUS=0,
	CALL_IN_STATUS,
	CALL_OUT_STATUS,
	CALL_CONNECT_STATUS,
};
/////////CALL TYPE///////////
enum
{
	CALL_NORMAL=0,
	CALL_ALARM,
};
/////////DTMF STAGE//////////////////////////////////////////////////////////////////////
enum
{
	DTMF_CHECK_PASSWORD = 0,
	DTMF_REMOTE_CODE,	
};

enum
{
	DTMF_PROMPT_NULL = 0,
	DTMF_PROMPT_ALARM,	
	DTMF_PROMPT_HOOKON,
};

enum
{
	DTMF_PLAY_ONE_BEEP = 1,
	DTMF_PLAY_TWO_BEEP,	
};
/////////DTMF STATUS////////
enum
{
	READ_SMS_FREE_STATUS=0,
	READ_SMS_HANDLE_STATUS,
};

/////////CALL IN STATUS///////////
enum
{
	CALL_IN_INIT=0,
	CALL_IN_SEND_ATA,
	CALL_IN_SEND_DDET,
	CALL_IN_SEND_VTS,
	CALL_IN_CONNECT_SUCCESS,
	CALL_IN_HOOK_ON,
};
/////////CALL OUT STATUS///////////
enum
{
	CALL_OUT_INIT=0,
	CALL_OUT_SEND_CHFA,
	CALL_OUT_SEND_ATD,
	CALL_OUT_SET_INQUERY_CONNECT,
	CALL_OUT_WAIT_ANSWER,
	CALL_OUT_CONNECT_SUCCESS,
	CALL_OUT_HOOK_ON,
};

enum
{
	CALL_PROMPT_NULL=0,
	CALL_PROMPT_NORMAL,	
	CALL_PROMPT_MENU,
	CALL_PROMPT_OPEN_SIREN,
	CALL_PROMPT_CLOSE_SIREN,
	CALL_PROMPT_HOOK_ON,
	CALL_PROMPT_TALK_BACK,	
	CALL_PROMPT_MONITOR,
	CALL_PROMPT_SOS,
};
////////////////////////////////////////
typedef struct
{
	unsigned char DTMF_buf[DTMF_BUF_LENGH];
	unsigned char DTMF_num;
	unsigned char DTMF_Verify_Times;
	unsigned char DTMF_Stage;

	unsigned char AutoHookOnCount;
	unsigned char CheckCallStatusCount;	
	unsigned char PromtToneCMD;
}Call_Var_t;

extern Call_Var_t g_CallVar;

void CallInit(void);
void GSMDialCallStatus(void);
void GSMReceiveCallStatus(void);
void GSMAlarmCallStatus(void);
void CallDTMFHandle(unsigned char DTMF_Code);
void CallDTMFRemote(unsigned char DTMF_Code);
void GSMCallTalkBack(void);
void GSMCallMonitor(void);
#endif

