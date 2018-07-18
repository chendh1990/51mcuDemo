#ifndef __GSM_H
#define __GSM_H

#define GSM_POWER_KEY_TIME			   						30 // 30*100ms=3s
#define GSM_CHECK_MODULE_STATUS_INTERVAL_TIME			   	30 // 30*100ms=3s
#define GSM_MAX_CHECK_MODULE_STATUS_TIME		   			15// 15 times
#define GSM_MODULE_WAIT_RESPOND_TIME			   			10 // 30*100ms=1s
#define GSM_CHECK_NET_STATUS_INTERVAL_TIME			   		200 // 200*100ms=20S
#define GSM_AT_COMMAND_TIMEOUT			   					40 // 40*100ms=4s
#define GSM_RESEND_AT_COMMAND_TIME			   				3

////////////////////////receive valid data offset/////////////////////////////////////////////
#define GSM_REGISTER_STATUS_LOCATE_OFFSET     				9
#define GSM_DTMF_LOCATE_OFFSET								6
#define GSM_CALL_NUMMBER_LOCATE_OFFSET						8
#define GSM_SMS_INDEX_LOCATE_OFFSET							12
#define GSM_SMS_PHONE_NUMMBER_OFFSET						21
#define GSM_SMS_COMMAND_OFFSET								21
#define GSM_SMS_TIME_OFFSET									50
/////////////////////////NET STATUS////////////////////////////////////////////////////////////
#define HOME_REGISTERED      								'1'
#define ROAMING_REGISTERED      							'5'
#define GSM_MAX_LOST_NET_TIME			   					3
////////////////////GSM_SPEAKER CHANNEL/////////////////////////////////////////////////
#define NORMAL_AUDIO										0
#define AUX_AUDIO											1 
////////////////GSM PIN ALLOCATION///////////////////////////////////////////////////////
#define GSM_MIC_CHANNEL_1									0
#define GSM_MIC_CHANNEL_2									1  //MIC
/////////////////////////////////////////////////////////////////////////
#define MAX_PHONE_NUMBER_LENGHT      						40//20
#define MAX_STORE_SMS_NUMBER								5
#define MAX_STORE_PHONE_NUMBER								5

#define DEFENCE_ZONE_NAME_LENGHT							50
#define MAX_DEFENCE_ZONE_NUMBER								9

#define MIN_SAME_PHONE_NUMBER								14//7

#define MAX_STORE_REMOTE_NUM     							10
#define MAX_CALL_IN_RING_TIME								2

enum
{
	GSM_MODULE_UNREADY=0,
	GSM_MODULE_READY,	
};

enum
{
	GSM_POWERON_STAGE_1 = 0,
	GSM_POWERON_STAGE_2,	
	GSM_POWERON_STAGE_3,
	GSM_CHECK_MODULE_STATUS_STAGE,
	GSM_INITIAL_SET_MODULE,
	GSM_CHECK_NET_STATUS,
};

enum
{
	GSM_RESPOND_NULL = 0,
	GSM_RESPOND_OK,	
	GSM_RESPOND_ERROR,
	GSM_RESPOND_WAIT_SMS_CONTENT,	
	GSM_WAIT_RESPOND,
};

enum
{
	GSM_INIT_STEP_1 = 0,
	GSM_INIT_STEP_2,	
	GSM_INIT_STEP_3,
	GSM_INIT_STEP_4,
	GSM_INIT_STEP_5,
	GSM_INIT_STEP_6,
	GSM_INIT_STEP_7,
	GSM_INIT_STEP_8,
	GSM_INIT_STEP_9,
	GSM_INIT_STEP_10,	
	GSM_INIT_STEP_LENGTH,	
};
/////////////////////////////////////////////////////////////////////////////
#define GSM_EVENT_TIMEOUT	  							0
#define GSM_EVENT_BUFFER_LENGTH	  						20 
#define QUERY_MODULE_INTERVAL_TIME						3 // 3S
#define QUERY_NET_INTERVAL_TIME							30 

/////////////////////////////////////////////////////////////////////////////
#define AT_NO_ARG										0 
/////////GSM STATUS//////////////////////////////////////////////////////////
enum
{
	GSM_START_UP_STATUS=0,
	GSM_IDLE_STATUS,
	GSM_READ_SMS_STATUS,
	GSM_SEND_SMS_STATUS,
	GSM_RECEIVE_CALL_STATUS,
	GSM_ALARM_CALL_STATUS,
	GSM_DIAL_CALL_STATUS,
};
////////GSM EVENT////////////////////////////////////////////////////////////
enum
{
	GSM_EVENT_NULL = 0,
	GSM_EVENT_INIT_CURRENT_STATUS,
	GSM_EVENT_LEFT_CURRENT_STATUS,
	GSM_EVENT_QUERY_MODULE_STATUS,	
	GSM_EVENT_MODULE_INIT_SET,	
	GSM_EVENT_RECEIVE_DATA_FROM_USART,
	GSM_EVENT_CHECK_NET_STATUS,
	GSM_EVENT_RECEIVE_SMS_NOTE,
	GSM_EVENT_PARSE_SMS_COMPLETE,		
	GSM_EVENT_READ_SMS_COMPLETE,	
	GSM_EVENT_READ_SMS_NUMBER,	
	GSM_EVENT_SEND_SMS_CONTENT,
	GSM_EVENT_SEND_SMS_COMPLETE,
	GSM_EVENT_INQUERY_CALL_CONNECT_STATUS,	
	GSM_EVENT_CALL_CONNECT_ESTABLISH,	
	GSM_EVENT_CALL_AUTO_HOOKON,
	GSM_EVENT_BUSY,	
	GSM_EVENT_NO_CARRIER,	
	GSM_EVENT_NO_ANSWER,	
	GSM_EVENT_DIAL_CALL_REQUEST,
	GSM_EVENT_HOOK_ON_REQUEST,
	GSM_EVENT_READ_PHONE_NUMBER,
	GSM_EVENT_SYS_ALARM,
	GSM_EVENT_SYS_INFOR_SMS,
	GSM_EVENT_SYS_ENTRY_DISARM,	
	GSM_EVENT_KEY_DTMF,	
	GSM_EVENT_MUTE_ON,	
	GSM_EVENT_MUTE_OFF,	
	GSM_EVENT_DEBUG_AT_CMD,	
};
///////////////////////////////////////////////////////////////////////////////
typedef struct{
	unsigned int Queue[10];
	unsigned char cStartupState;
	unsigned char cQueryModuleInterval;
	unsigned char cQueryNetInterval;	
	unsigned char cSendMSGControl;
	
	unsigned char cState;
	unsigned char cCheckModuleCount;	
	unsigned char ModuleStatus;	
	unsigned char GSMLostNetTime;
	unsigned char RingTimeNum;	
	unsigned char GSMInitStep;
	unsigned char RespondVal;	
}GSM_Var_t;

extern xdata GSM_Var_t g_GSMVar;
void GSMInit(void);
void GSMQueueInit(void);
void SendToGSM( unsigned char *pFormat, ...);
unsigned char ATCommand( unsigned char *pCommand,  unsigned char pArg , unsigned char *pReturnVal);
void GsmMsgPost(unsigned int MsgID, unsigned int Param);
void GsmMsgPend(MSG_t *pMsg);
void GSMRoutineHandle(void);
void GSMReceiveDataHandle(void);
void GSMStatusMachine(unsigned char GsmStatus);
#endif

