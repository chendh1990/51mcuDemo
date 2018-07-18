#ifndef __WIFI_H
#define __WIFI_H

#define WIFI_ENTRY_SMARTLINK_TIMEOUT						3
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define WIFI_START_RESET_TIMEOUT     						2
#define WIFI_RESET_TIMEOUT     								240
#define WIFI_SMARTLINK_TIMEOUT     							60
#define WIFI_CONFIG_TIMEOUT     							20
#define WIFI_CONFIG_DATA_LENGTH     						10 
#define MAX_WAIT_TCP_RESPOND_TIME     						10
#define MAX_TCP_SEND_FAIL_TIME     							3
#define WIFI_GET_IP_TIMEOUT     							40
#define WIFI_SET_CONNECT_AP_TIMEOUT     					240
#define WIFI_CHECK_PLATFORM_CONNECT_INTERVAL     			6
#define WIFI_DEVICE_INFOR_RESPOND_TIMEOUT     				3

typedef enum
{
	WIFI_NORMAL=0,
	WIFI_SMARTLINK,
}WIFI_Mode_t;

typedef enum
{
	WIFI_NULL=0,
	WIFI_DISCONNECT,
	WIFI_CONNECTED,
	WIFI_LOST_CONNECT_PLATFORM,
	WIFI_CONNECT_PLATFORM,
}WIFI_Connect_Status_t;

typedef enum
{
	PLATFORM_SET_DISARM=0,
	PLATFORM_SET_ARM,
	PLATFORM_SET_HOME_ARM,
}Platform_Set_Status_t;

typedef enum
{
	WIFI_NO_CONFIG=0,
	WIFI_EXECUTE_CONFIG,
	WIFI_CONFIG_SUCCESS,
}WIFI_Config_Status_t;

typedef enum
{
	RESULT_SUCCESS=0,
	RESULT_PASSWORD_ERROR,
	RESULT_RC_FULL,
	RESULT_ZONE_FULL,
	RESULT_ID_ERROR,
	RESULT_RC_NULL,
	RESULT_ZONE_NULL,
	RESULT_RF_REGISTERED,
	RESULT_FORMAT_ERROR,
	RESULT_DOOR_BELL_NULL,
	RESULT_DOOR_BELL_FULL,
	RESULT_PART_NULL,
}Result_Val_t;

typedef struct{
	unsigned char cStatus;
	unsigned char cGetIpTimeOut;
	unsigned char cStartResetTimeOut;
	unsigned char cResetTimeOut;
	unsigned char cForceReset;
	unsigned char cBeepControl;
	unsigned char cTcpRespond;
	unsigned char cTcpSendFailCount;
	unsigned char cEntrySetConnectApTimeout;
	unsigned char cSetConnectApFlag;
	unsigned char cPlatformConnectStatus;
	unsigned char cCheckPlatformConnectInterval;
	unsigned char cDeviceInforRespondTimeOut;		
} WIFI_Var_t;	

extern xdata WIFI_Var_t g_WifiVar;

void WifiPrintf( unsigned char *pFormat, ...);
void WifiDisable(void);
void WifiInit(void);
void WifiRoutineHandle(void);
void WifiStartupReset(void);
void WifiReset(void);
void WifiEntrySetConnectAp(void);
void WifiExitSetConnectAp(void);
void WifiSetConnectApSuccess(void);
void WifiPowerOn(void);
void WifiDisconnect(void);
void WifiConnected(void);
void WifiSetCmsAdr(void);
void WifiGetIpInfor(unsigned char *pBuf);
void WifiInquiryIpInfor(void);
void WifiLostConnectPlatform(void);
void WifiConnectPlatform(void);
void WifiConfig(void);
void WifiDefault(void);
void WifiTcpReconnect(void);
void WifiUploadEventToPlatform(void);
void WifiReceiveDataHandle(void);
void WifiApStationDisconnected(void);
void WifiApStationConnected(void);
void WifiGetIP(void);
void WifiCheckPlatformConnect(void);
void WifiSendDeviceInfor(void);
#endif

