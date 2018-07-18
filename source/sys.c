#include "sys.h"
#include "SysClk.h"
#include "timer.h"
#include "key.h"
#include "uart.h"
#include "PCA.h"
#include "RF_remote.h"
#include "led.h"
#include "tone.h"
#include "eeprom.h"
#include "HistoryLog.h"
#include "power.h"
#include "WIFI.h"

/////////////////////////////////////////////////////////
xdata MSG_Q_t g_HostQuene;
xdata MSG_t g_HostMsgBuf[HOST_MSG_BUF_SIZE];
xdata MSG_t g_HostMsg;
xdata System_Var_t g_SystemVar;
xdata System_Param_t g_SystemParam;
xdata unsigned char g_HostCommonBuf[HOST_COMMON_BUF_SIZE];
/////////////////////////////////////////////////////////
xdata uchar g_cSysTasksTimeTable[TASK_TABLE_LENGTH];

void AlarmInforPush(unsigned char Case, unsigned char Name, unsigned char Locate)
{
	unsigned char index;

	for(index=0; index<g_SystemVar.AlarmInforIndex; index++)
	{
		if((g_SystemVar.AlarmInforBuffer[index].Case == Case)
			&&(g_SystemVar.AlarmInforBuffer[index].Name == Name)
			&&(g_SystemVar.AlarmInforBuffer[index].Locate == Locate))
		{
			return;
		}
	}
	
	if(g_SystemVar.AlarmInforIndex < ALARM_INFOR_BUFFER_SIZE)
	{
		g_SystemVar.AlarmInforIndex++;
	}
	else
	{
		for(index=0; index<(ALARM_INFOR_BUFFER_SIZE-1); index++)
		{
			g_SystemVar.AlarmInforBuffer[index].Case = g_SystemVar.AlarmInforBuffer[index+1].Case;
			g_SystemVar.AlarmInforBuffer[index].Name = g_SystemVar.AlarmInforBuffer[index+1].Name;
			g_SystemVar.AlarmInforBuffer[index].Locate = g_SystemVar.AlarmInforBuffer[index+1].Locate;
		}	
	}
	g_SystemVar.AlarmInforBuffer[g_SystemVar.AlarmInforIndex-1].Case = Case;
	g_SystemVar.AlarmInforBuffer[g_SystemVar.AlarmInforIndex-1].Name = Name;
	g_SystemVar.AlarmInforBuffer[g_SystemVar.AlarmInforIndex-1].Locate = Locate;

}

unsigned char AlarmInforPop(unsigned char *pBuf)
{
	if(g_SystemVar.AlarmInforIndex > 0)
	{
		g_SystemVar.AlarmInforIndex--;
		*pBuf = g_SystemVar.AlarmInforBuffer[g_SystemVar.AlarmInforIndex].Case;
		*(pBuf+1) = g_SystemVar.AlarmInforBuffer[g_SystemVar.AlarmInforIndex].Name;
		*(pBuf+2) = g_SystemVar.AlarmInforBuffer[g_SystemVar.AlarmInforIndex].Locate;
		return TRUE;
	}	
	
	return FALSE;
}

void AutoUploadStatusToPlatform(void)
{
	if(--g_SystemVar.AutoUploadTime == 0)
	{
		g_SystemVar.AutoUploadTime = g_SystemParam.AutoUploadTime;
		switch(g_SystemParam.SystemStatus)
		{
			case SYSTEM_DISARMED_STATUS:
				AlarmInforPush(CASE_DISARM, N_HOST, ZONE_SELF);
				break;

			case SYSTEM_ARMED_STATUS:
				AlarmInforPush(CASE_ARM, N_HOST, ZONE_SELF);
				break;

			case SYSTEM_HOME_ARMED_STATUS:
				AlarmInforPush(CASE_BYPASS, N_HOST, ZONE_SELF);
				break;				
		}
	}
}

void SysCheckAlarmEvent(void)
{
	if(g_SystemVar.UploadPlatformControl == TRUE)
	{
		if(g_SystemVar.AlarmInforIndex > 0)
		{
			if(--g_SystemVar.CheckAlarmEventInterval == 0)
			{
				g_SystemVar.CheckAlarmEventInterval = SYS_CHECK_ALARM_EVENT_INTERVAL_TIME;
				HostMsgPost(SYS_MSG_WIFI_UPLOAD_PLATFORM, NULL);
			}	
		}
	}
}

void AutoArmDisarmHandle(void)
{
	unsigned int Temp;
	
	if((g_SystemParam.AutoArmDisarmFlag == 0)
		||((g_SystemParam.AutoArmTime == g_SystemParam.AutoDisarmTime)))
	{
		return;
	}
	
	Temp = g_RTC.Hour;
	Temp <<= 8;
	Temp += g_RTC.Minute;
	
	if(g_SystemParam.AutoArmTime == Temp)
	{
		HostMsgPost(SYS_MSG_LEFT_CURRENT_STATUS, SYSTEM_ARMED_STATUS);
	}

	if(g_SystemParam.AutoDisarmTime == Temp)
	{
		HostMsgPost(SYS_MSG_LEFT_CURRENT_STATUS, SYSTEM_DISARMED_STATUS);
	}
}

void SysDelayArmAlarmHandle(void)
{
	if(g_SystemVar.DelayArmCount > 0)
	{
		if(--g_SystemVar.DelayArmCount == 0)
		{
			HostMsgPost(SYS_MSG_LEFT_CURRENT_STATUS, g_SystemVar.DelayArmType);
		}
		else if(g_WifiVar.cSetConnectApFlag == FALSE)
		{
			HostMsgPost(SYS_MSG_PLAY_TONE, BEEP_NORMAL);
		}
	}

	if(g_SystemVar.DelayAlarmCount > 0)
	{
		if(--g_SystemVar.DelayAlarmCount == 0)
		{
			HostMsgPost( SYS_MSG_RF_DELAY_ZONE, g_SystemVar.DelayAlarmParam);
		}
		else if(g_WifiVar.cSetConnectApFlag == FALSE)
		{
			HostMsgPost(SYS_MSG_PLAY_TONE, BEEP_NORMAL);
		}
	}	
}

void SysMinuteEventHandle(void)
{
	SysPrintf("\r\nSysMinuteEventHandle.");
	AutoArmDisarmHandle();
}

TaskTimeout_t SysSecondEventHandle(void)
{
	WifiRoutineHandle();
	SysDelayArmAlarmHandle();
	AutoUploadStatusToPlatform();
	RFRoutineHandle();
	ToneRoutineHandle();
	SysCheckAlarmEvent();
	return SYS_ROUTINE_RUN_INTERVAL;
}

code TaskTable_t g_iSysTasksPtrTable[TASK_TABLE_LENGTH] = 
{
	SysTimerUnitTask,
	ScanKey1,
	ScanKey2,
	SysSecondEventHandle,
	LedTask,
	PowerACDetect,
	BatteryVoltageCheck,
	BeepTask,
};

void SysDelayArmBeepTone(MSG_t *pMsg)
{
	if(g_SystemVar.DelayArmCount > 0)
	{
		BeepTone(pMsg->Param);
		if(g_SystemVar.DelayArmCount > 10)
			SetTimeout(HOST_BEEP_TONE_TIMER, CreateMsg(SYS_MSG_DELAY_ARM_TONE, BEEP_NORMAL), SECOND(1));
		else
			SetTimeout(HOST_BEEP_TONE_TIMER, CreateMsg(SYS_MSG_DELAY_ARM_TONE, BEEP_NORMAL), MIllSECOND(5));
	}
	else if(g_SystemVar.DelayAlarmCount > 0)
	{
		BeepTone(pMsg->Param);
		if(g_SystemVar.DelayAlarmCount > 10)
			SetTimeout(HOST_BEEP_TONE_TIMER, CreateMsg(SYS_MSG_DELAY_ARM_TONE, BEEP_NORMAL), SECOND(1));
		else
			SetTimeout(HOST_BEEP_TONE_TIMER, CreateMsg(SYS_MSG_DELAY_ARM_TONE, BEEP_NORMAL), MIllSECOND(5));
	}
}

void SysPlayBeepTone(MSG_t *pMsg)
{
	BeepTone(pMsg->Param);
}

void SySTaskInit(void)
{
	uchar cIndex;

	for(cIndex=0; cIndex<TASK_TABLE_LENGTH; cIndex++)
	{
		g_cSysTasksTimeTable[cIndex] = TASK_DISABLE;
	}

	SysTaskEnable(SYS_ROUTINE_TASK_ID, SYS_RUN_TASK_IMMEDIATELY);
}

void SysRunTask (void)
{
	uchar cIndex;

	for(cIndex=0; cIndex<TASK_TABLE_LENGTH; cIndex++)
	{
		if(g_cSysTasksTimeTable[cIndex] != TASK_DISABLE)
		{
			if(g_cSysTasksTimeTable[cIndex] > 0)
			{
				g_cSysTasksTimeTable[cIndex]--;
			}
			else
			{
				g_cSysTasksTimeTable[cIndex] = TASK_TO_RUN;
				if(g_iSysTasksPtrTable[cIndex] != NULL)
				{
					g_cSysTasksTimeTable[cIndex] = g_iSysTasksPtrTable[cIndex]();
				}			
			}
		}
	}
}

void SysTaskEnable(UnitTask_t eTask, TaskTimeout_t cTime)
{
	g_cSysTasksTimeTable[eTask] = cTime;
}

/////////////////////////////////////////////////////////////////////////////////
void HostQueneInit(void)
{
	g_HostQuene.cReadIndex = 0;
	g_HostQuene.cWriteIndex = 0;
	g_HostQuene.cBufSize = HOST_MSG_BUF_SIZE;
	g_HostQuene.pMsgBuf = g_HostMsgBuf;
}

//==============================================================
MSG_t CreateMsg(unsigned int MsgID, unsigned int Param)
{
	MSG_t Msg;

	Msg.msgID = MsgID;
	Msg.Param = Param;

	return Msg;
}

void HostMsgPost(unsigned int MsgID, unsigned int Param)
{
	OS_ENTER_CRITICAL();
	if(HOST_QUENE_PTR->cWriteIndex < HOST_QUENE_PTR->cBufSize)
	{
		HOST_QUENE_PTR->pMsgBuf[HOST_QUENE_PTR->cWriteIndex].msgID = MsgID;
		HOST_QUENE_PTR->pMsgBuf[HOST_QUENE_PTR->cWriteIndex].Param = Param;
		HOST_QUENE_PTR->cWriteIndex++;
	}
	OS_EXIT_CRITICAL();
}

void HostMsgPend(void)
{
	uchar cMsgFlag;
	
	while(1)
	{
		cMsgFlag = TRUE;
		OS_ENTER_CRITICAL();
		if(HOST_QUENE_PTR->cReadIndex == HOST_QUENE_PTR->cWriteIndex)
		{
			HOST_QUENE_PTR->cReadIndex = 0;
			HOST_QUENE_PTR->cWriteIndex = 0;
			cMsgFlag = FALSE;
		}
		OS_EXIT_CRITICAL();
		
		if(cMsgFlag == TRUE)
		{
			//SysPrintf("\r\nHOST_QUENE_PTR->cReadIndex = %d \r\nHOST_QUENE_PTR->cWriteIndex = %d", HOST_QUENE_PTR->cReadIndex, HOST_QUENE_PTR->cWriteIndex);
			break;
		}
		OSWait(K_TMO,OS_TICKS_PER_SEC/10);
		
	}
	
	g_HostMsg.msgID = HOST_QUENE_PTR->pMsgBuf[HOST_QUENE_PTR->cReadIndex].msgID;
	g_HostMsg.Param = HOST_QUENE_PTR->pMsgBuf[HOST_QUENE_PTR->cReadIndex].Param;
	HOST_QUENE_PTR->cReadIndex++;
	//SysPrintf("\r\nHOST msgID = %x Param = %x", pMsg->msgID, pMsg->Param);
}
/////////////////////////////////////////////////////////////////////
void SysVarInit(void)
{	
	g_SystemVar.SystemLastStatus = SYSTEM_NULL_STATUS;
	g_SystemVar.SystemCurrentStatus = SYSTEM_NULL_STATUS;
	g_SystemVar.SystemNextStatus = g_SystemParam.SystemStatus;
	g_SystemVar.DelayArmStatus = FALSE;
	g_SystemVar.DelayAlarmStatus = FALSE;
	g_SystemVar.SirenStatus = SIREN_OFF;
	g_SystemVar.SirenResuceCount = 0;
	g_SystemVar.PWMFlag = FALSE;
	g_SystemVar.DelayAlarmCount = 0;	
	g_SystemVar.DelayArmCount = 0;	
	g_SystemVar.PlayDoorBell = FALSE;
	g_SystemVar.PlayDoorTimeout = 0;
	g_SystemVar.PlayStartupToneCount = 2;
	g_SystemVar.AutoUploadTime = g_SystemParam.AutoUploadTime;
	g_SystemVar.StartupFlag = TRUE;
	g_SystemVar.UploadPlatformControl = FALSE;
	g_SystemVar.AlarmInforIndex = 0;
	g_SystemVar.CheckAlarmEventInterval = SYS_CHECK_ALARM_EVENT_INTERVAL_TIME;
	memcpy(g_SystemVar.DeviceID, g_SystemParam.HostMacAdr, DEVICE_ID_LENGTH);
}

void SysInit(void)
{
	SysParamFromEeprom();
	SysVarInit();
	UartInit();
	HostQueneInit();
	SySTaskInit();
	PowerInit();
	TimerInit();
	WifiInit();
	LedInit();
	KeyInit();
	PCAInit();
	RFInit();
	ToneInit();
	SetTimeout(HOST_STARTUP_PROMPT_TIMER, CreateMsg(SYS_MSG_PLAY_PROMPT_TONE, STARTUP_TONE), SECOND(3));
}

void SysAlarmInfor(MSG_t *pMsg)
{
	unsigned char Case;
	unsigned char Name;
	unsigned char Locate;

	Locate = (unsigned char)pMsg->Param;
	Name = (unsigned char)(pMsg->Param>>8);
	Case = Name&0X0F;
	Name >>= 4; 
	SysPrintf("\r\nSysAlarmInfor param = %xi", pMsg->Param);
	SysPrintf("\r\nSysAlarmInfor Locate = %x Name = %x Case = %x", Locate, Name, Case);
	AlarmInforPush(Case, Name, Locate);
}

void SysAlarmHandle(MSG_t *pMsg)
{
	OpenSiren();
	LedAlarmIndicate(pMsg);
	SysAlarmInfor(pMsg);
	SaveAlarmLog((unsigned char)((pMsg->Param>>8)>>4), (unsigned char)((pMsg->Param>>8)&0x0f), (unsigned char)pMsg->Param);
	g_SystemVar.AutoUploadTime = g_SystemParam.AutoUploadTime;
}

void SysPowerOff(void)
{
	uchar Temp;
	
	BeepPlay();
	OSWait(K_TMO,OS_TICKS_PER_SEC/2);
	CloseSiren();
	RFDisable();
	WifiDisable();
	LedAllOff();
	LM386Set(LM386_OFF);
	EX0 = 0; 
	AUXR2 |= 0X20; //OPEN EXT3 FOR WAKE UP
SYS_POWER_OFF:	
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	PCON=0X02;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();

	Temp = g_RTC.Second;
	if(p_S2==0)
	{
		while(p_S2==0)
		{
			if(g_RTC.Second>(Temp+3))
			{
				IAP_CONTR=0X60; //MCU RESET
				while(1);
			}	
		}
	}
	else
	{
		if(p_AC == 0)
		{
			IAP_CONTR=0X60; //MCU RESET
			while(1);
		}
	}
	goto SYS_POWER_OFF;
}

void SystemSever(MSG_t *pMsg)
{
	switch(pMsg->msgID)
	{
		case SYS_MSG_KEY_PRESS:
			KeyPadHandler(pMsg);
			break;
			
		case SYS_MSG_RECEIVE_DATA_FROM_USART:
			UartReceiveHandler(pMsg);		
			break;
			
		case SYS_MSG_RF_DECODE_COMPLETE:
			RFReceiveHandler();
			break;
			
		case SYS_MSG_PLAY_PROMPT_TONE:
			PlayPromptTone(pMsg);
			break;
		
		case SYS_MSG_PLAY_DOOR_BELL_TIMEOUT:
			PlayDoorBellTimeOut();
			break;
			
		case SYS_MSG_HOST_SOS_ALARM:
		case SYS_MSG_RF_SOS_CMD:
		case SYS_MSG_RF_ACCESSORY_TAMPER:
		case SYS_MSG_RF_24HOUR_ZONE:
		case SYS_MSG_RF_ACCESSORY_LOW_BAT:	
			SysAlarmHandle(pMsg);
			break;
			
		case SYS_MSG_SIREN_TIME_OUT:
			CloseSiren();
			break;

		case SYS_MSG_MINUTE_UPDATE:
			SysMinuteEventHandle();
			break;
			
		case SYS_MSG_AC_SWITCH:
			PowerAcSwitchHandle();
			break;
			
		case SYS_MSG_DELAY_ARM_TONE:
			SysDelayArmBeepTone(pMsg);
			break;	

		case SYS_MSG_PLAY_TONE:
			SysPlayBeepTone(pMsg);
			break;	

		case SYS_MSG_POWER_OFF:
			SysPowerOff();
			break;	
/////////////////WIFI//////////////////////////////
		case SYS_MSG_WIFI_POWER_ON:
			WifiPowerOn();
			break;
			
		case SYS_MSG_WIFI_SEND_DEVICE_INFOR:
			WifiSendDeviceInfor();
			break;
			
		case SYS_MSG_WIFI_STARTUP_RESET:
			WifiStartupReset();
			break;
			
		case SYS_MSG_WIFI_RESET:
			WifiReset();
			break;

		case SYS_MSG_WIFI_CONFIG:
			WifiConfig();
			break;

		case SYS_MSG_WIFI_SET_AP_CONNECTED:
			WifiEntrySetConnectAp();
			break;

		case SYS_MSG_WIFI_EXIT_SET_AP_CONNECTED:
			WifiExitSetConnectAp();
			break;

		case SYS_MSG_WIFI_SET_AP_CONNECTED_SUCCESS:
			WifiSetConnectApSuccess();
			break;

		case SYS_MSG_WIFI_SET_PLATFORM_SERVER_ADR:
			WifiSetCmsAdr();
			break;
			
		case SYS_MSG_RECEIVE_DATA_FROM_WIFI:
			WifiReceiveDataHandle();
			break;

		case SYS_MSG_WIFI_DISCONNECT:
			WifiDisconnect();
			break;

		case SYS_MSG_WIFI_CONNECTED:
			WifiConnected();
			break;

		case SYS_MSG_WIFI_STATION_GOT_IP:
			WifiGetIP();
			break;
			
		case SYS_MSG_WIFI_AP_STATION_DISCONNECTED:
			WifiApStationDisconnected();
			break;

		case SYS_MSG_WIFI_AP_STATION_CONNECTED:
			WifiApStationConnected();
			break;
		
		case SYS_MSG_WIFI_LOST_CONNECT_PLATFORM:
			WifiLostConnectPlatform();
			break;
			
		case SYS_MSG_WIFI_CONNECT_PLATFORM:
			WifiConnectPlatform();
			break;	

		case SYS_MSG_WIFI_CHECK_PLATFORM_CONNECT_STATUS:
			WifiCheckPlatformConnect();
			break; 	
		
		case SYS_MSG_WIFI_UPLOAD_PLATFORM:
			WifiUploadEventToPlatform();
			break;

		case SYS_MSG_WIFI_TCP_RECONNECT:
			WifiTcpReconnect();
			break;

		case SYS_MSG_WIFI_DEFAULT:
			WifiDefault();
			break;
	}
	
}

void SystemDisarmedStatus(void)
{	
	g_SystemVar.SystemNextStatus = SYSTEM_DISARMED_STATUS;
	g_SystemVar.SystemCurrentStatus = g_SystemVar.SystemNextStatus;
	g_SystemParam.SystemStatus = g_SystemVar.SystemCurrentStatus;
	if(g_SystemVar.StartupFlag == FALSE)
	{
		SysParamToEeprom();
		g_SystemVar.AutoUploadTime = SYS_STATUS_SWITCH_UPLOAD_TIME;
	}
	else
		g_SystemVar.StartupFlag = FALSE;	
	
	SysPrintf("\r\nEntry system disarmed status.");
	while(g_SystemVar.SystemCurrentStatus == g_SystemVar.SystemNextStatus)
	{
		HostMsgPend();
		switch(g_HostMsg.msgID)
		{
			case SYS_MSG_INIT_CURRENT_STATUS:
				LedSysStatusIndicate();
				g_SystemVar.DelayArmStatus = FALSE;
				g_SystemVar.DelayArmCount = 0;
				TimerUnitDel(HOST_DELAY_ARM_TIMER);
				g_SystemVar.DelayAlarmStatus = FALSE;
				TimerUnitDel(HOST_DELAY_ARM_TIMER);
				g_SystemVar.DelayAlarmCount = 0;
				CloseSiren();
				break;
				
			case SYS_MSG_LEFT_CURRENT_STATUS:
				g_SystemVar.SystemNextStatus = g_HostMsg.Param;
				HostMsgPost( SYS_MSG_INIT_CURRENT_STATUS, g_HostMsg.Param>>8);
				SysPrintf("\r\nSYS_MSG_LEFT_CURRENT_STATUS.");
				break;		
		}
		SystemSever(&g_HostMsg);
	}
	SysPrintf("\r\nExit system disarmed status.");
}

void SystemArmedStatus(void)
{
	g_SystemVar.SystemNextStatus = SYSTEM_ARMED_STATUS;
	g_SystemVar.SystemCurrentStatus = g_SystemVar.SystemNextStatus;
	g_SystemParam.SystemStatus = g_SystemVar.SystemCurrentStatus;
	if(g_SystemVar.StartupFlag == FALSE)
	{
		SysParamToEeprom();
		g_SystemVar.AutoUploadTime = SYS_STATUS_SWITCH_UPLOAD_TIME;
	}
	else
		g_SystemVar.StartupFlag = FALSE;	
	g_SystemVar.DelayArmStatus = FALSE;
	g_SystemVar.DelayArmCount = 0;	
	TimerUnitDel(HOST_DELAY_ARM_TIMER);
	SysPrintf("\r\nEntry system armed status.");
	while(g_SystemVar.SystemCurrentStatus == g_SystemVar.SystemNextStatus)
	{
		HostMsgPend();
		switch(g_HostMsg.msgID)
		{
			case SYS_MSG_INIT_CURRENT_STATUS:
				LedSysStatusIndicate();
				break;		
		
			case SYS_MSG_RF_NORMAL_ZONE:
			case SYS_MSG_RF_AT_HOME_ZONE:
			case SYS_MSG_RF_DELAY_ZONE:
				SysAlarmHandle(&g_HostMsg);
				break;
				
			case SYS_MSG_LEFT_CURRENT_STATUS:
				g_SystemVar.SystemNextStatus = g_HostMsg.Param;
				HostMsgPost( SYS_MSG_INIT_CURRENT_STATUS, g_HostMsg.Param>>8);
				SysPrintf("\r\nSYS_MSG_LEFT_CURRENT_STATUS.");
				break;			

		}
		SystemSever(&g_HostMsg);
	}
	SysPrintf("\r\nExit system arm status.");
}

void SystemHomeArmedStatus(void)
{
	g_SystemVar.SystemNextStatus = SYSTEM_HOME_ARMED_STATUS;
	g_SystemVar.SystemCurrentStatus = g_SystemVar.SystemNextStatus;
	g_SystemParam.SystemStatus = g_SystemVar.SystemCurrentStatus;
	if(g_SystemVar.StartupFlag == FALSE)
	{
		SysParamToEeprom();
		g_SystemVar.AutoUploadTime = SYS_STATUS_SWITCH_UPLOAD_TIME;
	}
	else
		g_SystemVar.StartupFlag = FALSE;	
	g_SystemVar.DelayArmStatus = FALSE;
	g_SystemVar.DelayArmCount = 0;	
	TimerUnitDel(HOST_DELAY_ARM_TIMER);
	SysPrintf("\r\nEntry system home arm status.");
	while(g_SystemVar.SystemCurrentStatus == g_SystemVar.SystemNextStatus)
	{
		HostMsgPend();
		switch(g_HostMsg.msgID)
		{
			case SYS_MSG_INIT_CURRENT_STATUS:
				LedSysStatusIndicate();
				break;
				
			case SYS_MSG_RF_NORMAL_ZONE:
			case SYS_MSG_RF_DELAY_ZONE:
				SysAlarmHandle(&g_HostMsg);
				break;
				
			case SYS_MSG_LEFT_CURRENT_STATUS:
				g_SystemVar.SystemNextStatus = g_HostMsg.Param;
				HostMsgPost( SYS_MSG_INIT_CURRENT_STATUS, g_HostMsg.Param>>8);
				SysPrintf("\r\nSYS_MSG_LEFT_CURRENT_STATUS.");
				break;			

		}
		SystemSever(&g_HostMsg);
	}
	SysPrintf("\r\nExit system home arm status.");
}


void SystemStatusMachine(unsigned char SystemStatus)
{
	switch(SystemStatus)
	{			
		case SYSTEM_ARMED_STATUS:
			SystemArmedStatus();
			break;
			
		case SYSTEM_DISARMED_STATUS:
			SystemDisarmedStatus();
			break;
			
		case SYSTEM_HOME_ARMED_STATUS:
			SystemHomeArmedStatus();
			break;

		case SYSTEM_RF_REGISTER_STATUS:
			SystemRfRegisterStatus();
			break;

		default:
			SystemDisarmedStatus();
			break;
	}
	
	g_SystemVar.SystemLastStatus = g_SystemVar.SystemCurrentStatus;
	g_SystemVar.SystemCurrentStatus = SYSTEM_NULL_STATUS;
}



//////////////////lib function////////////////////////////
void DelayNop(unsigned char nop)
{
	unsigned char index;

	for(index=0; index<nop; index++)
	{
		;
	}
}


unsigned char AscToHex(unsigned char Asc_Data)
{
	if((Asc_Data>='0')&&(Asc_Data<='9'))
		return (Asc_Data-'0');
	else if((Asc_Data>='A')&&(Asc_Data<='F'))
		return (Asc_Data-('A'-10));
	else if((Asc_Data>='a')&&(Asc_Data<='f'))
		return (Asc_Data-('a'-10));
	return 0xff;
}





unsigned char AscStrToByte(unsigned char *Asc_Str)
{
	unsigned char temp;
	unsigned char ret_val;
	
	temp = AscToHex(*Asc_Str++);
	ret_val = temp<<4;
	temp = AscToHex(*Asc_Str++);
	ret_val += temp;

	return ret_val;
}

void HexToAsc(unsigned char *dst, unsigned char src)
{
	unsigned char Hex_H, Hex_L;
	
	Hex_H = src>>4;
	Hex_L = src&0x0f;
	
	if((Hex_H==0)||((Hex_H>0)&&(Hex_H<=9)))
	{
		*dst++ = Hex_H + '0';
	}
	else
	{
		*dst++ = Hex_H + 'A' -10;
	}

	if((Hex_L==0)||((Hex_L>0)&&(Hex_L<=9)))
	{
		*dst = Hex_L + '0';
	}
	else
	{
		*dst = Hex_L + 'A' -10;
	}	
	
}



