#ifndef __SYS_H
#define __SYS_H
#include "config.h"

#define SOFTWARE_VERSION	  					0x1003
////////////////////HW DEFINE/////////////////////////////////////
//#define MODEL_MV3
//#define MODEL_S3
//#define MODEL_T6
#define MODEL_DOOR_BELL

#define MODULE_NAME								"t0"

/////////CPU NAME/////////////////////////
//#define STC15F2K48S2
#define STC15L2K48S2
//#define STC15L2K60S2
//#define IAP15L2K61S2
/////////////////////////////////////////////////
//#define HW_T0_0
#define HW_T0_1
//#define HW_T0_2
///////////////////////////////////////////////
/////////////cpu ID config////////////////////////////////////////////
#if defined(STC15F2K48S2) || defined(STC15L2K48S2)
#define CPU_ID_ADD  				0XBFF9//48s2
#define EE_SECTOR_TOTAL     		26 
#elif defined(STC15F2K60S2)
#define CPU_ID_ADD  0XEFF9//60s2
#elif defined(IAP15L2K61S2)
#define CPU_ID_ADD  0Xf3f9//61s2
#endif

#define CPU_ID_ADD_START	CPU_ID_ADD//最后的7个单元是 CPU ID
#define CPU_ID_ADD_END 		CPU_ID_ADD+6//最后的7个单元是 CPU ID

#define MAX_SIREN_3_MINUTE

///////////pin assigment///////////////////////////////////////
#if defined(HW_T0_2)
sbit p_WIFI_TX	       	= P1^0;
sbit p_WIFI_RX	       	= P1^1;
sbit p_BAT	    	= P1^2;
sbit p_NTC	    		= P1^3;
sbit p_PIR_LED	    	= P1^4;
sbit p_WIFI_LED	    	= P1^5;
sbit p_SMOKE_LED	    = P1^6;
sbit p_DOOR_LED	    	= P1^7;

sbit p_WT_DATA	       	= P2^0;
sbit p_WT_RESET	       	= P2^1;
sbit p_VOL	       		= P2^2;
sbit p_PWM	    		= P2^3;
sbit p_8002_EN	    	= P2^4;

sbit p_WIFI_RST			= P2^6;
sbit p_WIFI_EN	    	= P2^7;

sbit p_RXD	    		= P3^0;
sbit p_TXD	    		= P3^1;
sbit p_RF	    		= P3^2;
sbit p_RF_EN            = P3^3;
sbit p_S3	    		= P3^4;
sbit p_S2				= P3^5;
sbit p_S1	    		= P3^6;
sbit p_SOS_LED	    	= P2^0;
sbit p_AC  				= P3^7;

sbit p_DOORBELL  		= P5^4;
sbit p_SOS_LED  		= P5^5;

#elif defined(HW_T0_1)
sbit p_WIFI_TX	       	= P1^0;
sbit p_WIFI_RX	       	= P1^1;
sbit p_WIFI_LED	    	= P1^2;


sbit p_9300_EN	       	= P2^0;
sbit p_SET_LED	       	= P2^1;
sbit p_VOL	       		= P2^2;
sbit p_PWM	    		= P2^3;
sbit p_8002_EN	    	= P2^4;
sbit p_RF_EN			= P2^5;
sbit p_WIFI_RST			= P2^6;
sbit p_WIFI_EN	    	= P2^7;

sbit p_RXD	    		= P3^0;
sbit p_TXD	    		= P3^1;
sbit p_RF	    		= P3^2;
sbit p_WT_DATA			= P3^3;
sbit p_WT_RESET	    	= P3^4;
sbit p_S2				= P3^5;
sbit p_S1	    		= P3^6;
sbit p_SOS_LED	    	= P2^0;
sbit p_AC  				= P3^7;

#elif defined(HW_T0_0)
sbit p_WIFI_TX	       	= P1^0;
sbit p_WIFI_RX	       	= P1^1;
sbit p_WIFI_LED	    	= P1^2;


sbit p_9300_EN	       	= P2^0;
sbit p_SET_LED	       	= P2^1;
sbit p_VOL	       		= P2^2;
sbit p_PWM	    		= P2^3;
sbit p_8002_EN	    	= P2^4;
sbit p_RF_EN			= P2^5;
sbit p_WIFI_RST			= P2^6;
sbit p_WIFI_EN	    	= P2^7;

sbit p_RXD	    		= P3^0;
sbit p_TXD	    		= P3^1;
sbit p_RF	    		= P3^2;
sbit p_WT_DATA			= P3^3;
sbit p_WT_RESET	    	= P3^4;
sbit p_S2				= P3^5;
sbit p_S1	    		= P3^6;
sbit p_SOS_LED	    	= P3^7;
sbit p_AC  				= P5^5;
#endif
////////////////////HW DEFINE END///////////////////////////////////////////

#define FOSC          										30000000L          

#define BAUD_9600      										9600//115200            
#define BAUD_115200      									115200 
#define S1_S0 												0x40              //P_SW1.6
#define S1_S1 												0x80              //P_SW1.7
#define T100Hz    											(FOSC / 12 / 100)
#define END_FLAG			   								0xff

///////////////////////////////////////////////////////////////////////////////
#define TASK_DISABLE     									0xff
#define TASK_TO_RUN      									0
#define SYS_RUN_TASK_IMMEDIATELY							0
#define SYS_ROUTINE_RUN_INTERVAL     						10 // 10*100MS = 1S
///////////////////////////////////////////////////////////////////////////////
#define HOST_ID_LENGTH			   							3
#define MAC_ADDRESS_LENGTH			   						6
#define IP_ADDRESS_LENGTH			   						4
#define TCP_DEFAULT_PORT			   						5893
#define UDP_DEFAULT_PORT			   						10002
#define UDP_SMART_LINK_DEFAULT_PORT			   				4560
////////////////////////////////////////////////////////////////////////////////
#define HOST_MSG_BUF_SIZE     								20
#define HOST_COMMON_BUF_SIZE     							512 // 400 //250
#define PASSWORD_LENGTH	  									4
#define MAX_DELAY_ARMED_TIME								300 //300S
#define MAX_DELAY_SIREN_TIME								300 //300S
#define HOST_AUTO_REPORT_INTERVAL				 			300 //300S
#define SYS_STATUS_SWITCH_UPLOAD_TIME	  					3
//////////////////////////////////////////////////////////////////////////////////
#define AUTO_UPLOAD_SYSTEM_STATUS_TIME     					60//300
#define ALARM_INFOR_BUFFER_SIZE			   					8
#define SYS_CHECK_ALARM_EVENT_INTERVAL_TIME	  				3
#define DEVICE_ID_LENGTH			   						6

//////////////////////////////////////////////////////////////////////
#define PAGE_RF_UNIT_NUM									10
////////////////////////////////////////////////////////////////////////////////
#define GPRS_APN_LENGTH			   							16
#define GPRS_APN_NAME_LENGTH			   					16
#define GPRS_APN_PASSWORD_LENGTH			   				16
////////////////////APP POTOCAL COMMAND SET////////////////////////////////////////////////
#define APP_COMMAND_SEARCH_HOST                  	"\"comm0\""  
#define APP_COMMAND_LOGIN_HOST                  	"\"comm1\"" 
#define APP_COMMAND_INQUERY_SET_SYS_SETTING         "\"comm2\"" 
#define APP_COMMAND_INQUERY_SET_ALARM_HOST          "\"comm3\""
#define APP_COMMAND_INQUERY_SET_TEL_NUMBER          "\"comm4\""
#define APP_COMMAND_INQUERY_SET_PASSWORD	        "\"comm5\""
#define APP_COMMAND_INQUERY_SET_SYS_STATUS          "\"comm6\""
#define APP_COMMAND_INQUERY_HISTORY_LOG				"\"comm7\""
#define APP_COMMAND_INQUERY_RF_ACCESSORY         	"\"comm8\""
#define APP_COMMAND_SET_RF_RC		        		"\"comm9\""
#define APP_COMMAND_REGISTER_RF_RC         			"\"comm10\""
#define APP_COMMAND_UNREGISTER_RF_RC         		"\"comm11\""
#define APP_COMMAND_SET_RF_ZONE         			"\"comm12\""
#define APP_COMMAND_REGISTER_RF_ZONE         		"\"comm13\""
#define APP_COMMAND_UNREGISTER_RF_ZONE         		"\"comm14\""
#define APP_COMMAND_SET_RF_DOOR_BELL         		"\"comm15\""
#define APP_COMMAND_REGISTER_RF_DOOR_BELL         	"\"comm16\""
#define APP_COMMAND_UNREGISTER_RF_DOOR_BELL         "\"comm17\""
#define APP_COMMAND_ENTRY_REGISTER_STATUS         	"\"comm18\""
//////////////////////////////////////////////////////////////////////////////////////////
#define PLATFORM_RESPOND_OK         				"ACCEPTOK"
//////////////////////////////////////////////////////////////////////////////////
/*
0撤防 1布防 2旁路布防 3紧急求助 4报警 5保安到达（巡更） 6断电 7低电 8无操作 9防拆 10恢复
11故障 12断线 13自检错误 14夜间撤防 15上电 
*/
enum __ALARM__CASE
{
	CASE_DISARM=0,
	CASE_ARM,
	CASE_BYPASS,
	CASE_SOS,
	CASE_ALARM,
	CASE_POLICE,
	CASE_ACLOST,
	CASE_LB,
	CASE_NOP,
	CASE_MOVE,
	CASE_GUZHANG_RECOVER,
	CASE_GUZHANG,
	CASE_BREAK,
	CASE_SENSOR_SELF_ERR,
	CASE_NIGHT_CHEFANG,//夜间撤防　
	CASE_AC_RECOVER,
	CASE_NUL
};

enum ALARM__NAME
{	  
	N_RT=0,
	N_IR,
	N_DOOR,
	N_SMOKE,
	N_GAS,
	N_DUISHE,  
	N_GLASSES,
	N_WENGAN,
	N_UNDEFINE,
	N_TIME,//定时
	N_SHUIGAN,
	N_FAR,
	  
	N_HOST,
	N_KEY_PANEL,	  
	N_WIRE,
	N_APP,	
	N_DOUBLE_NETWORK,
	N_SMS,
	N_XUEYAJI,//血压计
	N_PC,
	N_DOOR_BELL,
};

enum ALARM_MODE
{
	MODE_ARM = 1,		
	MODE_BYPASS,
	MODE_24HOUR,
	MODE_DELAY
};	

enum SPEC_ZONE_DEINFE
{
	ZONE_SOS=0X99,
	ZONE_HIJACK=0X98,	
	ZONE_EMERGENCY=0X97,
	ZONE_AC=0X96,
	ZONE_MOVE=0X95,
	ZONE_LB=0X94,
	ZONE_SMS=0X93,
	ZONE_TIME=0x92,
	ZONE_WIRE=0x91,


	ZONE_KEY=0X91,
	ZONE_SELF=0X90,  /////自检
	ZONE_CSQ_LOW=0X89,
	ZONE_GPRS=0X81,
	//ZONE_NO_CREG=0X91,
	ZONE_TEL=0X88
};
/////////Communication mode //////////
enum
{
	TCP_MODE=0,
	UDP_MODE,
};
//////////////////////////////////////////////////////////////////////////////////
enum
{
	LOCATE_ENGLISH=0,
	LOCATE_FRENCH,	
	LOCATE_RUSSIAN,
//	LOCATE_GERMAN,	
	LOCATE_CHINESE,
};

typedef enum {
    LEVEL_LOW             	   		= 0,
    LEVEL_HIGH                   	= 1,
} Pin_Level_t;

typedef enum {
	RET_FAIL=0,	
	RET_OK,					
} return_val_t;

typedef enum {
	QUENE_HOST=0,	
	QUENE_GSM,					
} Quene_ID_t;

typedef enum
{
	//////////////////////////////////////////////////////////////////////////////////
	// Dial keys
	//////////////////////////////////////////////////////////////////////////////////

	SYS_MSG_KEY_PAD_0		            	= 0x00,  
	SYS_MSG_KEY_PAD_1		     	    	= 0x01,  
	SYS_MSG_KEY_PAD_2		     	   		= 0x02,  
	SYS_MSG_KEY_PAD_3		     	   		= 0x03,  
	SYS_MSG_KEY_PAD_4		     	    	= 0x04,  
	SYS_MSG_KEY_PAD_5		     	   		= 0x05,  
	SYS_MSG_KEY_PAD_6		     	   		= 0x06,  
	SYS_MSG_KEY_PAD_7		     	   		= 0x07,  
	SYS_MSG_KEY_PAD_8		     	    	= 0x08,  
	SYS_MSG_KEY_PAD_9		     	    	= 0x09,  
	SYS_MSG_KEY_PAD_ASTERISK     			= 0x0A,	
	SYS_MSG_KEY_PAD_POUND	     			= 0x0B,
	SYS_MSG_KEY_PAD_DIAL	     			= 0x0C,
	SYS_MSG_KEY_PAD_ARM           			= 0x0D,  
	SYS_MSG_KEY_PAD_DISARM					= 0x0E,  
	SYS_MSG_KEY_PAD_HOME_ARM	     		= 0x0F,
	SYS_MSG_KEY_PAD_LONG_PRESS_DIAL			= 0x10,
	SYS_MSG_KEY_PAD_SOS     				= 0x11,	
	SYS_MSG_KEY_PLAY						= 0x12, 
	SYS_MSG_NO_KEY							= 0x13,  
	///////////////////////////////////////////////////////////////

	SYS_MSG_KEY_PRESS	     				= 0x14,
	SYS_MSG_KEY_LONG_PRESS	     			= 0x15, 
	SYS_MSG_RF_DECODE_COMPLETE	     		= 0x16,	
	SYS_MSG_SIREN_TIME_OUT	     			= 0x17,		
	SYS_MSG_RECEIVE_DATA_FROM_USART	     	= 0x18,	
	SYS_MSG_HOST_LOW_BAT    				= 0x19,	
	SYS_MSG_HOST_SOS_ALARM                	= 0x1A,	
	///////////////////////////////////////////////////////////// 
	SYS_MSG_INIT_CURRENT_STATUS             = 0x1B,	
	SYS_MSG_LEFT_CURRENT_STATUS             = 0x1C,
	SYS_MSG_ARM_STATUS_REQUEST             	= 0x1D,
	SYS_MSG_DISARM_STATUS_REQUEST           = 0x1E,
	SYS_MSG_HOME_ARM_STATUS_REQUEST         = 0x1F,
    /////////////////////////////////////////////////////////////
	SYS_MSG_POWER_OFF	     				= 0x20,
	SYS_MSG_HOST_TAMPER_ALARM     			= 0x21,	
	SYS_MSG_AC_SWITCH	     				= 0x22,	
	//SYS_MSG_AC_RESTORE           			= 0x23,
	SYS_MSG_KEY1_PRESS	     				= 0x24,
	SYS_MSG_KEY2_PRESS	     				= 0x25,
	SYS_MSG_KEY1_LONG_PRESS	     			= 0x26,
	SYS_MSG_KEY2_LONG_PRESS	     			= 0x27,
	SYS_MSG_MINUTE_UPDATE	     			= 0x28,
	/////////////RF MSG//////////////////////////////////////////
	SYS_MSG_RF_DISARM_CMD					= 0x30,	
	SYS_MSG_RF_ARM_CMD						= 0x31,	
	SYS_MSG_RF_HOME_ARM_CMD					= 0x32,	
	SYS_MSG_RF_SOS_CMD						= 0x33,	
	SYS_MSG_RF_24HOUR_ZONE					= 0x34,	
	SYS_MSG_RF_NORMAL_ZONE					= 0x35,
	SYS_MSG_RF_DELAY_ZONE					= 0x36,
	SYS_MSG_RF_AT_HOME_ZONE					= 0x37,	
	SYS_MSG_RF_ACCESSORY_LOW_BAT			= 0x38,	
	SYS_MSG_RF_ACCESSORY_TAMPER				= 0x39,	
	SYS_MSG_RF_SEND							= 0x3A,	
	SYS_MSG_RFID_RECEIVE_COMPLETE			= 0x3B,
	SYS_MSG_AUTO_LOCK						= 0x3C,	
	/////////////TONE MSG/////////////////////////////////////////
	SYS_MSG_PLAY_PROMPT_TONE                = 0x40,
	SYS_MSG_PROMPT_TONE_OVER                = 0x41,
	SYS_MSG_CLOSE_SIREN                		= 0x42,
	SYS_MSG_PLAY_TONE                		= 0x43,
	SYS_MSG_DELAY_ARM_TONE                	= 0x44,
	SYS_MSG_PLAY_DOOR_BELL_TIMEOUT         	= 0x45,
  ///////////WIFI////////////////////////////////
	SYS_MSG_WIFI_STARTUP_RESET				= 0x70,	
	SYS_MSG_RECEIVE_DATA_FROM_WIFI			= 0x71,	
	SYS_MSG_WIFI_START_SMARTLINK			= 0x72,
	SYS_MSG_WIFI_CONFIG						= 0x73,
	SYS_MSG_WIFI_STOP_SMARTLINK				= 0x74,
	SYS_MSG_WIFI_DISCONNECT					= 0x75,
	SYS_MSG_WIFI_LOST_CONNECT_PLATFORM		= 0x76,
	SYS_MSG_WIFI_CONNECT_PLATFORM			= 0x77,
	SYS_MSG_WIFI_UPLOAD_PLATFORM			= 0x78,
	SYS_MSG_WIFI_RESET						= 0x79,
	SYS_MSG_WIFI_SMARTLINK_SUCCESS			= 0x7A,
	SYS_MSG_WIFI_SET_PLATFORM_SERVER_ADR	= 0x7B,
	SYS_MSG_WIFI_TCP_RECONNECT				= 0x7C,
	SYS_MSG_WIFI_DEFAULT					= 0x7D,	
	SYS_MSG_WIFI_CONNECTED					= 0x7E,
	SYS_MSG_WIFI_POWER_ON					= 0x7F,
	SYS_MSG_WIFI_AP_STATION_CONNECTED		= 0x80,
	SYS_MSG_WIFI_AP_STATION_DISCONNECTED	= 0x81,
	SYS_MSG_WIFI_STATION_GOT_IP				= 0x82,
	SYS_MSG_WIFI_SET_AP_CONNECTED			= 0x83,
	SYS_MSG_WIFI_EXIT_SET_AP_CONNECTED		= 0x84,
	SYS_MSG_WIFI_SET_AP_CONNECTED_SUCCESS	= 0x85,	
	SYS_MSG_WIFI_CHECK_PLATFORM_CONNECT_STATUS	= 0x86,	
	SYS_MSG_WIFI_SEND_DEVICE_INFOR			= 0x87,	

    ///////////////////////////////////////////
	// Last invalid event
	///////////////////////////////////////////
	
	SYS_MSG_NULL=0xffff,
} SystemEvent_t;

#define SYS_MSG_KEY_PAD_RECORD				 SYS_MSG_KEY_PAD_ASTERISK
#define SYS_MSG_KEY_PAD_PLAY				 SYS_MSG_KEY_PAD_POUND
////////////////////////////////////////////////////////
typedef unsigned int TimeOutVal_t; 
typedef unsigned char TaskTimeout_t; 
typedef  TaskTimeout_t (*TaskTable_t)(void);

/////////SYSTEM STATUS///////////
typedef enum
{
	SYSTEM_NULL_STATUS=0,
	SYSTEM_DISARMED_STATUS,	
	SYSTEM_ARMED_STATUS,
	SYSTEM_HOME_ARMED_STATUS,
	SYSTEM_KEY_SETTING_STATUS,
	SYSTEM_RF_REGISTER_STATUS,
} system_status_t;

typedef enum {
	SYS_TIMER_TASK_ID=0,
	SCAN_KEY_1_TASK_ID,
	SCAN_KEY_2_TASK_ID,
	SYS_ROUTINE_TASK_ID,
	LED_TASK_ID,
	AC_DETECT_TASK_ID,
	BAT_CHECK_TASK_ID,
	BEEPER_TASK_ID,
///////////////////////////////	
	TASK_TABLE_LENGTH,						
} UnitTask_t;
/////////SYSTEM LANGUAGE///////////
enum
{
	LANGUAGE_CHINESE=0,
	LANGUAGE_ENGLISH,
	LANGUAGE_FRENCH,	
	LANGUAGE_RUSSIAN,
	LANGUAGE_DANISH,	
	LANGUAGE_DUTCH,
	LANGUAGE_ITALIAN,	
	LANGUAGE_SWEDISH,
	LANGUAGE_GERMAN,	
	LANGUAGE_SPANISH,
	LANGUAGE_FINNISH,	
	LANGUAGE_NORWEGIAN,
	LANGUAGE_ARABIC,	
	LANGUAGE_FARSI,
};

/////////SWICH SYSTEM STATUS///////////
enum
{
	SYSTEM_STATUS_FROM_KEYPAD=0x0000,
	SYSTEM_STATUS_FROM_RC=0x0001,
	SYSTEM_STATUS_FROM_SMS=0x0002,
	SYSTEM_STATUS_FROM_RFID=0x0003,
	SYSTEM_STATUS_FROM_DTMF=0x0004,
};	
//////////////////////////////////////////////////////////////////////
typedef struct{
	unsigned int msgID;
	unsigned int Param;
} MSG_t;

typedef struct{
	unsigned int msgID;
	unsigned int *pParam;
} MSG_PTR_t;

typedef struct{
	MSG_t *pMsgBuf;
	uchar cReadIndex;
	uchar cWriteIndex;
	uchar cBufSize;
} MSG_Q_t;	

typedef struct{
	unsigned char Case;
	unsigned char Name;
	unsigned char Locate;
} Alarm_Infor_t;

typedef struct{
	unsigned char Total;
	unsigned char Head;
} RF_Adr_Map_t;

typedef struct{
	unsigned int AutoUploadTime;
	unsigned int DelayAlarmParam;
	unsigned int SirenLastingTime;
	unsigned char DelayArmStatus;	
	unsigned char DelayAlarmStatus;		
	unsigned char DelayAlarmCount;	
	unsigned char DelayArmCount;
	unsigned char DelayArmType;
	unsigned char SystemLastStatus;
	unsigned char SystemCurrentStatus;
	unsigned char SystemNextStatus;
	unsigned char SirenResuceCount;	
	unsigned char PowerNormalVolCount;
	unsigned char PowerLowVolCount;
	unsigned char PowerStatus;
	unsigned char CheckAlarmEventInterval;
	unsigned char UploadPlatformControl;
	unsigned char AlarmInforIndex;
	Alarm_Infor_t AlarmInforBuffer[ALARM_INFOR_BUFFER_SIZE];
	Alarm_Infor_t UploadAlarmInforBackup;
	
	////////////////////////////////
	unsigned char Key1Status;
	unsigned char Key1DebounceCounter;
	unsigned char Key2Status;
	unsigned char Key2DebounceCounter;
	unsigned char ACStatus;	
	unsigned char StartupFlag;	
	unsigned char SirenStatus;
	unsigned char DeviceID[DEVICE_ID_LENGTH];

	unsigned char PWMFlag;	
	unsigned char PlayDoorBell;	
	unsigned char PlayDoorTimeout;	
	unsigned char PlayStartupToneCount;	
} System_Var_t;

typedef struct{
	unsigned char ParamInitFlag;
	unsigned char SystemStatus;
	unsigned char SirenLastingTime;
	unsigned char SystemLanguage;
	unsigned char SystemPassword[PASSWORD_LENGTH];	
	unsigned char DebugInforOutput;	
	unsigned char RegisteredRemoteNum;
	unsigned char RegisteredDoorBellNum;
	unsigned char RegisteredWirelessAccessoryNum;
	unsigned char HistoryLogNum;
	unsigned char SirenMuteFlag;
	unsigned char DelayArmTime;
	unsigned char DelayAlarmTime;	
	unsigned char RingTime; 
	unsigned char CallRecycleTime; 
	unsigned char AccompanyToneFlag;
	unsigned char ArmDisarmSmsInforFlag;
	unsigned char AcSwitchInforFlag;
	unsigned char AutoArmDisarmFlag;
	unsigned char HostID[HOST_ID_LENGTH];
	unsigned char HostMacAdr[MAC_ADDRESS_LENGTH];
	unsigned char HostIPAdr[IP_ADDRESS_LENGTH];
	unsigned char HostGate[IP_ADDRESS_LENGTH];
	unsigned char HostSubMask[IP_ADDRESS_LENGTH];
	unsigned char TcpIpServerAdr[IP_ADDRESS_LENGTH];
	unsigned char RfAdrMapHead;
	unsigned char RfAdrMapTotal;
	unsigned int TcpIpServerPort;
	unsigned int UdpLocalPort;
	unsigned int UdpSmartLinkPort;

	unsigned int AutoArmTime;
	unsigned int AutoDisarmTime;
	unsigned int AutoUploadTime;
	unsigned int CheckSum;
} System_Param_t;

typedef void (*VoidFun)(void);

//////////////////////////////////////////////////////////////////////
extern xdata System_Var_t g_SystemVar;
extern xdata System_Param_t g_SystemParam;
extern xdata MSG_Q_t g_HostQuene;
extern xdata unsigned char g_HostCommonBuf[HOST_COMMON_BUF_SIZE];
extern xdata MSG_t g_HostMsg;

#define HOST_QUENE_PTR	 ((MSG_Q_t*)&g_HostQuene)
void SysInit(void);
void SysTaskEnable(UnitTask_t eTask, uchar cTime);
void SysRunTask (void);
MSG_t CreateMsg(unsigned int MsgID, unsigned int Param);
void HostMsgPost(unsigned int MsgID, unsigned int Param);
void HostMsgPend(void);
void SystemSever(MSG_t *pMsg);
void SystemStatusMachine(unsigned char SystemStatus);
void AlarmInforPush(unsigned char Mode, unsigned char Type, unsigned char Locate);
unsigned char AlarmInforPop(unsigned char *pBuf);
////////////lib///////////////////////////////////////////////////////////////////////////////
void DelayNop(unsigned char nop);
unsigned char AscStrToByte(unsigned char *Asc_Str);
void HexToAsc(unsigned char *dst, unsigned char src);
//////////////////////////////////////////////////////////////////////////////////////////////
#endif

