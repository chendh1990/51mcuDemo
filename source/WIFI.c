#include "sys.h"
#include "SysClk.h"
#include "timer.h"
#include "key.h"
#include "uart.h"
#include "WIFI.h"
#include "SysClk.h"
#include "led.h"
#include "RF_remote.h"
#include "eeprom.h"
#include "HistoryLog.h"
#include "tone.h"
#include "power.h"

xdata WIFI_Var_t g_WifiVar;

void WifiPrintf( unsigned char *pFormat, ...)
{
	va_list vallist;
	
	va_start(vallist,pFormat);
	UsartPrint(UART_WIFI, pFormat, vallist);
	if(g_SystemParam.DebugInforOutput == ENABLE)
	{
		UsartPrint(UART_PC, pFormat, vallist);
	}
	va_end(vallist);
}

void WifiEnable(void)
{
	p_WIFI_EN = LEVEL_HIGH;
}

void WifiDisable(void)
{
	p_WIFI_EN = LEVEL_LOW;
}

void WifiInit(void)
{
	g_WifiVar.cStatus = WIFI_NULL;
	g_WifiVar.cGetIpTimeOut = 0;
	g_WifiVar.cResetTimeOut = 0; //;WIFI_RESET_TIMEOUT;
	g_WifiVar.cTcpRespond = 0;
	g_SystemVar.UploadPlatformControl = FALSE;
	g_WifiVar.cTcpSendFailCount = 0;
	g_WifiVar.cEntrySetConnectApTimeout = 0;
	g_WifiVar.cSetConnectApFlag = FALSE;
	g_WifiVar.cCheckPlatformConnectInterval = 0;
	g_WifiVar.cDeviceInforRespondTimeOut = 0;
	P2M0 |= 0x40; 	
	P2M1 &= 0xBF; 	
	p_WIFI_RST = LEVEL_HIGH;		
	P2M0 |= 0x80; 	
	P2M1 &= 0x7F; 	
	if(g_SystemVar.ACStatus == POWER_DC_IN)
	{
		WifiEnable();
		g_WifiVar.cForceReset = 3;
	}
	else
	{
		WifiDisable();
		g_WifiVar.cForceReset = 0;
	}
}

void WifiRoutineHandle(void)
{
	if(g_WifiVar.cForceReset > 0)
	{
		if(--g_WifiVar.cForceReset == 0)
		{
			HostMsgPost(SYS_MSG_WIFI_STARTUP_RESET, NULL);
		}
	}

	if(g_WifiVar.cStartResetTimeOut > 0)
	{
		if(--g_WifiVar.cStartResetTimeOut == 0)
		{
			HostMsgPost(SYS_MSG_WIFI_RESET, NULL);
		}
	}
	
	if(g_WifiVar.cResetTimeOut > 0)
	{
		if(--g_WifiVar.cResetTimeOut == 0)
		{
			g_WifiVar.cResetTimeOut = WIFI_RESET_TIMEOUT;
			HostMsgPost(SYS_MSG_WIFI_STARTUP_RESET, NULL);
		}
	}
	
	if(g_WifiVar.cDeviceInforRespondTimeOut > 0)
	{
		if(--g_WifiVar.cDeviceInforRespondTimeOut == 0)
		{
			HostMsgPost(SYS_MSG_WIFI_SEND_DEVICE_INFOR, NULL);
			g_WifiVar.cDeviceInforRespondTimeOut = WIFI_DEVICE_INFOR_RESPOND_TIMEOUT;
		}
	}
	
	if(g_WifiVar.cEntrySetConnectApTimeout > 0)
	{
		g_WifiVar.cEntrySetConnectApTimeout--;
		if(g_WifiVar.cEntrySetConnectApTimeout == 0)
		{
			HostMsgPost(SYS_MSG_WIFI_EXIT_SET_AP_CONNECTED, NULL);
		}
		else 
		{
			if(g_WifiVar.cBeepControl == TRUE)
			{
				HostMsgPost(SYS_MSG_PLAY_TONE, BEEP_NORMAL);
			}	
		}
	}

	if(g_WifiVar.cGetIpTimeOut > 0)
	{
		if(--g_WifiVar.cGetIpTimeOut == 0)
		{
			g_WifiVar.cResetTimeOut = WIFI_RESET_TIMEOUT;
			HostMsgPost(SYS_MSG_WIFI_STARTUP_RESET, NULL);
		}
	}

	if(g_WifiVar.cTcpRespond > 0)
	{
		if(--g_WifiVar.cTcpRespond == 0)
		{
			AlarmInforPush(g_SystemVar.UploadAlarmInforBackup.Case, g_SystemVar.UploadAlarmInforBackup.Name, g_SystemVar.UploadAlarmInforBackup.Locate);
			g_SystemVar.UploadPlatformControl = TRUE;
			g_WifiVar.cTcpSendFailCount++;
			if(g_WifiVar.cTcpSendFailCount == MAX_TCP_SEND_FAIL_TIME)
			{
				HostMsgPost(SYS_MSG_WIFI_STARTUP_RESET, NULL);
			}
			
		}
	}	

	if(g_WifiVar.cPlatformConnectStatus == FALSE)
	{
		if(g_WifiVar.cCheckPlatformConnectInterval > 0)
		{
			if(--g_WifiVar.cCheckPlatformConnectInterval == 0)
			{
				g_WifiVar.cCheckPlatformConnectInterval = WIFI_CHECK_PLATFORM_CONNECT_INTERVAL;
				HostMsgPost(SYS_MSG_WIFI_CHECK_PLATFORM_CONNECT_STATUS, NULL);
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////
void WifiSendDeviceInfor(void)
{
	SysPrintf("\r\nWifiSendDeviceInfor.");
	WifiPrintf("DeviceInfor:\"deviceid\":\"%x%x%x%x%x%x\",\"modelid\":\"%s\"\r\n",	\
		g_SystemVar.DeviceID[0],	\
		g_SystemVar.DeviceID[1],	\
		g_SystemVar.DeviceID[2],	\
		g_SystemVar.DeviceID[3],	\
		g_SystemVar.DeviceID[4],	\
		g_SystemVar.DeviceID[5],	\
		MODULE_NAME);
	g_WifiVar.cDeviceInforRespondTimeOut = WIFI_DEVICE_INFOR_RESPOND_TIMEOUT;
}

void WifiPowerOn(void)
{
	SysPrintf("\r\nWifiPowerOn.");
	g_WifiVar.cGetIpTimeOut = 0;
	g_WifiVar.cResetTimeOut = WIFI_RESET_TIMEOUT;
	HostMsgPost(SYS_MSG_WIFI_SEND_DEVICE_INFOR, NULL);
}

void WifiDisconnect(void)
{
	SysPrintf("\r\nWifiDisconnect.");
	if(g_WifiVar.cStatus == WIFI_DISCONNECT)
		return;	
	g_WifiVar.cPlatformConnectStatus = FALSE;
	g_SystemVar.UploadPlatformControl = FALSE;
	g_WifiVar.cTcpRespond = 0;
	g_WifiVar.cGetIpTimeOut = 0;
	g_WifiVar.cResetTimeOut = WIFI_RESET_TIMEOUT;
	g_WifiVar.cStatus = WIFI_DISCONNECT;
	LedBlinkSet(LED_WIFI_INDICATOR, LED_FAST_BLINK);
	
}

void WifiConnected(void)
{
	SysPrintf("\r\nWifiConnected.");
	
	if(g_WifiVar.cStatus == WIFI_CONNECTED)
		return;	
	g_WifiVar.cGetIpTimeOut = WIFI_GET_IP_TIMEOUT;
	g_WifiVar.cStatus = WIFI_CONNECTED;
	g_WifiVar.cSetConnectApFlag = FALSE;
	g_WifiVar.cEntrySetConnectApTimeout = 0;
	LedBlinkSet(LED_WIFI_INDICATOR, LED_NO_BLINK);
	LedIndicator(LED_WIFI_INDICATOR, LED_ON);
	
}

void WifiGetIP(void)
{
	SysPrintf("\r\nWifiGetIP.");
	g_WifiVar.cGetIpTimeOut = 0;
	g_WifiVar.cPlatformConnectStatus = FALSE;
	g_WifiVar.cCheckPlatformConnectInterval = 10;
	HostMsgPost(SYS_MSG_WIFI_CONFIG, NULL);
	
}

void WifiCheckPlatformConnect(void)
{
	WifiPrintf("CheckPlatformConnect\r\n");
}

void WifiApStationDisconnected(void)
{
	
	
}

void WifiApStationConnected(void)
{
	g_WifiVar.cBeepControl = FALSE;
}

void WifiStartupReset(void)
{
	SysPrintf("\r\nSartup WIFI RESET.");
	g_WifiVar.cStatus = WIFI_NULL;
	g_WifiVar.cGetIpTimeOut = 0;
	g_WifiVar.cResetTimeOut = 0; //;WIFI_RESET_TIMEOUT;
	g_WifiVar.cTcpRespond = 0;
	g_SystemVar.UploadPlatformControl = FALSE;
	g_WifiVar.cTcpSendFailCount = 0;
	g_WifiVar.cEntrySetConnectApTimeout = 0;
	g_WifiVar.cSetConnectApFlag = FALSE;
	g_WifiVar.cCheckPlatformConnectInterval = 0;
	g_WifiVar.cDeviceInforRespondTimeOut = 0;
	WifiEnable();
	p_WIFI_RST = LEVEL_LOW;
	g_WifiVar.cStartResetTimeOut = WIFI_START_RESET_TIMEOUT;
}

void WifiReset(void)
{
	SysPrintf("\r\nWIFI RESET.");
	WifiEnable();
	p_WIFI_RST = LEVEL_HIGH;
	g_WifiVar.cStatus = WIFI_NULL;
	g_WifiVar.cSetConnectApFlag = FALSE;
}

void WifiLostConnectPlatform(void)
{
	SysPrintf("\r\nWifiLostConnectPlatform.");

	if(g_WifiVar.cStatus == WIFI_LOST_CONNECT_PLATFORM)
		return;
	g_WifiVar.cPlatformConnectStatus = FALSE;
	g_WifiVar.cCheckPlatformConnectInterval = WIFI_CHECK_PLATFORM_CONNECT_INTERVAL;
	g_WifiVar.cStatus = WIFI_LOST_CONNECT_PLATFORM;
	g_SystemVar.UploadPlatformControl = FALSE;
	LedBlinkSet(LED_WIFI_INDICATOR, LED_NO_BLINK);
	LedIndicator(LED_WIFI_INDICATOR, LED_ON);
}

void WifiConnectPlatform(void)
{
	SysPrintf("\r\nWifiConnectPlatform.");
	g_WifiVar.cResetTimeOut = 0;
	g_WifiVar.cTcpRespond = 0;
	g_SystemVar.UploadPlatformControl = TRUE;
	if(g_WifiVar.cStatus == WIFI_CONNECT_PLATFORM)
		return;
	g_WifiVar.cPlatformConnectStatus = TRUE;
	g_WifiVar.cCheckPlatformConnectInterval = 0;
	g_WifiVar.cStatus = WIFI_CONNECT_PLATFORM;
	LedBlinkSet(LED_WIFI_INDICATOR, LED_SLOW_BLINK);
	
}

void WifiCheckConnectPlatformRespond(unsigned char *pBuf)
{
	if(*pBuf == '1')
	{
		SysPrintf("\r\nrespond Platform:1.");
		g_WifiVar.cPlatformConnectStatus = TRUE;
		g_WifiVar.cCheckPlatformConnectInterval = 0;
		HostMsgPost(SYS_MSG_WIFI_CONNECT_PLATFORM, NULL);
	}
	else
	{
		SysPrintf("\r\nrespond Platform:0.");
		g_WifiVar.cPlatformConnectStatus = FALSE;
		g_WifiVar.cCheckPlatformConnectInterval = WIFI_CHECK_PLATFORM_CONNECT_INTERVAL;
	}
	
}
void WifiConfig(void)
{	
	WifiPrintf("ConnectConfig:%c%c%c%c%c%c%c%c%c%c\r\n",	\
	g_SystemParam.TcpIpServerAdr[0],	\
	g_SystemParam.TcpIpServerAdr[1],	\
	g_SystemParam.TcpIpServerAdr[2],	\
	g_SystemParam.TcpIpServerAdr[3],	\
	(unsigned char)(g_SystemParam.TcpIpServerPort&0xff),	\
	(unsigned char)(g_SystemParam.TcpIpServerPort>>8),	\
	(unsigned char)(g_SystemParam.UdpLocalPort&0xff),		\
	(unsigned char)(g_SystemParam.UdpLocalPort>>8),		\
	(unsigned char)(g_SystemParam.UdpSmartLinkPort&0xff),		\
	(unsigned char)(g_SystemParam.UdpSmartLinkPort>>8));
	SysPrintf("\r\nWifiConfig.");
}

void WifiEntrySetConnectAp(void)
{
	if(g_WifiVar.cSetConnectApFlag == FALSE)
	{
		WifiPrintf("WifiEntrySetConnectAp\r\n");
		SysPrintf("\r\nWifiEntrySetConnectAp\r\n");
	}
}

void WifiExitSetConnectAp(void)
{
	g_WifiVar.cSetConnectApFlag = FALSE;
	g_WifiVar.cEntrySetConnectApTimeout = 0;
	g_WifiVar.cBeepControl = FALSE;
#if 1
	HostMsgPost(SYS_MSG_WIFI_STARTUP_RESET, NULL);
#else
	WifiPrintf("WifiExitSetConnectAp\r\n");
#endif	
	SysPrintf("\r\nWifiExitSetConnectAp\r\n");
}

void WifiSetConnectApSuccess(void)
{
	g_WifiVar.cSetConnectApFlag = FALSE;
	g_WifiVar.cEntrySetConnectApTimeout = 0;
	g_WifiVar.cBeepControl = FALSE;
}

void WifiSetCmsAdr(void)
{
#if 1
	g_WifiVar.cForceReset = 2;
#else
	SysPrintf("\r\nSetCmsAdr");
	WifiPrintf("SetCmsAdr%c%c%c%c%c%c\r\n", g_SystemParam.TcpIpServerAdr[0], g_SystemParam.TcpIpServerAdr[1], g_SystemParam.TcpIpServerAdr[2], g_SystemParam.TcpIpServerAdr[3], (unsigned char)(g_SystemParam.TcpIpServerPort>>8), (unsigned char)(g_SystemParam.TcpIpServerPort));
#endif
}

void WifiTcpSendSuccess(void)
{
	//g_WifiVar.cTcpRespond = 0;
}

void WifiTcpReconnect(void)
{
	SysPrintf("TcpReconnect\r\n");
	WifiPrintf("TcpReconnect\r\n");
}

void WifiDefault(void)
{
	SysPrintf("WifiDefault\r\n");
	WifiPrintf("WifiDefault\r\n");
}

void WifiUploadEventToPlatform(void)
{
	unsigned char CheckSum;
	unsigned char TempBuf[3];
	unsigned char Index;
	unsigned char Len;
	unsigned char Type;
	unsigned char *pComBuf;
	unsigned char *pTemp;

	g_SystemVar.UploadPlatformControl = FALSE;
	g_SystemVar.AutoUploadTime = g_SystemParam.AutoUploadTime;
	memset(g_HostCommonBuf, END_FLAG, HOST_COMMON_BUF_SIZE);
	pComBuf = g_HostCommonBuf;
	Len = 0;
	pTemp = pComBuf;
	///////////PACKET HEADER///////////////////////
	memcpy(pTemp, "POST:N37XX", strlen("POST:N37XX"));
	pTemp += strlen("POST:N37XX");
	Len += strlen("POST:N37XX");
	///////////HOST MAC ADDRESS///////////////////
	memcpy(pTemp, "&MAC=", strlen("&MAC="));
	pTemp += strlen("&MAC=");
	Len += strlen("&MAC=");
	for(Index=0; Index<MAC_ADDRESS_LENGTH; Index++)
	{
		HexToAsc(TempBuf, g_SystemParam.HostMacAdr[Index]);
		*pTemp++ = TempBuf[0];
		*pTemp++ = TempBuf[1];
		Len += 2;
	}
	///////////HOST ID/////////////////////////////
	memcpy(pTemp, "&ID=", strlen("&ID="));
	pTemp += strlen("&ID=");
	Len += strlen("&ID=");
	for(Index=0; Index<HOST_ID_LENGTH; Index++)
	{
		HexToAsc(TempBuf, g_SystemParam.HostID[Index]);
		*pTemp++ = TempBuf[0];
		*pTemp++ = TempBuf[1];
		Len += 2;
	}
	///////////PASSWORD ///////////////////
	memcpy(pTemp, "&PWD=", strlen("&PWD="));
	pTemp += strlen("&PWD=");
	Len += strlen("&PWD=");
	memcpy(pTemp, g_SystemParam.SystemPassword, PASSWORD_LENGTH);
	pTemp += PASSWORD_LENGTH;
	Len += PASSWORD_LENGTH;
	///////////LANGUAGE/////////////////////////////////////
	memcpy(pTemp, "&LAN=", strlen("&LAN="));
	pTemp += strlen("&LAN=");
	Len += strlen("&LAN=");
	*pTemp++ = HEX[(g_SystemParam.SystemLanguage>>4)&0x0f];
	*pTemp++ = HEX[g_SystemParam.SystemLanguage&0x0f];
	Len += 2;

	///////////RF NAME /////////////////////////////////////
	AlarmInforPop(TempBuf);
	g_SystemVar.UploadAlarmInforBackup.Case = TempBuf[0];
	g_SystemVar.UploadAlarmInforBackup.Name = TempBuf[1];
	g_SystemVar.UploadAlarmInforBackup.Locate = TempBuf[2];
	if((TempBuf[2] <= MAX_STORE_WIRELESS_ACCESSORY_NUM)
		&&((TempBuf[0]==CASE_ALARM) || (TempBuf[0]==CASE_SOS) || (TempBuf[0]==CASE_MOVE) || (TempBuf[0]==CASE_LB)))
	{
		if(TempBuf[1] == N_RT)
		{
			EE_Read_Data(EE_REMOTE_CONTROL_START_ADDRESS+(TempBuf[2]-1)*sizeof(RF_Store_t), (unsigned char *)&g_RFStore, sizeof(RF_Store_t));
		}
		else if(TempBuf[1] == N_DOOR_BELL)
		{
			EE_Read_Data(EE_WIRELESS_DOOR_BELL_START_ADDRESS+(TempBuf[2]-1)*sizeof(RF_Store_t), (unsigned char *)&g_RFStore, sizeof(RF_Store_t));
		}
		else 
		{
			EE_Read_Data(EE_WIRELESS_ACCESSORY_START_ADDRESS+(TempBuf[2]-1)*sizeof(RF_Store_t), (unsigned char *)&g_RFStore, sizeof(RF_Store_t));
		}

		memcpy(pTemp, "&partid=", strlen("&partid="));
		pTemp += strlen("&partid=");
		Len += strlen("&partid=");
		for(Index=0; Index<RF_CODE_LENGTH; Index++)
		{
			*pTemp++ = HEX[(g_RFStore.RFCode[Index]>>4)&0x0f];
			*pTemp++ = HEX[g_RFStore.RFCode[Index]&0x0f];
			Len += 2;
		}

		if(g_RFStore.NameFlag == TRUE)
		{
			memcpy(pTemp, "&alarmclass=", strlen("&alarmclass="));
			pTemp += strlen("&alarmclass=");
			Len += strlen("&alarmclass=");
			if(g_RFStore.Type == N_RT)
			{
				EE_Read_Data(EE_REMOTE_CONTROL_NAME_ADDRESS+(g_RFStore.CodeLocate-1)*RF_NAME_LENGHT, pTemp, RF_NAME_LENGHT);
			}
			else if(g_RFStore.Type == N_DOOR_BELL)
			{
				EE_Read_Data(EE_WIRELESS_DOOR_BELL_NAME_ADDRESS+(g_RFStore.CodeLocate-1)*RF_NAME_LENGHT, pTemp, RF_NAME_LENGHT);
			}
			else
			{
				EE_Read_Data(EE_ZONE_NAME_FIRST_START_ADDRESS+(g_RFStore.CodeLocate-1)*RF_NAME_LENGHT, pTemp, RF_NAME_LENGHT);
			}

			for(Index=0; Index<RF_NAME_UNICODE_LENGHT; Index++)
			{
				if((*pTemp == END_FLAG)&&(*(pTemp+1) == END_FLAG))
				{
					break;
				}
				
				pTemp += 2;
				Len += 2;
			}
		}
	}	
	///////////CASE  NAME  ZONE ///////////////////
	memcpy(pTemp, "&alarm=", strlen("&alarm="));
	pTemp += strlen("&alarm=");
	Len += strlen("&alarm=");
	//AlarmInforPop(TempBuf);
	*pTemp++ = TempBuf[0] + '0';
	*pTemp++ = TempBuf[1] + '0';
	Type = TempBuf[1];
	Len += 2;
	Index = TempBuf[2];
	HexToAsc(TempBuf, Index);
	*pTemp++ = TempBuf[0];
	*pTemp++ = TempBuf[1];
	Len += 2;
	memcpy(pTemp, "0000", strlen("0000"));
	pTemp += strlen("0000");
	Len += strlen("0000");
	////////////CACULATE CHECKSUM////////////////
	Len += 4; // "ck\r\n" ck=checksum value(2byte)
	HexToAsc(TempBuf, Len);
	pComBuf[8] = TempBuf[0]; // 9=1 + strlen("POST:N37")
	pComBuf[9] = TempBuf[1];
	CheckSum = 0;
	for(Index=0; Index<(Len-4); Index++)
	{
		CheckSum += pComBuf[Index];
	}	
	HexToAsc(TempBuf, CheckSum);
	*pTemp++ = TempBuf[0];
	*pTemp++ = TempBuf[1];
	*pTemp++ = '\r';
	*pTemp++ = '\n';
	WifiPrintf("TcpSend:%e\r\n", pComBuf);
	g_WifiVar.cTcpRespond = MAX_WAIT_TCP_RESPOND_TIME;
}

///////////////APP command handle set//////////////////////
unsigned char WifiGetDataFieldLen(unsigned char *pBuf, unsigned char EndChar)
{
	unsigned char DataLen;
	
	DataLen = 0;

	while(*pBuf != EndChar)
	{
		pBuf++;
		DataLen++;
		if(DataLen>200)
		{
			return 0;
		}
	}
	return DataLen;
}

unsigned char WifiGetUnicodeFieldLen(unsigned char *pBuf, unsigned char EndChar)
{
	unsigned char DataLen;
	
	DataLen = 0;

	while(*pBuf != EndChar)
	{
		pBuf += 2;
		DataLen += 2;
		if(DataLen>250)
		{
			return 0;
		}
	}
	return DataLen;
}
void AppHostFixRespond(unsigned char *pBuf, unsigned char ReturnCode)
{
	unsigned char *pLocate;
	unsigned char Len;
	
	WifiPrintf("{\"result\":");
	switch(ReturnCode)
	{
		case RESULT_SUCCESS:
			WifiPrintf("\"0\"");
			break;

		case RESULT_PASSWORD_ERROR:
			WifiPrintf("\"-1\"");
			break;	

		case RESULT_RC_FULL:
			WifiPrintf("\"-2\"");
			break;		

		case RESULT_ZONE_FULL:
			WifiPrintf("\"-3\"");
			break;	

		case RESULT_ID_ERROR:
			WifiPrintf("\"-4\"");
			break;	

		case RESULT_RC_NULL:
			WifiPrintf("\"-5\"");
			break;		

		case RESULT_ZONE_NULL:
			WifiPrintf("\"-6\"");
			break;
			
		case RESULT_RF_REGISTERED:
			WifiPrintf("\"-7\"");
			break;	
			
		case RESULT_FORMAT_ERROR:
			WifiPrintf("\"-8\"");
			break;
			
		case RESULT_DOOR_BELL_NULL:
			WifiPrintf("\"-9\"");
			break;	
			
		case RESULT_DOOR_BELL_FULL:
			WifiPrintf("\"-10\"");
			break;

		case RESULT_PART_NULL:
			WifiPrintf("\"-11\"");
			break;	
			
		default:
			WifiPrintf("\"0\"");
			break;
	}
	
	WifiPrintf(",\"deviceid\":\"%x%x%x%x%x%x\",\"modelid\":\"%s\",",	\
		g_SystemVar.DeviceID[0],	\
		g_SystemVar.DeviceID[1],	\
		g_SystemVar.DeviceID[2],	\
		g_SystemVar.DeviceID[3],	\
		g_SystemVar.DeviceID[4],	\
		g_SystemVar.DeviceID[5],	\
		MODULE_NAME);
	
	pLocate = strstr(pBuf, "\"userid\":\"");
	if(pLocate != NULL)
	{
		pLocate += strlen("\"userid\":\"");
		Len = WifiGetDataFieldLen(pLocate, '"');
		pLocate[Len] = '\0';
		WifiPrintf("\"userid\":\"%s\",", pLocate);
		pLocate[Len] = '"';
	}
	
	pLocate = strstr(pBuf, "\"phoneid\":\"");
	if(pLocate != NULL)
	{
		pLocate += strlen("\"phoneid\":\"");
		Len = WifiGetDataFieldLen(pLocate, '"');
		pLocate[Len] = '\0';
		WifiPrintf("\"phoneid\":\"%s\"", pLocate);
		pLocate[Len] = '"';
	}

}

void AppSearchHost(unsigned char *pBuf, unsigned char ComMode)
{
	if(ComMode == TCP_MODE)
	{
		WifiPrintf("TcpSend:");
		
	}
	else
	{
		WifiPrintf("UdpSend:");
	}
	AppHostFixRespond(pBuf, RESULT_SUCCESS);
	WifiPrintf(",\"sa_ip\":\"%d.%d.%d.%d\"",	\
		g_SystemParam.HostIPAdr[0],	\
		g_SystemParam.HostIPAdr[1],	\
		g_SystemParam.HostIPAdr[2],	\
		g_SystemParam.HostIPAdr[3]);
	WifiPrintf("}\r\n");
}

void AppLoginHost(unsigned char *pBuf, unsigned char ComMode)
{
	unsigned char *pLocate;
	
	if(ComMode == TCP_MODE)
	{
		WifiPrintf("TcpSend:");
	}
	else
	{
		WifiPrintf("UdpSend:");
	}
	pLocate = strstr(pBuf, "\"password\":\"");
	pLocate += strlen("\"password\":\"");
	if(memcmp(g_SystemParam.SystemPassword, pLocate, PASSWORD_LENGTH) ==0)	
	{
		AppHostFixRespond(pBuf, RESULT_SUCCESS);
		WifiPrintf(",\"sa_ctrl\":\"%d\"", g_SystemParam.SystemStatus);
	}
	else
	{
		AppHostFixRespond(pBuf, RESULT_PASSWORD_ERROR);
	}	
		
	WifiPrintf("}\r\n");
}

void AppInquerySetSysSetting(unsigned char *pBuf, unsigned char ComMode)
{
	unsigned char *pLocate;
	unsigned char Temp;
	
	if(ComMode == TCP_MODE)
	{
		WifiPrintf("TcpSend:");
	}
	else
	{
		WifiPrintf("UdpSend:");
	}	
	AppHostFixRespond(pBuf, RESULT_SUCCESS);

	pLocate = strstr(pBuf, "\"sa_sinretime\":");
	if(pLocate != NULL)
	{
		pLocate += strlen("\"sa_sinretime\":\"");
		g_SystemParam.SirenLastingTime = *pLocate-'0';
		pLocate = strstr(pBuf, "\"sa_rtimes\":");
		if(pLocate != NULL)
		{
			pLocate += strlen("\"sa_rtimes\":\"");
			g_SystemParam.RingTime = *pLocate -'0';
		}

		pLocate = strstr(pBuf, "sa_delayentry\":");
		if(pLocate != NULL)
		{
			pLocate += strlen("sa_delayentry\":\"");
			Temp = WifiGetDataFieldLen(pLocate, '"');
			if(Temp >= 3)
			{
				g_SystemParam.DelayAlarmTime = (*pLocate-'0')*100 + (*(pLocate+1)-'0')*10 + (*(pLocate+2)-'0');
			}
			else if(Temp == 2)
			{
				g_SystemParam.DelayAlarmTime = (*pLocate-'0')*10 + (*(pLocate+1)-'0');
			}
			else if(Temp == 1)
			{
				g_SystemParam.DelayAlarmTime = (*pLocate-'0');
			}
		}
		//delay arm
		pLocate = strstr(pBuf, "sa_delayexit");
		if(pLocate != NULL)
		{
			pLocate += strlen("sa_delayexit\":\"");
			Temp = WifiGetDataFieldLen(pLocate, '"');
			if(Temp >= 3)
			{
				g_SystemParam.DelayArmTime = (*pLocate-'0')*100 + (*(pLocate+1)-'0')*10 +(*(pLocate+2)-'0');
			}
			else if(Temp == 2)
			{
				g_SystemParam.DelayArmTime = (*pLocate-'0')*10 + (*(pLocate+1)-'0');
			}
			else if(Temp == 1)
			{
				g_SystemParam.DelayArmTime = (*pLocate-'0');
			}
		}
		
		pLocate = strstr(pBuf, "sa_redialtime\":");
		if(pLocate != NULL)
		{
			pLocate += strlen("sa_redialtime\":\"");
			g_SystemParam.CallRecycleTime = *pLocate -'0';
		}	

		pLocate = strstr(pBuf, "sa_autoarm\":");
		if(pLocate != NULL)
		{
			pLocate += strlen("sa_autoarm\":\""); 
			Temp = WifiGetDataFieldLen(pLocate, ':');
			if(Temp >= 2)
			{
				g_SystemParam.AutoArmTime = (*pLocate-'0')*10 + (*(pLocate+1)-'0');
			}
			else if(Temp == 1)
			{
				g_SystemParam.AutoArmTime = (*pLocate-'0');
			}
			g_SystemParam.AutoArmTime <<= 8;
			pLocate += (Temp+1); 
			Temp = WifiGetDataFieldLen(pLocate, '"');
			if(Temp >= 2)
			{
				g_SystemParam.AutoArmTime += ((*pLocate-'0')*10 + (*(pLocate+1)-'0'));
			}
			else if(Temp == 1)
			{
				g_SystemParam.AutoArmTime += (*pLocate-'0');
			}
		}
		
		pLocate = strstr(pBuf, "sa_autodisarm\":");
		if(pLocate != NULL)
		{
			pLocate += strlen("sa_autodisarm\":\""); 
			Temp = WifiGetDataFieldLen(pLocate, ':');
			if(Temp >= 2)
			{
				g_SystemParam.AutoDisarmTime = (*pLocate-'0')*10 + (*(pLocate+1)-'0');
			}
			else if(Temp == 1)
			{
				g_SystemParam.AutoDisarmTime = (*pLocate-'0');
			}
			g_SystemParam.AutoDisarmTime <<= 8;
			pLocate += (Temp+1); 
			Temp = WifiGetDataFieldLen(pLocate, '"');
			if(Temp >= 2)
			{
				g_SystemParam.AutoDisarmTime += ((*pLocate-'0')*10 + (*(pLocate+1)-'0'));
			}
			else if(Temp == 1)
			{
				g_SystemParam.AutoDisarmTime += (*pLocate-'0');
			}
		}
		pLocate = strstr(pBuf, "sa_autoarmdisarmctrl\":");
		if(pLocate != NULL)
		{
			pLocate += strlen("sa_autoarmdisarmctrl\":\"");
			g_SystemParam.AutoArmDisarmFlag = *pLocate -'0';
		}	

		pLocate = strstr(pBuf, "\"sa_rctone\":");
		if(pLocate != NULL)
		{
			pLocate += strlen("\"sa_rctone\":\"");
			g_SystemParam.AccompanyToneFlag = *pLocate -'0';
		}

		pLocate = strstr(pBuf, "\"sa_armdisarmsmsinfor\":");
		if(pLocate != NULL)
		{
			pLocate += strlen("\"sa_armdisarmsmsinfor\":\"");
			g_SystemParam.ArmDisarmSmsInforFlag = *pLocate -'0';
		}
		
		pLocate = strstr(pBuf, "\"sa_acsmsinfor\":");
		if(pLocate != NULL)
		{
			pLocate += strlen("\"sa_acsmsinfor\":\"");
			g_SystemParam.AcSwitchInforFlag = *pLocate -'0';
		}


		pLocate = strstr(pBuf, "\"sa_language\":");
		if(pLocate != NULL)
		{
			pLocate += strlen("\"sa_language\":\"");
			g_SystemParam.SystemLanguage = *pLocate -'0';
		}	
		
		SysParamToEeprom();
	}
	else
	{
		WifiPrintf(",\"sa_sinretime\":\"%d\",\"sa_rtimes\":\"%d\",\"sa_delayexit\":\"%d\",\"sa_delayentry\":\"%d\",\"sa_redialtime\":\"%d\",\"sa_autoarmdisarmctrl\":\"%d\",\"sa_rctone\":\"%d\",\"sa_armdisarmsmsinfor\":\"%d\",\"sa_acsmsinfor\":\"%d\",\"sa_language\":\"%d\"",	\
			g_SystemParam.SirenLastingTime,	\
			g_SystemParam.RingTime,	\
			g_SystemParam.DelayArmTime ,	\
			g_SystemParam.DelayAlarmTime,	\
			g_SystemParam.CallRecycleTime,	\
			g_SystemParam.AutoArmDisarmFlag,	\
			g_SystemParam.AccompanyToneFlag,	\
			g_SystemParam.ArmDisarmSmsInforFlag,	\
			g_SystemParam.AcSwitchInforFlag,	\
			g_SystemParam.SystemLanguage);
		WifiPrintf(",\"sa_autoarm\":\"%d%d:%d%d\",\"sa_autodisarm\":\"%d%d:%d%d\""		\
			, ((unsigned char)(g_SystemParam.AutoArmTime>>8))/10						\
			, ((unsigned char)(g_SystemParam.AutoArmTime>>8))%10						\
			, ((unsigned char)g_SystemParam.AutoArmTime)/10							\
			, ((unsigned char)g_SystemParam.AutoArmTime)%10							\
			, ((unsigned char)(g_SystemParam.AutoDisarmTime>>8))/10						\
			, ((unsigned char)(g_SystemParam.AutoDisarmTime>>8))%10						\
			, ((unsigned char)g_SystemParam.AutoDisarmTime)/10							\
			, ((unsigned char)g_SystemParam.AutoDisarmTime)%10);
	}
	
	WifiPrintf("}\r\n");
	
}

void AppInquerySetAlarmHost(unsigned char *pBuf, unsigned char ComMode)
{
	unsigned char *pLocate;
	unsigned char Index;
	unsigned char Temp;

	if(ComMode == TCP_MODE)
	{
		WifiPrintf("TcpSend:");
	}
	else
	{
		WifiPrintf("UdpSend:");
	}	
	AppHostFixRespond(pBuf, RESULT_SUCCESS);

	pLocate = strstr(pBuf, "\"sa_selfcheck\":");
	if(pLocate != NULL)
	{
		pLocate += strlen("\"sa_selfcheck\":\"");
		Temp = WifiGetDataFieldLen(pLocate, '"');
		if(Temp >= 4)
		{
			g_SystemParam.AutoUploadTime = (*pLocate-'0')*1000 + (*(pLocate+1)-'0')*100 + (*(pLocate+2)-'0')*10 + (*(pLocate+3)-'0');
		}
		else if(Temp == 3)
		{
			g_SystemParam.AutoUploadTime = (*pLocate-'0')*100 + (*(pLocate+1)-'0')*10 + (*(pLocate+2)-'0');
		}
		else if(Temp == 2)
		{
			g_SystemParam.AutoUploadTime = (*pLocate-'0')*10 + (*(pLocate+1)-'0');
		}
		else if(Temp == 1)
		{
			g_SystemParam.AutoUploadTime = (*pLocate-'0');
		}

		if(g_SystemParam.AutoUploadTime == 0)
		{
			g_SystemParam.AutoUploadTime = 3;
		}
	
		pLocate = strstr(pBuf, "\"sa_hostid\":");
		if(pLocate != NULL)
		{
			pLocate += strlen("\"sa_hostid\":\"");
			g_SystemParam.HostID[0] = AscStrToByte((unsigned char *)pLocate);
			pLocate += 2;
			g_SystemParam.HostID[1] = AscStrToByte((unsigned char *)pLocate);
			pLocate += 2;
			g_SystemParam.HostID[2] = AscStrToByte((unsigned char *)pLocate);
		}

		pLocate = strstr(pBuf, "\"sa_cmsip\":");
		if(pLocate != NULL)
		{
			pLocate += strlen("\"sa_cmsip\":\"");
			for(Index=0; Index<IP_ADDRESS_LENGTH; Index++)
			{
				if(Index==IP_ADDRESS_LENGTH-1)
					Temp = WifiGetDataFieldLen(pLocate, '"');
				else
					Temp = WifiGetDataFieldLen(pLocate, '.');
				if(Temp == 3)
				{
					g_SystemParam.TcpIpServerAdr[Index] = (*pLocate-'0')*100 + (*(pLocate+1)-'0')*10 +(*(pLocate+2)-'0');
				}
				else if(Temp == 2)
				{
					g_SystemParam.TcpIpServerAdr[Index] = (*pLocate-'0')*10 + (*(pLocate+1)-'0');
				}
				else if(Temp == 1)
				{
					g_SystemParam.TcpIpServerAdr[Index] = (*pLocate-'0');
				}
				pLocate += (Temp+1);
			}
		}

		pLocate = strstr(pBuf, "\"sa_cmsport\":");
		if(pLocate != NULL)
		{
			pLocate += strlen("\"sa_cmsport\":\"");
			Temp = WifiGetDataFieldLen(pLocate, '"');
			if(Temp >= 5)
			{
				g_SystemParam.TcpIpServerPort = (*pLocate-'0')*10000 + (*(pLocate+1)-'0')*1000 + (*(pLocate+2)-'0')*100 + (*(pLocate+3)-'0')*10 + (*(pLocate+4)-'0')*10;
			}
			else if(Temp == 4)
			{
				g_SystemParam.TcpIpServerPort = (*pLocate-'0')*1000 + (*(pLocate+1)-'0')*100 + (*(pLocate+2)-'0')*10 + (*(pLocate+3)-'0');
			}
			else if(Temp == 3)
			{
				g_SystemParam.TcpIpServerPort = (*pLocate-'0')*100 + (*(pLocate+1)-'0')*10 + (*(pLocate+2)-'0');
			}
			else if(Temp == 2)
			{
				g_SystemParam.TcpIpServerPort = (*pLocate-'0')*10 + (*(pLocate+1)-'0');
			}
			else if(Temp == 1)
			{
				g_SystemParam.TcpIpServerPort = (*pLocate-'0');
			}	
		}
#if	0	
		pLocate = strstr(pBuf, "\"sa_apn\":");
		if(pLocate != NULL)
		{
			pLocate += strlen("\"sa_apn\":\"");
			Temp = WifiGetDataFieldLen(pLocate, '"');
			if(Temp > GPRS_APN_LENGTH)
				Temp = GPRS_APN_LENGTH;
			memcpy(g_SystemParam.GprsApn, pLocate, Temp);
		}

		pLocate = strstr(pBuf, "\"sa_apnuser\":");
		if(pLocate != NULL)
		{
			pLocate += strlen("\"sa_apnuser\":\"");
			Temp = WifiGetDataFieldLen(pLocate, '"');
			if(Temp > GPRS_APN_NAME_LENGTH)
				Temp = GPRS_APN_NAME_LENGTH;
			memcpy(g_SystemParam.GprsApnName, pLocate, Temp);
		}

		pLocate = strstr(pBuf, "\"sa_apnpwd\":");
		if(pLocate != NULL)
		{
			pLocate += strlen("\"sa_apnpwd\":\"");
			Temp = WifiGetDataFieldLen(pLocate, '"');
			if(Temp > GPRS_APN_PASSWORD_LENGTH)
				Temp = GPRS_APN_PASSWORD_LENGTH;
			memcpy(g_SystemParam.GprsApnPassword, pLocate, Temp);
		}
#endif
		
		SysParamToEeprom();
		HostMsgPost(SYS_MSG_WIFI_SET_PLATFORM_SERVER_ADR, NULL);
	}
	else
	{
		WifiPrintf(",\"sa_selfcheck\":\"%di\",\"sa_hostid\":\"%x%x%x\",\"sa_cmsip\":\"%d.%d.%d.%d\",\"sa_cmsport\":\"%di\",\"sa_apn\":\"\",\"sa_apnuser\":\"\",\"sa_apnpwd\":\"\"",	\
			g_SystemParam.AutoUploadTime,	\
			g_SystemParam.HostID[0],	\
			g_SystemParam.HostID[1],	\
			g_SystemParam.HostID[2],	\
			g_SystemParam.TcpIpServerAdr[0],	\
			g_SystemParam.TcpIpServerAdr[1],	\
			g_SystemParam.TcpIpServerAdr[2],	\
			g_SystemParam.TcpIpServerAdr[3],	\
			g_SystemParam.TcpIpServerPort);
	}
	
	WifiPrintf("}\r\n");
	
}

void AppInquerySetPhoneNumber(unsigned char *pBuf, unsigned char ComMode)
{
	unsigned char *pLocate;

	if(ComMode == TCP_MODE)
	{
		WifiPrintf("TcpSend:");
	}
	else
	{
		WifiPrintf("UdpSend:");
	}
	AppHostFixRespond(pBuf, RESULT_SUCCESS);
	
	pLocate = strstr(pBuf, "sa_tele");
	if(pLocate != NULL)
	{
		;
	}
	else
	{
		WifiPrintf(",\"sa_tele\":[{\"tel\":\"\"},{\"tel\":\"\"},{\"tel\":\"\"},{\"tel\":\"\"},{\"tel\":\"\"}],\"sa_sms\":[{\"tel\":\"\"},{\"tel\":\"\"}]");						
	}
	
	WifiPrintf("}\r\n");
	
}


void AppInquerySetPassword(unsigned char *pBuf, unsigned char ComMode)
{
	unsigned char *pLocate;

	if(ComMode == TCP_MODE)
	{
		WifiPrintf("TcpSend:");
	}
	else
	{
		WifiPrintf("UdpSend:");
	}
	
	pLocate = strstr(pBuf, "\"old\":\"");
	if(pLocate != NULL)
	{
		pLocate += strlen("\"old\":\"");
		if(memcmp(g_SystemParam.SystemPassword, pLocate, PASSWORD_LENGTH) ==0)	
		{
			AppHostFixRespond(pBuf, RESULT_SUCCESS);
			pLocate = strstr(pBuf, "\"password\":\"");
			if(pLocate != NULL)
			{
				pLocate += strlen("\"password\":\"");
				memcpy(g_SystemParam.SystemPassword, pLocate, PASSWORD_LENGTH);
				SysParamToEeprom();
			}
		}
		else
		{
			AppHostFixRespond(pBuf, RESULT_PASSWORD_ERROR);
		}	
	}
	else
	{
		AppHostFixRespond(pBuf, RESULT_SUCCESS);
		WifiPrintf(",\"password\":\"%c%c%c%c\"", g_SystemParam.SystemPassword[0], g_SystemParam.SystemPassword[1], g_SystemParam.SystemPassword[2], g_SystemParam.SystemPassword[3]);	
	}
	
	WifiPrintf("}\r\n");
	
}

void AppInquerySetSysStatus(unsigned char *pBuf, unsigned char ComMode)
{
	unsigned char *pLocate;

	if(ComMode == TCP_MODE)
	{
		WifiPrintf("TcpSend:");
	}
	else
	{
		WifiPrintf("UdpSend:");
	}
	AppHostFixRespond(pBuf, RESULT_SUCCESS);
	pLocate = strstr(pBuf, "\"sa_ctrl\":\"");
	if(pLocate != NULL)
	{
		pLocate += strlen("\"sa_ctrl\":\"");
 		switch(*pLocate - '0')
		{
			case SYSTEM_DISARMED_STATUS:
				CloseSiren();
				BeepTone(BEEP_CONFIRM);
				HostMsgPost(SYS_MSG_LEFT_CURRENT_STATUS, SYSTEM_DISARMED_STATUS);
				break;

			case SYSTEM_ARMED_STATUS:
				if((g_SystemParam.DelayArmTime>0) 
					&&(g_SystemParam.SystemStatus==SYSTEM_DISARMED_STATUS))
				{
					SysPrintf("\r\nSYSTEM_ARMED_STATUS1.");
					//if(g_SystemVar.DelayArmStatus == FALSE)
					{
						g_SystemVar.DelayArmStatus = TRUE;
						g_SystemVar.DelayArmCount = g_SystemParam.DelayArmTime;
						g_SystemVar.DelayArmType = SYSTEM_ARMED_STATUS;
					}
				}
				else
				{
					SysPrintf("\r\nSYSTEM_ARMED_STATUS2%d %d.",g_SystemParam.SystemStatus,g_SystemParam.DelayArmTime);
					HostMsgPost(SYS_MSG_LEFT_CURRENT_STATUS, SYSTEM_ARMED_STATUS);
					BeepTone(BEEP_NORMAL);
				}	
				break;

			case SYSTEM_HOME_ARMED_STATUS:
				if((g_SystemParam.DelayArmTime>0) 
					&&(g_SystemParam.SystemStatus==SYSTEM_DISARMED_STATUS))
				{
					//if(g_SystemVar.DelayArmStatus == FALSE)
					{
						g_SystemVar.DelayArmStatus = TRUE;
						g_SystemVar.DelayArmCount = g_SystemParam.DelayArmTime;	
						g_SystemVar.DelayArmType = SYSTEM_HOME_ARMED_STATUS;
					}
				}
				else
				{
					BeepTone(BEEP_NORMAL);
					HostMsgPost(SYS_MSG_LEFT_CURRENT_STATUS, SYSTEM_HOME_ARMED_STATUS);	
				}
				break;			
		}
	}
	else
	{
		WifiPrintf(",\"sa_ctrl\":\"%d\"", g_SystemParam.SystemStatus);	
	}
	
	WifiPrintf("}\r\n");
	
}

void AppInquerySetRfAccessory(unsigned char *pBuf, unsigned char ComMode)
{
	unsigned char index;
	unsigned char temp;
	unsigned char *pComBuf;
	unsigned char Locate;
	unsigned int *pStoreAdr;
	
	if(ComMode == TCP_MODE)
	{
		WifiPrintf("TcpSend:");
	}
	else
	{
		WifiPrintf("UdpSend:");
	}
	

	if(g_SystemParam.RfAdrMapTotal > 0)
	{
		AppHostFixRespond(pBuf, RESULT_SUCCESS);
		pComBuf = strstr(pBuf, "\"page\":\"");
		if(pComBuf != NULL)
		{
			pComBuf += strlen("\"page\":\"");
			temp = *pComBuf-'0';
			if((temp>0) && (temp<9))
			{
				Locate = (temp-1)*PAGE_RF_UNIT_NUM;
				if(Locate <= g_SystemParam.RfAdrMapTotal)
				{
					WifiPrintf(",\"sa_parts\":{ \"page\":\"%c\",\"count\":\"%d\",\"data\":[", *pComBuf, g_SystemParam.RfAdrMapTotal);
					EE_Read_Data(EE_RF_ADDRESS_MAP_START, g_HostCommonBuf, g_SystemParam.RfAdrMapTotal<<1);
					index = 0;
					pStoreAdr = (unsigned int *)g_HostCommonBuf;
					pStoreAdr += Locate;
					while(1)
					{
						//SysPrintf("\r\nRF Store adr:%xi", *pStoreAdr);
						EE_Read_Data(*pStoreAdr++, (unsigned char *)&g_RFStore, sizeof(RF_Store_t));
						WifiPrintf("{\"pid\":\"%x%x%x\",\"type\":\"%d\",\"mode\":\"%d\",\"ctrl\":\"%d\""	\
							,g_RFStore.RFCode[0]		\
							,g_RFStore.RFCode[1]		\
							,g_RFStore.RFCode[2]		\
							,g_RFStore.Type		\
							,g_RFStore.Mode		\
							,g_RFStore.Control);
						WifiPrintf(",\"name\":\"");
						if(g_RFStore.NameFlag == TRUE)
						{
							memset(pComBuf, END_FLAG, (RF_NAME_LENGHT+2));
							if(g_RFStore.Type == N_RT)
							{
								EE_Read_Data(EE_REMOTE_CONTROL_NAME_ADDRESS+(g_RFStore.CodeLocate-1)*RF_NAME_LENGHT, pComBuf, RF_NAME_LENGHT);
							}
							else if(g_RFStore.Type == N_DOOR_BELL)
							{
								EE_Read_Data(EE_WIRELESS_DOOR_BELL_NAME_ADDRESS+(g_RFStore.CodeLocate-1)*RF_NAME_LENGHT, pComBuf, RF_NAME_LENGHT);
							}
							else
							{
								EE_Read_Data(EE_ZONE_NAME_FIRST_START_ADDRESS+(g_RFStore.CodeLocate-1)*RF_NAME_LENGHT, pComBuf, RF_NAME_LENGHT);
							}
								
							WifiPrintf("%u", pComBuf);
						}
						index++;
						Locate++;
						if((Locate==g_SystemParam.RfAdrMapTotal)||(index==PAGE_RF_UNIT_NUM))
						{
							WifiPrintf("\"}");
							break;
						}
						WifiPrintf("\"},");
					}
					WifiPrintf("]}");
				}	
				
			}	
		}
			
	}
	else
	{
		AppHostFixRespond(pBuf, RESULT_PART_NULL);
	}
		
	WifiPrintf("}\r\n");
}

void AppSetRemoteContrl(unsigned char *pBuf, unsigned char ComMode)
{
	unsigned char *pLocate;
	unsigned char Index;
	unsigned char Len;
	unsigned char Temp;
	
	if(ComMode == TCP_MODE)
	{
		WifiPrintf("TcpSend:");
	}
	else
	{
		WifiPrintf("UdpSend:");
	}
	
	pLocate = strstr(pBuf, "\"remoteid\":\"");
	if(pLocate != NULL)
	{
		pLocate += strlen("\"remoteid\":\"");
		g_HostCommonBuf[0] = AscStrToByte((unsigned char *)pLocate);
		pLocate += 2;
		g_HostCommonBuf[1] = AscStrToByte((unsigned char *)pLocate);
		pLocate += 2;
		g_HostCommonBuf[2] = AscStrToByte((unsigned char *)pLocate);
		Index = RfSearchMatchCode(g_HostCommonBuf, RF_RC);
		if(Index > 0)
		{
			pLocate = strstr(pBuf, "\"remotemode\":\"");
			if(pLocate != NULL)
			{
				pLocate += strlen("\"remotemode\":\"");
				g_RFStore.Mode = *pLocate - '0';
			}
			
			pLocate = strstr(pBuf, "\"remotetype\":\"");
			if(pLocate != NULL)
			{
				pLocate += strlen("\"remotetype\":\"");
				g_RFStore.Type = *pLocate - '0';
					
			}
			g_RFStore.Type = N_RT;
			pLocate = strstr(pBuf, "\"remotectrl\":\"");
			if(pLocate != NULL)
			{
				pLocate += strlen("\"remotectrl\":\"");
				g_RFStore.Control = *pLocate - '0';		
			}
			
			if(Index > 0)
			{
				pLocate = strstr(pBuf, "\"remotename\":\"");
				if(pLocate != NULL)
				{					
					pLocate += strlen("\"remotename\":\"");
					Len = WifiGetUnicodeFieldLen(pLocate, '"');
					if(Len > 0)
					{
						memset(g_HostCommonBuf, END_FLAG, HOST_COMMON_BUF_SIZE);
						if(Len > RF_NAME_LENGHT*2)
						{
							Len = RF_NAME_LENGHT*2;
						}
						
						for(Temp=0; Temp<(Len>>1); Temp++)
						{
							g_HostCommonBuf[Temp] = AscStrToByte(pLocate);
							pLocate += 2;
						}	
						g_RFStore.NameFlag = TRUE;
						EE_Write_Sector(EE_REMOTE_CONTROL_NAME_ADDRESS + (g_RFStore.CodeLocate-1)*RF_NAME_LENGHT, RF_NAME_LENGHT, g_HostCommonBuf);
					}
					else
					{
						g_RFStore.NameFlag = FALSE;
					}						
				}
				EE_Write_Sector(EE_REMOTE_CONTROL_START_ADDRESS + (g_RFStore.CodeLocate-1)*sizeof(RF_Store_t), sizeof(RF_Store_t), (unsigned char *)&g_RFStore);
			}
			AppHostFixRespond(pBuf, RESULT_SUCCESS);
		}
		else
		{
			AppHostFixRespond(pBuf, RESULT_ID_ERROR);
		}
	}
	else
	{
		AppHostFixRespond(pBuf, RESULT_FORMAT_ERROR);
	}
	WifiPrintf("}\r\n");
	
}

void AppRegisterRemoteContrl(unsigned char *pBuf, unsigned char ComMode)
{
	unsigned char *pLocate;
	unsigned char Index;
	unsigned char Temp;
	unsigned char Len;
	
	if(ComMode == TCP_MODE)
	{
		WifiPrintf("TcpSend:");
	}
	else
	{
		WifiPrintf("UdpSend:");
	}
	
	if(g_SystemParam.RegisteredRemoteNum < MAX_STORE_REMOTE_NUM)
	{
		pLocate = strstr(pBuf, "\"remoteid\":\"");
		if(pLocate != NULL)
		{
			pLocate += strlen("\"remoteid\":\"");
			g_HostCommonBuf[0] = AscStrToByte((unsigned char *)pLocate);
			pLocate += 2;
			g_HostCommonBuf[1] = AscStrToByte((unsigned char *)pLocate);
			pLocate += 2;
			g_HostCommonBuf[2] = AscStrToByte((unsigned char *)pLocate);
			Index = RfSearchMatchCode(g_HostCommonBuf, RF_RC);
			if(Index == RF_MATCH_FAIL)
			{
				memcpy(g_RFStore.RFCode, g_HostCommonBuf, RF_CODE_LENGTH);
				Temp = GetRFtype(g_RFStore.RFCode[2]&0x0f);
				if(Temp == RF_RC)
				{
					g_RFStore.CodeLocate = RfSearchValidStoreLocate(RF_RC);
					pLocate = strstr(pBuf, "\"remotemode\":\"");
					if(pLocate != NULL)
					{
						pLocate += strlen("\"remotemode\":\"");
						g_RFStore.Mode = *pLocate - '0';
							
					}
					
					pLocate = strstr(pBuf, "\"remotetype\":\"");
					if(pLocate != NULL)
					{
						pLocate += strlen("\"remotetype\":\"");
						g_RFStore.Type = *pLocate - '0';
							
					}
					g_RFStore.Type = N_RT;
					pLocate = strstr(pBuf, "\"remotectrl\":\"");
					if(pLocate != NULL)
					{
						pLocate += strlen("\"remotectrl\":\"");
						g_RFStore.Control = *pLocate - '0';		
					}
					
					pLocate = strstr(pBuf, "\"remotename\":\"");
					if(pLocate != NULL)
					{						
						pLocate += strlen("\"remotename\":\"");
						Len = WifiGetUnicodeFieldLen(pLocate, '"');
						if(Len > 0)
						{
							memset(g_HostCommonBuf, END_FLAG, HOST_COMMON_BUF_SIZE);
							if(Len > RF_NAME_LENGHT*2)
							{
								Len = RF_NAME_LENGHT*2;
							}
							
							for(Temp=0; Temp<(Len>>1); Temp++)
							{
								g_HostCommonBuf[Temp] = AscStrToByte(pLocate);
								pLocate += 2;
							}	
							g_RFStore.NameFlag = TRUE;
							EE_Write_Sector(EE_REMOTE_CONTROL_NAME_ADDRESS+ (g_RFStore.CodeLocate-1)*RF_NAME_LENGHT, RF_NAME_LENGHT, g_HostCommonBuf);
						}
						else
						{
							g_RFStore.NameFlag = FALSE;
						}						
					}
					EE_Write_Sector(EE_REMOTE_CONTROL_START_ADDRESS + (g_RFStore.CodeLocate-1)*sizeof(RF_Store_t), sizeof(RF_Store_t), (u8 *)&g_RFStore);
					AddRfAdrItem(EE_REMOTE_CONTROL_START_ADDRESS + (g_RFStore.CodeLocate-1)*sizeof(RF_Store_t));
					g_SystemParam.RegisteredRemoteNum++;
					SysParamToEeprom();
					AppHostFixRespond(pBuf, RESULT_SUCCESS);
				}
				else
				{
					AppHostFixRespond(pBuf, RESULT_ID_ERROR);
				}
			}
			else
			{
				AppHostFixRespond(pBuf, RESULT_ID_ERROR);
			}
		}
	}
	else
	{
		AppHostFixRespond(pBuf, RESULT_RC_FULL);
	}
	
	WifiPrintf("}\r\n");
	
}

void AppUnregisterRemoteContrl(unsigned char *pBuf, unsigned char ComMode)
{
	unsigned char *pLocate;
	unsigned char Index;
	
	if(ComMode == TCP_MODE)
	{
		WifiPrintf("TcpSend:");
	}
	else
	{
		WifiPrintf("UdpSend:");
	}

	if(g_SystemParam.RegisteredRemoteNum > 0)
	{
		pLocate = strstr(pBuf, "\"remoteid\":\"");
		if(pLocate != NULL)
		{
			pLocate += strlen("\"remoteid\":\"");
			g_HostCommonBuf[0] = AscStrToByte((unsigned char *)pLocate);
			pLocate += 2;
			g_HostCommonBuf[1] = AscStrToByte((unsigned char *)pLocate);
			pLocate += 2;
			g_HostCommonBuf[2] = AscStrToByte((unsigned char *)pLocate);
			Index = RfSearchMatchCode(g_HostCommonBuf, RF_RC);
			if(Index > 0)
			{
				DeleteRfAdrItem(EE_REMOTE_CONTROL_START_ADDRESS + (g_RFStore.CodeLocate-1)*sizeof(RF_Store_t));
				memset(g_HostCommonBuf, END_FLAG, HOST_COMMON_BUF_SIZE);
				EE_Write_Sector(EE_REMOTE_CONTROL_START_ADDRESS+(g_RFStore.CodeLocate-1)*sizeof(RF_Store_t), sizeof(RF_Store_t)*(g_SystemParam.RegisteredRemoteNum-Index), g_HostCommonBuf);
				DelayNop(50);
				//EE_Write_Sector(EE_REMOTE_CONTROL_NAME_ADDRESS+(g_RFStore.CodeLocate-1)*RF_NAME_LENGHT, RF_NAME_LENGHT*(g_SystemParam.RegisteredRemoteNum-Index), g_HostCommonBuf);
				g_SystemParam.RegisteredRemoteNum--;
				SysParamToEeprom();
				AppHostFixRespond(pBuf, RESULT_SUCCESS);
			}
			else
			{
				AppHostFixRespond(pBuf, RESULT_ID_ERROR);
			}
		}
		else
		{
			AppHostFixRespond(pBuf, RESULT_FORMAT_ERROR);
		}
	}
	else
	{
		AppHostFixRespond(pBuf, RESULT_RC_NULL);
	}
	
	WifiPrintf("}\r\n");
	
}


void AppSetZone(unsigned char *pBuf, unsigned char ComMode)
{
	unsigned char *pLocate;
	unsigned char Index;
	unsigned char Len;
	unsigned char Temp;
	
	if(ComMode == TCP_MODE)
	{
		WifiPrintf("TcpSend:");
	}
	else
	{
		WifiPrintf("UdpSend:");
	}
	pLocate = strstr(pBuf, "\"zoneid\":\"");
	if(pLocate != NULL)
	{
		pLocate += strlen("\"zoneid\":\"");
		g_HostCommonBuf[0] = AscStrToByte((unsigned char *)pLocate);
		pLocate += 2;
		g_HostCommonBuf[1] = AscStrToByte((unsigned char *)pLocate);
		pLocate += 2;
		g_HostCommonBuf[2] = AscStrToByte((unsigned char *)pLocate);
		Index = RfSearchMatchCode(g_HostCommonBuf, RF_ZONE);
		if(Index > 0)
		{
			memcpy(g_RFStore.RFCode, g_HostCommonBuf, RF_CODE_LENGTH);
			pLocate = strstr(pBuf, "\"zonemode\":\"");
			if(pLocate != NULL)
			{
				pLocate += strlen("\"zonemode\":\"");
				g_RFStore.Mode = *pLocate - '0';
			}
			
			pLocate = strstr(pBuf, "\"zonetype\":\"");
			if(pLocate != NULL)
			{
				pLocate += strlen("\"zonetype\":\"");
				if(*(pLocate+1) == '"')	
				{
					g_RFStore.Type = *pLocate - '0';
				}
				else
				{
					g_RFStore.Type = (*pLocate - '0')*10;
					g_RFStore.Type +=(*(pLocate+1) - '0');
				}
					
			}
			
			pLocate = strstr(pBuf, "\"zonectrl\":\"");
			if(pLocate != NULL)
			{
				pLocate += strlen("\"zonectrl\":\"");
				g_RFStore.Control = *pLocate - '0';		
			}
			
			if(Index > 0)
			{
				pLocate = strstr(pBuf, "\"zonename\":\"");
				if(pLocate != NULL)
				{					
					pLocate += strlen("\"zonename\":\"");
					Len = WifiGetUnicodeFieldLen(pLocate, '"');
					if(Len > 0)
					{
						memset(g_HostCommonBuf, END_FLAG, HOST_COMMON_BUF_SIZE);
						if(Len > RF_NAME_LENGHT*2)
						{
							Len = RF_NAME_LENGHT*2;
						}
						
						for(Temp=0; Temp<(Len>>1); Temp++)
						{
							g_HostCommonBuf[Temp] = AscStrToByte(pLocate);
							pLocate += 2;
						}
						g_RFStore.NameFlag = TRUE;
						EE_Write_Sector(EE_ZONE_NAME_FIRST_START_ADDRESS+(g_RFStore.CodeLocate-1)*RF_NAME_LENGHT, RF_NAME_LENGHT, g_HostCommonBuf);
					}
					else
					{
						g_RFStore.NameFlag = FALSE;
					}
				}
				EE_Write_Sector(EE_WIRELESS_ACCESSORY_START_ADDRESS + (g_RFStore.CodeLocate-1)*sizeof(RF_Store_t), sizeof(RF_Store_t), (u8 *)&g_RFStore);
			}
			AppHostFixRespond(pBuf, RESULT_SUCCESS);
		}
		else
		{
			AppHostFixRespond(pBuf, RESULT_ID_ERROR);
		}
	}
	else
	{
		AppHostFixRespond(pBuf, RESULT_FORMAT_ERROR);
	}
	WifiPrintf("}\r\n");
	
}

void AppRegisterZone(unsigned char *pBuf, unsigned char ComMode)
{
	unsigned char *pLocate;
	unsigned char Index;
	unsigned char Temp;
	unsigned char Len;

	if(ComMode == TCP_MODE)
	{
		WifiPrintf("TcpSend:");
	}
	else
	{
		WifiPrintf("UdpSend:");
	}
	
	if(g_SystemParam.RegisteredWirelessAccessoryNum < MAX_STORE_WIRELESS_ACCESSORY_NUM)
	{
		pLocate = strstr(pBuf, "\"zoneid\":\"");
		if(pLocate != NULL)
		{
			pLocate += strlen("\"zoneid\":\"");
			g_HostCommonBuf[0] = AscStrToByte((unsigned char *)pLocate);
			pLocate += 2;
			g_HostCommonBuf[1] = AscStrToByte((unsigned char *)pLocate);
			pLocate += 2;
			g_HostCommonBuf[2] = AscStrToByte((unsigned char *)pLocate);
			Index = RfSearchMatchCode(g_HostCommonBuf, RF_ZONE);
			if(Index == RF_MATCH_FAIL)
			{
				memcpy(g_RFStore.RFCode, g_HostCommonBuf, RF_CODE_LENGTH);
				Temp = GetRFtype(g_RFStore.RFCode[2]&0x0f);
				if(Temp == RF_ZONE)
				{
					g_RFStore.CodeLocate = RfSearchValidStoreLocate(RF_ZONE);
					pLocate = strstr(pBuf, "\"zonemode\":\"");
					if(pLocate != NULL)
					{
						pLocate += strlen("\"zonemode\":\"");
						g_RFStore.Mode = *pLocate - '0';
							
					}
					
					pLocate = strstr(pBuf, "\"zonetype\":\"");
					if(pLocate != NULL)
					{
						pLocate += strlen("\"zonetype\":\"");
						if(*(pLocate+1) == '"')	
						{
							g_RFStore.Type = *pLocate - '0';
						}
						else
						{
							g_RFStore.Type = (*pLocate - '0')*10;
							g_RFStore.Type +=(*(pLocate+1) - '0');
						}
							
					}
					
					pLocate = strstr(pBuf, "\"zonectrl\":\"");
					if(pLocate != NULL)
					{
						pLocate += strlen("\"zonectrl\":\"");
						g_RFStore.Control = *pLocate - '0';		
					}
					
					pLocate = strstr(pBuf, "\"zonename\":\"");
					if(pLocate != NULL)
					{						
						pLocate += strlen("\"zonename\":\"");
						Len = WifiGetUnicodeFieldLen(pLocate, '"');
						if(Len > 0)
						{
							memset(g_HostCommonBuf, END_FLAG, HOST_COMMON_BUF_SIZE);
							if(Len> RF_NAME_LENGHT*2)
							{
								Len = RF_NAME_LENGHT*2;
							}
							
							for(Temp=0; Temp<(Len>>1); Temp++)
							{
								g_HostCommonBuf[Temp] = AscStrToByte(pLocate);
								pLocate += 2;
							}	
							g_RFStore.NameFlag = TRUE;
							EE_Write_Sector(EE_ZONE_NAME_FIRST_START_ADDRESS+(g_RFStore.CodeLocate-1)*RF_NAME_LENGHT, RF_NAME_LENGHT, g_HostCommonBuf);
						}
						else
						{
							g_RFStore.NameFlag = FALSE;
						}
						
					}
					EE_Write_Sector(EE_WIRELESS_ACCESSORY_START_ADDRESS + (g_RFStore.CodeLocate-1)*sizeof(RF_Store_t), sizeof(RF_Store_t), (u8 *)&g_RFStore);
					AddRfAdrItem(EE_WIRELESS_ACCESSORY_START_ADDRESS + (g_RFStore.CodeLocate-1)*sizeof(RF_Store_t));
					g_SystemParam.RegisteredWirelessAccessoryNum++;
					SysParamToEeprom();
					AppHostFixRespond(pBuf, RESULT_SUCCESS);
				}
				else
				{
					AppHostFixRespond(pBuf, RESULT_ID_ERROR);
				}
			}
			else
			{
				AppHostFixRespond(pBuf, RESULT_ID_ERROR);
			}
		}
	}
	else
	{
		AppHostFixRespond(pBuf, RESULT_ZONE_FULL);
	}
	
	WifiPrintf("}\r\n");
	
}

void AppUnregisterZone(unsigned char *pBuf, unsigned char ComMode)
{
	unsigned char *pLocate;
	unsigned char Index;
	unsigned int Adr;
	if(ComMode == TCP_MODE)
	{
		WifiPrintf("TcpSend:");
	}
	else
	{
		WifiPrintf("UdpSend:");
	}

	if(g_SystemParam.RegisteredWirelessAccessoryNum > 0)
	{
		pLocate = strstr(pBuf, "\"zoneid\":\"");
		if(pLocate != NULL)
		{
			pLocate += strlen("\"zoneid\":\"");
			g_HostCommonBuf[0] = AscStrToByte((unsigned char *)pLocate);
			pLocate += 2;
			g_HostCommonBuf[1] = AscStrToByte((unsigned char *)pLocate);
			pLocate += 2;
			g_HostCommonBuf[2] = AscStrToByte((unsigned char *)pLocate);
			Index = RfSearchMatchCode(g_HostCommonBuf, RF_ZONE);
			if(Index > 0)
			{
				DeleteRfAdrItem(EE_WIRELESS_ACCESSORY_START_ADDRESS + (g_RFStore.CodeLocate-1)*sizeof(RF_Store_t));
				memset(g_HostCommonBuf, END_FLAG, HOST_COMMON_BUF_SIZE);
				EE_Write_Sector(EE_WIRELESS_ACCESSORY_START_ADDRESS+(g_RFStore.CodeLocate-1)*sizeof(RF_Store_t), sizeof(RF_Store_t), g_HostCommonBuf);
				//EE_Write_Sector(EE_ZONE_NAME_FIRST_START_ADDRESS+(g_RFStore.CodeLocate-1)*RF_NAME_LENGHT,  RF_NAME_LENGHT,  g_HostCommonBuf);
				g_SystemParam.RegisteredWirelessAccessoryNum--;
				SysParamToEeprom();
				AppHostFixRespond(pBuf, RESULT_SUCCESS);
			}
			else
			{
				AppHostFixRespond(pBuf, RESULT_ID_ERROR);
			}
		}
		else
		{
			AppHostFixRespond(pBuf, RESULT_FORMAT_ERROR);
		}
	}
	else
	{
		AppHostFixRespond(pBuf, RESULT_ZONE_NULL);
	}
	
	WifiPrintf("}\r\n");
	
}


void AppSetDoorBell(unsigned char *pBuf, unsigned char ComMode)
{
	unsigned char *pLocate;
	unsigned char Index;
	unsigned char Len;
	unsigned char Temp;
	
	if(ComMode == TCP_MODE)
	{
		WifiPrintf("TcpSend:");
	}
	else
	{
		WifiPrintf("UdpSend:");
	}
	
	pLocate = strstr(pBuf, "\"bellid\":\"");
	if(pLocate != NULL)
	{
		pLocate += strlen("\"bellid\":\"");
		g_HostCommonBuf[0] = AscStrToByte((unsigned char *)pLocate);
		pLocate += 2;
		g_HostCommonBuf[1] = AscStrToByte((unsigned char *)pLocate);
		pLocate += 2;
		g_HostCommonBuf[2] = AscStrToByte((unsigned char *)pLocate);
		Index = RfSearchMatchCode(g_HostCommonBuf, RF_DOOR_BELL);
		if(Index > 0)
		{
			pLocate = strstr(pBuf, "\"bellmode\":\"");
			if(pLocate != NULL)
			{
				pLocate += strlen("\"bellmode\":\"");
				g_RFStore.Mode = *pLocate - '0';
			}
			
			
			g_RFStore.Type = N_DOOR_BELL;
			pLocate = strstr(pBuf, "\"bellctrl\":\"");
			if(pLocate != NULL)
			{
				pLocate += strlen("\"bellctrl\":\"");
				g_RFStore.Control = *pLocate - '0';		
			}
			
			if(Index > 0)
			{
				pLocate = strstr(pBuf, "\"bellname\":\"");
				if(pLocate != NULL)
				{
					pLocate += strlen("\"bellname\":\"");
					Len = WifiGetUnicodeFieldLen(pLocate, '"');
					if(Len > 0)
					{
						memset(g_HostCommonBuf, END_FLAG, HOST_COMMON_BUF_SIZE);
						if(Len >= RF_NAME_LENGHT*2)
						{
							Len = RF_NAME_LENGHT*2;
						}
						
						for(Temp=0; Temp<(Len>>1); Temp++)
						{
							g_HostCommonBuf[Temp] = AscStrToByte(pLocate);
							pLocate += 2;
						}	
						g_RFStore.NameFlag = TRUE;
						EE_Write_Sector(EE_WIRELESS_DOOR_BELL_NAME_ADDRESS + (g_RFStore.CodeLocate-1)*RF_NAME_LENGHT, RF_NAME_LENGHT,  g_HostCommonBuf);
					}
					else
					{
						g_RFStore.NameFlag = FALSE;
					}
					
				}
				EE_Write_Sector(EE_WIRELESS_DOOR_BELL_START_ADDRESS + (g_RFStore.CodeLocate-1)*sizeof(RF_Store_t), sizeof(RF_Store_t), (u8 *)&g_RFStore);
			}
			AppHostFixRespond(pBuf, RESULT_SUCCESS);
		}
		else
		{
			AppHostFixRespond(pBuf, RESULT_ID_ERROR);
		}
	}
	else
	{
		AppHostFixRespond(pBuf, RESULT_FORMAT_ERROR);
	}
	WifiPrintf("}\r\n");
	
}

void AppRegisterDoorBell(unsigned char *pBuf, unsigned char ComMode)
{
	unsigned char *pLocate;
	unsigned char Index;
	unsigned char Temp;
	unsigned char Len;

	if(ComMode == TCP_MODE)
	{
		WifiPrintf("TcpSend:");
	}
	else
	{
		WifiPrintf("UdpSend:");
	}
	
	if(g_SystemParam.RegisteredDoorBellNum < MAX_STORE_WIRELESS_DOOR_BELL_NUM)
	{
		pLocate = strstr(pBuf, "\"bellid\":\"");
		if(pLocate != NULL)
		{
			pLocate += strlen("\"bellid\":\"");
			g_HostCommonBuf[0] = AscStrToByte((unsigned char *)pLocate);
			pLocate += 2;
			g_HostCommonBuf[1] = AscStrToByte((unsigned char *)pLocate);
			pLocate += 2;
			g_HostCommonBuf[2] = AscStrToByte((unsigned char *)pLocate);
			Index = RfSearchMatchCode(g_HostCommonBuf, RF_DOOR_BELL);
			if(Index == RF_MATCH_FAIL)
			{
				g_RFStore.CodeLocate = RfSearchValidStoreLocate(RF_DOOR_BELL);
				memcpy(g_RFStore.RFCode, g_HostCommonBuf, RF_CODE_LENGTH);
				Temp = GetRFtype(g_RFStore.RFCode[2]&0x0f);
				if(Temp == RF_DOOR_BELL)
				{
					pLocate = strstr(pBuf, "\"bellmode\":\"");
					if(pLocate != NULL)
					{
						pLocate += strlen("\"bellmode\":\"");
						g_RFStore.Mode = *pLocate - '0';
							
					}
					
					g_RFStore.Type = N_DOOR_BELL;
							
					pLocate = strstr(pBuf, "\"bellctrl\":\"");
					if(pLocate != NULL)
					{
						pLocate += strlen("\"bellctrl\":\"");
						g_RFStore.Control = *pLocate - '0';		
					}
					
					pLocate = strstr(pBuf, "\"bellname\":\"");
					if(pLocate != NULL)
					{
						pLocate += strlen("\"bellname\":\"");
						Len = WifiGetUnicodeFieldLen(pLocate, '"');
						if(Len > 0)
						{
							memset(g_HostCommonBuf, END_FLAG, HOST_COMMON_BUF_SIZE);
							if(Len >= RF_NAME_LENGHT*2)
							{
								Len = RF_NAME_LENGHT*2;
							}
							
							for(Temp=0; Temp<(Len>>1); Temp++)
							{
								g_HostCommonBuf[Temp] = AscStrToByte(pLocate);
								pLocate += 2;
							}
							g_RFStore.NameFlag = TRUE;
							EE_Write_Sector(EE_WIRELESS_DOOR_BELL_NAME_ADDRESS+(g_RFStore.CodeLocate-1)*RF_NAME_LENGHT, RF_NAME_LENGHT, g_HostCommonBuf);
						}
						else
						{
							g_RFStore.NameFlag = FALSE;
						}
					}
					EE_Write_Sector(EE_WIRELESS_DOOR_BELL_START_ADDRESS + (g_RFStore.CodeLocate-1)*sizeof(RF_Store_t),  sizeof(RF_Store_t),  (u8 *)&g_RFStore);
					AddRfAdrItem(EE_WIRELESS_DOOR_BELL_START_ADDRESS + (g_RFStore.CodeLocate-1)*sizeof(RF_Store_t));
					g_SystemParam.RegisteredDoorBellNum++;
					SysParamToEeprom();
					AppHostFixRespond(pBuf, RESULT_SUCCESS);
				}
				else
				{
					AppHostFixRespond(pBuf, RESULT_ID_ERROR);
				}
			}
			else
			{
				AppHostFixRespond(pBuf, RESULT_ID_ERROR);
			}
		}
	}
	else
	{
		AppHostFixRespond(pBuf, RESULT_DOOR_BELL_FULL);
	}
	
	WifiPrintf("}\r\n");
	
}

void AppUnregisterDoorBell(unsigned char *pBuf, unsigned char ComMode)
{
	unsigned char *pLocate;
	unsigned char Index;
	
	if(ComMode == TCP_MODE)
	{
		WifiPrintf("TcpSend:");
	}
	else
	{
		WifiPrintf("UdpSend:");
	}

	if(g_SystemParam.RegisteredDoorBellNum > 0)
	{
		pLocate = strstr(pBuf, "\"bellid\":\"");
		if(pLocate != NULL)
		{
			pLocate += strlen("\"bellid\":\"");
			g_HostCommonBuf[0] = AscStrToByte((unsigned char *)pLocate);
			pLocate += 2;
			g_HostCommonBuf[1] = AscStrToByte((unsigned char *)pLocate);
			pLocate += 2;
			g_HostCommonBuf[2] = AscStrToByte((unsigned char *)pLocate);
			Index = RfSearchMatchCode(g_HostCommonBuf, RF_DOOR_BELL);
			if(Index > 0)
			{
				DeleteRfAdrItem(EE_WIRELESS_DOOR_BELL_START_ADDRESS+ (g_RFStore.CodeLocate-1)*sizeof(RF_Store_t));
				memset(g_HostCommonBuf, END_FLAG, HOST_COMMON_BUF_SIZE);
				EE_Write_Sector(EE_WIRELESS_DOOR_BELL_START_ADDRESS+(g_RFStore.CodeLocate-1)*sizeof(RF_Store_t), sizeof(RF_Store_t), g_HostCommonBuf);
				//EE_Write_Sector(EE_WIRELESS_DOOR_BELL_NAME_ADDRESS+(g_RFStore.CodeLocate-1)*RF_NAME_LENGHT, RF_NAME_LENGHT, g_HostCommonBuf);
				g_SystemParam.RegisteredDoorBellNum--;
				SysParamToEeprom();
				AppHostFixRespond(pBuf, RESULT_SUCCESS);
			}
			else
			{
				AppHostFixRespond(pBuf, RESULT_ID_ERROR);
			}
		}
		else
		{
			AppHostFixRespond(pBuf, RESULT_FORMAT_ERROR);
		}
	}
	else
	{
		AppHostFixRespond(pBuf, RESULT_DOOR_BELL_NULL);
	}
	
	WifiPrintf("}\r\n");
	
}

void AppEntryRegisterStatus(unsigned char *pBuf, unsigned char ComMode)
{

	if(g_SystemParam.SystemStatus != SYSTEM_RF_REGISTER_STATUS)
	{
		BeepTone(BEEP_NORMAL);
		HostMsgPost(SYS_MSG_LEFT_CURRENT_STATUS, SYSTEM_RF_REGISTER_STATUS);
	}
	
	if(ComMode == TCP_MODE)
	{
		WifiPrintf("TcpSend:");
	}
	else
	{
		WifiPrintf("UdpSend:");
	}

	AppHostFixRespond(pBuf, RESULT_SUCCESS);
	
	WifiPrintf("}\r\n");
	
}
///////////////////////////////////////////////////////////
void UpdateTimeFromPlatform(unsigned char *pBuf)
{
	unsigned char Year,Month,Day,Hour,Minute,Second;
	
	if(*pBuf++ != ',')
		return;
	Year = (*pBuf-'0')*10 + (*(pBuf+1)-'0');
	pBuf += 2;
	Month = (*pBuf-'0')*10 + (*(pBuf+1)-'0');
	pBuf += 2;
	Day = (*pBuf-'0')*10 + (*(pBuf+1)-'0');
	pBuf += 2;
	Hour = (*pBuf-'0')*10 + (*(pBuf+1)-'0');
	pBuf += 2;
	Minute = (*pBuf-'0')*10 + (*(pBuf+1)-'0');
	pBuf += 2;
	Second = (*pBuf-'0')*10 + (*(pBuf+1)-'0');

//	if((Hour>23)||(Minute>59)||(Second>59)||(Month>12)||(Day>31))
//		return;
	
	g_RTC.Year= Year;
	g_RTC.Month = Month;
	g_RTC.Day = Day;
	g_RTC.Hour= Hour;
	g_RTC.Minute = Minute;
	g_RTC.Second = Second;
	
}

void WifiAcceptOkHadle(unsigned char *pBuf)
{
	unsigned char SetVal;

	SetVal = *pBuf - '0';
	switch(SetVal)
	{
		case PLATFORM_SET_DISARM:
			//oseSiren();
			BeepTone(BEEP_CONFIRM);
			HostMsgPost(SYS_MSG_LEFT_CURRENT_STATUS, SYSTEM_DISARMED_STATUS);
			break;

		case PLATFORM_SET_ARM:
		#if	1
			if((g_SystemParam.DelayArmTime>0) 
				&& (g_SystemParam.SystemStatus==SYSTEM_DISARMED_STATUS))
			{
				//if(g_SystemVar.DelayArmStatus == FALSE)
				{
					g_SystemVar.DelayArmStatus = TRUE;
					g_SystemVar.DelayArmCount = g_SystemParam.DelayArmTime;	
					g_SystemVar.DelayArmType = SYSTEM_ARMED_STATUS;
				}
			}
			else
		#endif		
			{
				BeepTone(BEEP_NORMAL);
				HostMsgPost(SYS_MSG_LEFT_CURRENT_STATUS, SYSTEM_ARMED_STATUS);	
			}
			break;

		case PLATFORM_SET_HOME_ARM:
		#if	1
			if((g_SystemParam.DelayArmTime>0) 
				&& (g_SystemParam.SystemStatus==SYSTEM_DISARMED_STATUS))
			{
				//if(g_SystemVar.DelayArmStatus == FALSE)
				{
					g_SystemVar.DelayArmStatus = TRUE;
					g_SystemVar.DelayArmCount = g_SystemParam.DelayArmTime;	
					g_SystemVar.DelayArmType = SYSTEM_HOME_ARMED_STATUS;
				}
			}
			else
		#endif		
			{
				BeepTone(BEEP_NORMAL);
				HostMsgPost(SYS_MSG_LEFT_CURRENT_STATUS, SYSTEM_HOME_ARMED_STATUS);	
			}
			break;			
	}

	//update RTC
	UpdateTimeFromPlatform(pBuf+1);
}
/////////////////////end///////////////////////////////////
///////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////
void WifiAppDataParseHandle(unsigned char *pBuf, unsigned char ComMode)
{	
	unsigned char *pLocate;

	///////////////////////APP COMMAND////////////////////////////	
	//search host command
	pLocate = strstr(pBuf, APP_COMMAND_SEARCH_HOST);
	if(pLocate != NULL)
	{
		AppSearchHost(pBuf, ComMode);
		return;
	}	

	//login host command
	pLocate = strstr(pBuf, APP_COMMAND_LOGIN_HOST);
	if(pLocate != NULL)
	{
		AppLoginHost(pBuf, ComMode);
		return;
	}	

	//inquery&set system setting command
	pLocate = strstr(pBuf, APP_COMMAND_INQUERY_SET_SYS_SETTING);
	if(pLocate != NULL)
	{
		AppInquerySetSysSetting(pBuf, ComMode);
		return;
	}	

	//inquery&set alarm host command
	pLocate = strstr(pBuf, APP_COMMAND_INQUERY_SET_ALARM_HOST);
	if(pLocate != NULL)
	{
		AppInquerySetAlarmHost(pBuf, ComMode);
		return;
	}	

	//inquery&set telephone number command
	pLocate = strstr(pBuf, APP_COMMAND_INQUERY_SET_TEL_NUMBER);
	if(pLocate != NULL)
	{
		AppInquerySetPhoneNumber(pBuf, ComMode);
		return;
	}	

	//inquery&set password command
	pLocate = strstr(pBuf, APP_COMMAND_INQUERY_SET_PASSWORD);
	if(pLocate != NULL)
	{
		AppInquerySetPassword(pBuf, ComMode);
		return;
	}	

	//inquery&set system status command
	pLocate = strstr(pBuf, APP_COMMAND_INQUERY_SET_SYS_STATUS);
	if(pLocate != NULL)
	{
		AppInquerySetSysStatus(pBuf, ComMode);
		return;
	}	
#if 0
	//inquery history log command
	pLocate = strstr(pBuf, APP_COMMAND_INQUERY_HISTORY_LOG);
	if(pLocate != NULL)
	{
		AppInqueryHistoryLog(pBuf, ComMode);
		return;
	}	
#endif
	//inquery RF accessory command
	pLocate = strstr(pBuf, APP_COMMAND_INQUERY_RF_ACCESSORY);
	if(pLocate != NULL)
	{
		AppInquerySetRfAccessory(pBuf, ComMode);
		return;
	}	

	//set remote contrl command
	pLocate = strstr(pBuf, APP_COMMAND_SET_RF_RC);
	if(pLocate != NULL)
	{
		AppSetRemoteContrl(pBuf, ComMode);
		return;
	}	

	//register remote contrl command
	pLocate = strstr(pBuf, APP_COMMAND_REGISTER_RF_RC);
	if(pLocate != NULL)
	{
		AppRegisterRemoteContrl(pBuf, ComMode);
		return;
	}	

	//unregister remote contrl command
	pLocate = strstr(pBuf, APP_COMMAND_UNREGISTER_RF_RC);
	if(pLocate != NULL)
	{
		AppUnregisterRemoteContrl(pBuf, ComMode);
		return;
	}	

	//set ZONE command
	pLocate = strstr(pBuf, APP_COMMAND_SET_RF_ZONE);
	if(pLocate != NULL)
	{
		AppSetZone(pBuf, ComMode);
		return;
	}	

	//register ZONE command
	pLocate = strstr(pBuf, APP_COMMAND_REGISTER_RF_ZONE);
	if(pLocate != NULL)
	{
		AppRegisterZone(pBuf, ComMode);
		return;
	}	

	//unregister ZONE command
	pLocate = strstr(pBuf, APP_COMMAND_UNREGISTER_RF_ZONE);
	if(pLocate != NULL)
	{
		AppUnregisterZone(pBuf, ComMode);
		return;
	}

	//set door bell command
	pLocate = strstr(pBuf, APP_COMMAND_SET_RF_DOOR_BELL);
	if(pLocate != NULL)
	{
		AppSetDoorBell(pBuf, ComMode);
		return;
	}	

	//register door bell command
	pLocate = strstr(pBuf, APP_COMMAND_REGISTER_RF_DOOR_BELL);
	if(pLocate != NULL)
	{
		AppRegisterDoorBell(pBuf, ComMode);
		return;
	}	

	//unregister door bell command
	pLocate = strstr(pBuf, APP_COMMAND_UNREGISTER_RF_DOOR_BELL);
	if(pLocate != NULL)
	{
		AppUnregisterDoorBell(pBuf, ComMode);
		return;
	}

	//Entry register status command
	pLocate = strstr(pBuf, APP_COMMAND_ENTRY_REGISTER_STATUS);
	if(pLocate != NULL)
	{
		AppEntryRegisterStatus(pBuf, ComMode);
		return;
	}
	
}

void ExitSetConnectApFromModule(void)
{
	g_WifiVar.cSetConnectApFlag = FALSE;
	g_WifiVar.cEntrySetConnectApTimeout = 0;	
	SysPrintf("\r\nExitSetConnectApFromModule");
}

void ExeSetConnectApFromModule(void)
{
	g_WifiVar.cBeepControl = TRUE;
	SysPrintf("\r\nExeSetConnectApFromModule");
}

void RespondEntrySetConnectAp(void)
{
	g_WifiVar.cResetTimeOut = WIFI_RESET_TIMEOUT;
	g_WifiVar.cSetConnectApFlag = TRUE;
	g_WifiVar.cBeepControl = TRUE;
	LedBlinkSet(LED_WIFI_INDICATOR, LED_FAST_BLINK);
	g_WifiVar.cEntrySetConnectApTimeout = WIFI_SET_CONNECT_AP_TIMEOUT;
	SysPrintf("\r\nRespondEntrySetConnectAp");
}

void RespondDeviceInforCheck(unsigned char *pBuf)
{
	unsigned char Index;
	unsigned char MatchFlag;	

	MatchFlag = TRUE;
	for(Index=0; Index<DEVICE_ID_LENGTH; Index++)
	{
		if(AscStrToByte(pBuf)!= g_SystemVar.DeviceID[Index])
		{
			MatchFlag = FALSE;
			break;
		}
		pBuf += 2;
	}
	
	if(MatchFlag == TRUE)
	{
		g_WifiVar.cDeviceInforRespondTimeOut = 0;
		SysPrintf("\r\nRespondDeviceInforMatch.");
	}
}
 
void WifiModuleDataParseHandle(unsigned char *pBuf)
{
	unsigned char *pLocate;
	
	//get WIFI connect status
	pLocate = strstr(pBuf, "WIFI POWER ON\r\n");
	if(pLocate != NULL)
	{
		HostMsgPost(SYS_MSG_WIFI_POWER_ON, NULL);
		return;
	}	
	
	//get RespondDeviceInfor
	pLocate = strstr((char *)pBuf, "RespondDeviceInfor:");
	if(pLocate != NULL)
	{
		pLocate += strlen("RespondDeviceInfor:");
		RespondDeviceInforCheck(pLocate);
		return;
	}	
	
	pLocate = strstr(pBuf, "STAMODE DISCONNECTED\r\n");
	if(pLocate != NULL)
	{
		HostMsgPost(SYS_MSG_WIFI_DISCONNECT, NULL);
		return;
	}	
	
	pLocate = strstr(pBuf, "STAMODE CONNECTED\r\n");
	if(pLocate != NULL)
	{
		HostMsgPost(SYS_MSG_WIFI_CONNECTED, NULL);
		return;
	}

	pLocate = strstr(pBuf, "STAMODE GOT IP\r\n");
	if(pLocate != NULL)
	{
		HostMsgPost(SYS_MSG_WIFI_STATION_GOT_IP, NULL);
		return;
	}

	pLocate = strstr(pBuf, "SOFTAPMODE STACONNECTED\r\n");
	if(pLocate != NULL)
	{
		HostMsgPost(SYS_MSG_WIFI_AP_STATION_CONNECTED, NULL);
		return;
	}
	
	pLocate = strstr(pBuf, "PLATFORM DISCONNECTED\r\n");
	if(pLocate != NULL)
	{
		HostMsgPost(SYS_MSG_WIFI_LOST_CONNECT_PLATFORM, NULL);
		return;
	}	

	pLocate = strstr(pBuf, "PLATFORM CONNECTED\r\n");
	if(pLocate != NULL)
	{
		HostMsgPost(SYS_MSG_WIFI_CONNECT_PLATFORM, NULL);
		return;
	}		
	
	//TCP SEND SUCCESSFULLY
	pLocate = strstr(pBuf, "ConnectConfigOK\r\n");
	if(pLocate != NULL)
	{
		//g_SystemVar.UploadPlatformControl = TRUE;
		g_SystemVar.AutoUploadTime = 4;//g_SystemParam.AutoUploadTime;
		return;
	}
	
	//Check Connect Platform Respond
	pLocate = strstr(pBuf, "Platform:");
	if(pLocate != NULL)
	{
		pLocate += strlen("Platform:");
		WifiCheckConnectPlatformRespond(pLocate);
		return;
	}

	//ExeSetConnectApFromModule
	pLocate = strstr(pBuf, "ExeSetConnectAp");
	if(pLocate != NULL)
	{
		ExeSetConnectApFromModule();
		return;
	}

	//Set Connect Ap Respond
	pLocate = strstr(pBuf, "RespondSetConnectAp");
	if(pLocate != NULL)
	{
		RespondEntrySetConnectAp();
		return;
	}	
	
	//Check Connect Platform Respond
	pLocate = strstr(pBuf, "ExitSetConnectAp");
	if(pLocate != NULL)
	{
		ExitSetConnectApFromModule();
		return;
	}		

	//TCP SEND SUCCESSFULLY
	pLocate = strstr(pBuf, "TcpsendOK\r\n");
	if(pLocate != NULL)
	{
		WifiTcpSendSuccess();
		return;
	}		

}

unsigned char MatchDeviceID(unsigned char *pBuf)
{
	unsigned char Index;
	unsigned char RetVal;
	unsigned char Temp;
	unsigned char *pLocate;

	RetVal = FALSE;
	pLocate = strstr(pBuf, "deviceid\":\"");
	if(pLocate != NULL)
	{
		pLocate += strlen("deviceid\":\"");
		RetVal = TRUE;
		for(Index=0; Index<DEVICE_ID_LENGTH; Index++)
		{
			Temp = AscStrToByte((unsigned char *)pLocate);
			if(g_SystemVar.DeviceID[Index] != Temp)
			{
				RetVal = FALSE;
				break;
			}
			pLocate += 2;
		}
	}

	return RetVal;
}

void WifiReceiveDataHandle(void)
{	
	unsigned char RetVal;
	unsigned char *pLocate;
	
	SysPrintf("\r\nWIFI data: %u", g_sUart[UART_WIFI].cReveiveBuf);
	if(g_WifiVar.cForceReset > 0)
	{
		g_sUart[UART_WIFI].iPushIndex = 0;
		memset(g_sUart[UART_WIFI].cReveiveBuf, END_FLAG, UART_WIFI_BUFFER_SIZE);
		g_sUart[UART_WIFI].bRxControl = ENABLE;
		return;
	}
	pLocate = strstr(g_sUart[UART_WIFI].cReveiveBuf, "TCP Receive:");
	if(pLocate != NULL)
	{
		//search host command
		pLocate = strstr(g_sUart[UART_WIFI].cReveiveBuf, APP_COMMAND_SEARCH_HOST);
		if(pLocate != NULL)
		{
			AppSearchHost((unsigned char *)g_sUart[UART_WIFI].cReveiveBuf, TCP_MODE);
		}
		else
		{
			pLocate = strstr(g_sUart[UART_WIFI].cReveiveBuf, "TCP Receive:ACCEPTOK");
			if(pLocate != NULL)
			{
				g_WifiVar.cTcpSendFailCount = 0;
				g_SystemVar.UploadPlatformControl = TRUE;
				g_WifiVar.cTcpRespond = 0;
				pLocate += strlen("TCP Receive:ACCEPTOK");
				WifiAcceptOkHadle((unsigned char *)pLocate);
				if(g_WifiVar.cStatus != WIFI_CONNECT_PLATFORM)
				{
					HostMsgPost(SYS_MSG_WIFI_CONNECT_PLATFORM, NULL);
				}
			}	
			else
			{
				RetVal = MatchDeviceID(g_sUart[UART_WIFI].cReveiveBuf);
				if(RetVal == TRUE)
				{
					WifiAppDataParseHandle((unsigned char *)g_sUart[UART_WIFI].cReveiveBuf, TCP_MODE);
				}
			}
		}
	}	
	else
	{
		pLocate = strstr(g_sUart[UART_WIFI].cReveiveBuf, "UDP Receive:");
		if(pLocate != NULL)
		{
			pLocate = strstr(g_sUart[UART_WIFI].cReveiveBuf, APP_COMMAND_SEARCH_HOST);
			if(pLocate != NULL)
			{
				AppSearchHost((unsigned char *)g_sUart[UART_WIFI].cReveiveBuf, UDP_MODE);
			}
			else
			{
				RetVal = MatchDeviceID(g_sUart[UART_WIFI].cReveiveBuf);
				if(RetVal == TRUE)
				{
					WifiAppDataParseHandle((unsigned char *)g_sUart[UART_WIFI].cReveiveBuf, UDP_MODE);
				}	
			}
		}	
		else
		{
			WifiModuleDataParseHandle((unsigned char *)g_sUart[UART_WIFI].cReveiveBuf);
		}	
	}
	//ClearUartBuffer(UART_WIFI);
	{
	g_sUart[UART_WIFI].iPushIndex = 0;
	memset(g_sUart[UART_WIFI].cReveiveBuf, END_FLAG, UART_WIFI_BUFFER_SIZE);
	}
	g_sUart[UART_WIFI].bRxControl = ENABLE;
}

