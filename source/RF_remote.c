#include "sys.h"
#include "uart.h"
#include "RF_remote.h"
#include "led.h"
#include "timer.h"
#include "key.h"
#include "eeprom.h"
#include "RFID.h"
#include "tone.h"
#include "WIFI.h"

bit RfRxPinStatus;
unsigned int RFLowLevelStart;
unsigned int RFLowLevelLenght;
unsigned int RFLowLevelEnd;

xdata RF_Receive_t g_RFDecode;
xdata RF_Store_t g_RFStore;
xdata RF_Adr_Node_t g_RfAdrNode;
xdata RF_Adr_Node_t g_RfAdrNodeTemp;

void RFEnable(void)
{
	p_RF_EN = LEVEL_LOW;
}

void RFDisable(void)
{
	p_RF_EN = LEVEL_HIGH;
}

void RFInit(void)
{
	g_RFDecode.BitCounter = 0;
	g_RFDecode.Address1 = 0;
	g_RFDecode.Address2 = 0;
	g_RFDecode.Data = 0;
	g_RFDecode.Decodestart = FALSE;
	g_RFDecode.DecodeCheck = FALSE;
	g_RFDecode.DecodeControl = ENABLE;
	g_RFDecode.MaxHandleTime = 0;
	g_RFDecode.DecodeInterval = 0;
	RFLowLevelStart = 0;	
///////////////////////////////////////////////
////////////////////////////////////////////////	
	p_RF = 1;
	IT0 = 0;                
	EX0 = 1; 
	PX0 = 1;
//	EA = 1;  
////////p_RF/////////////////////////////
	P3M1 |= 0x04;
	P3M0 &= 0xFB;
////////p_RF_EN/////////////////////////////
#if defined(HW_T0_2)
	P3M1 |= 0x08;
	P3M0 &= 0xF7;
#else
	P2M0 |= 0x20;
	P2M1 &= 0xDF;
#endif
	RFEnable();

	
}

void exint0() interrupt 0   
{
	RfRxPinStatus = p_RF;
	if(RfRxPinStatus == 1)
	{
		if(g_RFDecode.DecodeControl == ENABLE)
		{
			RFLowLevelEnd = CH;
			RFLowLevelEnd <<= 8;	
			RFLowLevelEnd = RFLowLevelEnd | CL;	
			if(RFLowLevelStart <= RFLowLevelEnd)
			{
				RFLowLevelLenght = RFLowLevelEnd - RFLowLevelStart;
			}
			else
			{
				RFLowLevelLenght =0xffff - RFLowLevelStart + RFLowLevelEnd;
			}
			
			if(g_RFDecode.Decodestart == FALSE)
			{
				if((RFLowLevelLenght > HEAD_MIN) && (RFLowLevelLenght < HEAD_MAX))	
				{
					RFLowLevelLenght = 0;
					g_RFDecode.BitCounter = RF_CODE_BIT_LENGTH;
					g_RFDecode.Address1 = 0;
					g_RFDecode.Address2 = 0;
					g_RFDecode.Data = 0;
					g_RFDecode.Decodestart = TRUE;
				}	
			}
			else
			{
				if(((RFLowLevelLenght > L_MIN)&&(RFLowLevelLenght < L_MAX))||((RFLowLevelLenght > H_MIN)&&(RFLowLevelLenght < H_MAX)))
				{
					if(g_RFDecode.BitCounter == RF_CODE_SECOND_BYTE)
					{
						g_RFDecode.Address2 = g_RFDecode.Data;	
						g_RFDecode.Data = 0;
					}

					if(g_RFDecode.BitCounter == RF_CODE_FIRST_BYTE)
					{
						g_RFDecode.Address1 = g_RFDecode.Data;
						g_RFDecode.Data = 0;
					}

					g_RFDecode.Data <<= 1;

					if(RFLowLevelLenght < H_MIN)
					{
						g_RFDecode.Data |= 0x01;
					}
					else
					{
						g_RFDecode.Data &= 0xfe;
					}

					g_RFDecode.BitCounter--;

					if(g_RFDecode.BitCounter == 0)
					{	
						g_RFDecode.MaxHandleTime = MAX_HANDLE_REMOTE_CODE_TIME;
						g_RFDecode.Decodestart = FALSE;
						if(g_RFDecode.DecodeCheck == FALSE)
						{
							g_RFDecode.CodeBuffer[0] = g_RFDecode.Address1;
							g_RFDecode.CodeBuffer[1] = g_RFDecode.Address2;
							g_RFDecode.CodeBuffer[2] = g_RFDecode.Data;
							g_RFDecode.DecodeCheck = TRUE;
						}
						else
						{
							if((g_RFDecode.CodeBuffer[0]==g_RFDecode.Address1)
								&&(g_RFDecode.CodeBuffer[1]==g_RFDecode.Address2)
								&&(g_RFDecode.CodeBuffer[2]==g_RFDecode.Data))
							{
								g_RFDecode.DecodeCheck = FALSE;
								g_RFDecode.DecodeControl = DISABLE;
								HostMsgPost(SYS_MSG_RF_DECODE_COMPLETE, g_RFDecode.CodeBuffer[2]);
							}
							else
							{
								g_RFDecode.CodeBuffer[0] = g_RFDecode.Address1;
								g_RFDecode.CodeBuffer[1] = g_RFDecode.Address2;
								g_RFDecode.CodeBuffer[2] = g_RFDecode.Data;								
							}
						}
					}
				}
				else
				{
					g_RFDecode.Decodestart = FALSE;			
				}		
			}
		}

	}
	else
	{
		RFLowLevelStart = CH;
		RFLowLevelStart <<= 8;	
		RFLowLevelStart = RFLowLevelStart | CL;	
	}	
}

void RFRoutineHandle(void)
{
	if(g_RFDecode.MaxHandleTime > 0)
	{
		if(--g_RFDecode.MaxHandleTime == 0)
		{
			g_RFDecode.DecodeControl = ENABLE;
			g_RFDecode.DecodeCheck = FALSE;
		}
	}

	if(g_RFDecode.RegisterTimeOut > 0)
	{
		if(--g_RFDecode.RegisterTimeOut == 0)
		{
			HostMsgPost(SYS_MSG_LEFT_CURRENT_STATUS, g_SystemVar.SystemLastStatus);
		}
	}
}

void RFTickHandle(void)
{
	if(g_RFDecode.DecodeInterval > 0)
	{
		if(--g_RFDecode.DecodeInterval == 0)
		{
			g_RFDecode.DecodeControl = ENABLE;
		}
	}
}

unsigned char GetRFtype(unsigned char OperateCode)
{
	unsigned char RetVal;

	RetVal = RF_UNDEFINE;
	switch(OperateCode)
	{
		case RF_COMMAND_ARMED:
		case RF_COMMAND_DISARMED:
		case RF_COMMAND_HOME_ARMED:
		case RF_COMMAND_SOS_ALARM:
			RetVal = RF_RC;
			break;

		case RF_COMMAND_24_HOUR_ZONE:
		case RF_COMMAND_DELAY_ZONE:
		case RF_COMMAND_NORMAL_ZONE:
		case RF_COMMAND_NORMAL_ZONE_1:	
		case RF_COMMAND_AT_HOME_ZONE:
		case RF_COMMAND_AT_HOME_ZONE_1:	
		case RF_COMMAND_ACCESSORY_LOW_VOLTAGE:
		case RF_COMMAND_ACCESSORY_TAMPER:	
			RetVal = RF_ZONE;
			break;
			
		case RF_COMMAND_DOOR_BELL:
			RetVal = RF_DOOR_BELL;
			break;
	}

	return RetVal;
}

unsigned char RfSearchMatchCode(unsigned char *pBuf, unsigned char RFType)
{
	unsigned int Store_Start_Adr;
	unsigned char ret_val;
	unsigned char index;
	unsigned char Max_Store_Num;
	
	#if 1
	{
		SysPrintf("\r\nRF code:");
		for(index=0; index<RF_CODE_LENGTH; index++)
			SysPrintf(" %x",pBuf[index]);

	}
	#endif
	if((pBuf[0] == 0xff)
		&&(pBuf[1] == 0xff)
		&&((pBuf[2]&0xf0)== 0xf0))
	{
		return RF_MATCH_FAIL;
	}

	ret_val = RF_MATCH_FAIL;

	switch(RFType)
	{
		case RF_RC:
			Max_Store_Num = MAX_STORE_REMOTE_NUM;
			Store_Start_Adr = EE_REMOTE_CONTROL_START_ADDRESS;
			break;

		case RF_ZONE:	
			Max_Store_Num = MAX_STORE_WIRELESS_ACCESSORY_NUM;
			Store_Start_Adr = EE_WIRELESS_ACCESSORY_START_ADDRESS;
			break;

		case RF_DOOR_BELL:
			Max_Store_Num = MAX_STORE_WIRELESS_DOOR_BELL_NUM;
			Store_Start_Adr = EE_WIRELESS_DOOR_BELL_START_ADDRESS;
			break;
	}
	
	for(index=0; index<Max_Store_Num; index++)
	{
		EE_Read_Data((Store_Start_Adr+sizeof(RF_Store_t)*index), (unsigned char *)&g_RFStore, sizeof(RF_Store_t));
		if((pBuf[0] == g_RFStore.RFCode[0])
			&&(pBuf[1] == g_RFStore.RFCode[1])
			&&((pBuf[2]&0xf0)== (g_RFStore.RFCode[2]&0xf0)))
		{
			ret_val = index + 1;	
			break;
		}
		
	}
	return ret_val;
}

unsigned char RfSearchValidStoreLocate(unsigned char RFType)
{
	unsigned int Store_Start_Adr;
	unsigned char ret_val;
	unsigned char index;
	unsigned char Max_Store_Num;
	
	
	ret_val = 0;

	switch(RFType)
	{
		case RF_RC:
			Max_Store_Num = MAX_STORE_REMOTE_NUM;
			Store_Start_Adr = EE_REMOTE_CONTROL_START_ADDRESS;
			break;

		case RF_ZONE:	
			Max_Store_Num = MAX_STORE_WIRELESS_ACCESSORY_NUM;
			Store_Start_Adr = EE_WIRELESS_ACCESSORY_START_ADDRESS;
			break;

		case RF_DOOR_BELL:
			Max_Store_Num = MAX_STORE_WIRELESS_DOOR_BELL_NUM;
			Store_Start_Adr = EE_WIRELESS_DOOR_BELL_START_ADDRESS;
			break;
	}
	
	for(index=0; index<Max_Store_Num; index++)
	{
		EE_Read_Data((Store_Start_Adr+sizeof(RF_Store_t)*index), (unsigned char *)&g_RFStore, sizeof(RF_Store_t));
		if((END_FLAG == g_RFStore.RFCode[0])
			&&(END_FLAG == g_RFStore.RFCode[1])
			&&(END_FLAG == (g_RFStore.RFCode[2])))
		{
			ret_val = index + 1;	
			break;
		}
		
	}
	
	return ret_val;
}

unsigned char GetRFMode(unsigned char OperateCode)
{
	unsigned char RetVal;

	RetVal = MODE_ARM;
	switch(OperateCode)
	{
		case RF_COMMAND_24_HOUR_ZONE:
			RetVal = MODE_24HOUR;
			break;
			
		case RF_COMMAND_DELAY_ZONE:
			RetVal = MODE_DELAY;
			break;
			
		case RF_COMMAND_NORMAL_ZONE:
		case RF_COMMAND_NORMAL_ZONE_1:	
			RetVal = MODE_ARM;
			break;
			
		case RF_COMMAND_AT_HOME_ZONE:
		case RF_COMMAND_AT_HOME_ZONE_1:	
			RetVal = MODE_BYPASS;
			break;			
	}

	return RetVal;
}

unsigned int GetZoneNameAdr(unsigned char index)
{
	unsigned int RetVal;

	if(index > MAX_THIRD_BLOCK_ZONE_NAME_NUM)
	{
		RetVal = EE_ZONE_NAME_FORTH_START_ADDRESS + (index - 1 - MAX_THIRD_BLOCK_ZONE_NAME_NUM)*RF_NAME_LENGHT;
	}
	else if(index > MAX_SECOND_BLOCK_ZONE_NAME_NUM)
	{
		RetVal = EE_ZONE_NAME_THIRD_START_ADDRESS + (index - 1 - MAX_SECOND_BLOCK_ZONE_NAME_NUM)*RF_NAME_LENGHT;
	}
	else if(index > MAX_FIRST_BLOCK_ZONE_NAME_NUM)
	{
		RetVal = EE_ZONE_NAME_SECOND_START_ADDRESS + (index - 1 - MAX_FIRST_BLOCK_ZONE_NAME_NUM)*RF_NAME_LENGHT;
	}
	else
	{
		RetVal = EE_ZONE_NAME_FIRST_START_ADDRESS + (index - 1)*RF_NAME_LENGHT;
	}
	
	return RetVal;
}

void RFZoneAlarm(unsigned char Cmd, unsigned char Index)
{
	unsigned int Param;
	unsigned char ModeFlag;

	Param = g_RFStore.Type;
	Param <<= 4;
	Param += CASE_ALARM;
	Param <<= 8;
	Param += Index;
	ModeFlag = TRUE;
	switch(g_RFStore.Mode)
	{
		case MODE_ARM:
			HostMsgPost(SYS_MSG_RF_NORMAL_ZONE, Param);
			break;

		case MODE_BYPASS:
			HostMsgPost(SYS_MSG_RF_AT_HOME_ZONE, Param);
			break;

		case MODE_24HOUR:
			HostMsgPost(SYS_MSG_RF_24HOUR_ZONE, Param);
			break;

		case MODE_DELAY:
			if((g_SystemParam.DelayAlarmTime>0) 
				&& (g_SystemParam.SystemStatus!=SYSTEM_DISARMED_STATUS))
			{
				if(g_SystemVar.DelayAlarmStatus == FALSE)
				{
					g_SystemVar.DelayAlarmStatus = TRUE;
					g_SystemVar.DelayAlarmCount = g_SystemParam.DelayAlarmTime;
					g_SystemVar.DelayAlarmParam = Param;
				}
			}
			else
			{
				HostMsgPost( SYS_MSG_RF_DELAY_ZONE, Param);
			}
			break;	

		default:
			ModeFlag = FALSE;
			break;
	}

	if(ModeFlag == FALSE)
	{
		switch(Cmd)
		{
			case RF_COMMAND_AT_HOME_ZONE:
			case RF_COMMAND_AT_HOME_ZONE_1:	
				HostMsgPost( SYS_MSG_RF_AT_HOME_ZONE, Param);
				break;

			case RF_COMMAND_NORMAL_ZONE:
			case RF_COMMAND_NORMAL_ZONE_1:	
				HostMsgPost( SYS_MSG_RF_NORMAL_ZONE, Param);
				break;
				
			case RF_COMMAND_24_HOUR_ZONE:
				HostMsgPost( SYS_MSG_RF_24HOUR_ZONE, Param);
				break;
				
			case RF_COMMAND_DELAY_ZONE:
				if((g_SystemParam.DelayAlarmTime>0) 
					&& (g_SystemParam.SystemStatus!=SYSTEM_DISARMED_STATUS))
				{
					if(g_SystemVar.DelayAlarmStatus == FALSE)
					{
						g_SystemVar.DelayAlarmStatus = TRUE;
						g_SystemVar.DelayAlarmCount = g_SystemParam.DelayAlarmTime;
						g_SystemVar.DelayAlarmParam = Param;
					}
				}
				else
				{
					HostMsgPost( SYS_MSG_RF_DELAY_ZONE, Param);
				}
				break;		
		}
	}
}

void RFCommandHandle(unsigned char Cmd, unsigned char Index)
{
	unsigned int Param;
	
	switch(Cmd)
	{
		case RF_COMMAND_ARMED:
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
			{
				BeepTone(BEEP_NORMAL);
				HostMsgPost(SYS_MSG_LEFT_CURRENT_STATUS, SYSTEM_ARMED_STATUS);
			}	
			
			SysPrintf("\r\nRF_COMMAND_ARMED.");
			break;
			
		case RF_COMMAND_DISARMED:
			BeepTone(BEEP_CONFIRM);
			HostMsgPost( SYS_MSG_LEFT_CURRENT_STATUS, SYSTEM_DISARMED_STATUS);
			SysPrintf("\r\nRF_COMMAND_DISARMED.");
			break;
		
		case RF_COMMAND_HOME_ARMED:
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
			{
				BeepTone(BEEP_NORMAL);
				HostMsgPost( SYS_MSG_LEFT_CURRENT_STATUS, SYSTEM_HOME_ARMED_STATUS);	
			}
			
			SysPrintf("\r\nRF_COMMAND_HOME_ARMED.");
			break;
			
		case RF_COMMAND_SOS_ALARM:
			Param = CASE_SOS|(N_RT<<4);
			Param <<= 8;
			Param += Index;
			BeepTone(BEEP_NORMAL);
			HostMsgPost( SYS_MSG_RF_SOS_CMD, Param);
			SysPrintf("\r\nRF_COMMAND_SOS_ALARM.");
			break;

		case RF_COMMAND_DOOR_BELL:
			if(g_RFStore.Mode == MODE_ARM)
			{
				AlarmInforPush(CASE_NOP, N_DOOR_BELL, Index);
			}
			else
			{
				AlarmInforPush(CASE_SOS, N_DOOR_BELL, Index);
			}	
			HostMsgPost( SYS_MSG_PLAY_PROMPT_TONE, BELL_TONE);
			SysPrintf("\r\nRF_COMMAND_DOOR_BELL.");
			break;
			
		case RF_COMMAND_AT_HOME_ZONE:
		case RF_COMMAND_AT_HOME_ZONE_1:	
			RFZoneAlarm(Cmd, Index);
			SysPrintf("\r\nRF_COMMAND_AT_HOME_ZONE.");
			break;

		case RF_COMMAND_NORMAL_ZONE:
		case RF_COMMAND_NORMAL_ZONE_1:	
			RFZoneAlarm(Cmd, Index);
			SysPrintf("\r\nRF_COMMAND_NORMAL_ZONE.");
			break;
			
		case RF_COMMAND_24_HOUR_ZONE:
			RFZoneAlarm(Cmd, Index);
			SysPrintf("\r\nRF_COMMAND_24_HOUR_ZONE.");
			break;
			
		case RF_COMMAND_DELAY_ZONE:
			RFZoneAlarm(Cmd, Index);
			SysPrintf("\r\nRF_COMMAND_DELAY_ZONE.");
			break;
			
		case RF_COMMAND_ACCESSORY_LOW_VOLTAGE:
			Param = g_RFStore.Type;
			Param <<= 4;
			Param += CASE_LB;
			Param <<= 8;
			Param += Index;
			HostMsgPost( SYS_MSG_RF_ACCESSORY_LOW_BAT, Param);
			SysPrintf("\r\nRF_COMMAND_ACCESSORY_LOW_VOLTAGE.");
			break;

		case RF_COMMAND_ACCESSORY_TAMPER:
			Param = g_RFStore.Type;
			Param <<= 4;
			Param += CASE_MOVE;
			Param <<= 8;
			Param += Index;
			HostMsgPost( SYS_MSG_RF_ACCESSORY_TAMPER, Param);
			SysPrintf("\r\nRF_COMMAND_ACCESSORY_TAMPER.");
			break;			
	}
}



void RFReceiveHandler(void)
{
	unsigned char Index;
	unsigned char RFType;
	
	SysPrintf("\r\nRFReceiveHandler.");
	RFType = GetRFtype(g_RFDecode.CodeBuffer[2]&0X0F);
	if(RFType != RF_UNDEFINE)
	{
		Index = RfSearchMatchCode(g_RFDecode.CodeBuffer, RFType);
		if(Index > 0)
		{
			if(g_RFStore.Control == RF_CONTROL_ENABLE)
			{
				RFCommandHandle((g_RFDecode.CodeBuffer[2]&0x0f), Index);
			}
		}	
	}
	g_RFDecode.DecodeInterval = RF_DECODE_INTERVAL_TIME;	
}

void RFCodeStore(unsigned char RFType)
{
	unsigned int Store_Start_Adr;
	
	switch(RFType)
	{
		case RF_RC:
			g_SystemParam.RegisteredRemoteNum++;
			Store_Start_Adr = EE_REMOTE_CONTROL_START_ADDRESS;
			break;

		case RF_ZONE:
			g_SystemParam.RegisteredWirelessAccessoryNum++;
			Store_Start_Adr = EE_WIRELESS_ACCESSORY_START_ADDRESS;
			break;
			
		case RF_DOOR_BELL:
			g_SystemParam.RegisteredDoorBellNum++;
			Store_Start_Adr = EE_WIRELESS_DOOR_BELL_START_ADDRESS;
			break;
	}


	EE_Write_Sector(Store_Start_Adr+(g_RFStore.CodeLocate-1)*sizeof(RF_Store_t), sizeof(RF_Store_t), (unsigned char *)&g_RFStore);
	AddRfAdrItem(Store_Start_Adr+(g_RFStore.CodeLocate-1)*sizeof(RF_Store_t));
	//SysParamToEeprom();
}
void RFRegisterHandle(unsigned char *pBuf, RF_Regiseter_Mode_t RegisterMode)
{
	unsigned char RetVal;
	unsigned char RRType;
	
	SysPrintf("\r\nRFRegisterHandle.");
	if(RegisterMode == RF_FROM_TRIGER)
	{
		RRType = GetRFtype(pBuf[2]&0x0f);
		if(RRType != RF_UNDEFINE)
		{
			RetVal = RfSearchMatchCode(pBuf, RRType);
			if(RetVal == RF_MATCH_FAIL)
			{
				if(RRType == RF_RC)
				{
					g_RFStore.Type = N_RT;
				}	
				else if(RRType == RF_DOOR_BELL)
				{
					g_RFStore.Type = N_DOOR_BELL;
				}
				else
				{
					g_RFStore.Type = N_UNDEFINE;
				}
				g_RFStore.CodeLocate = RfSearchValidStoreLocate(RRType);
				if(g_RFStore.CodeLocate > 0)
				{
					g_RFStore.Mode = GetRFMode(pBuf[2]&0x0f);
					g_RFStore.NameFlag = FALSE;
					g_RFStore.Control = RF_CONTROL_ENABLE;
					memcpy(g_RFStore.RFCode, pBuf, RF_CODE_LENGTH);
					RFCodeStore(RRType);
					BeepTone(BEEP_NORMAL);
				}
				else
				{
					BeepTone(BEEP_TWO);
				}
			}
			else
			{
				BeepTone(BEEP_TWO);
			}
		}
		else
		{
			BeepTone(BEEP_TWO);
		}
	}
}



void SystemRfRegisterStatus(void)
{
	SysPrintf("\r\nEntry system RF register status.");
	g_SystemVar.SystemCurrentStatus = g_SystemVar.SystemNextStatus;
	g_RFDecode.RegisterTimeOut = MAX_REGISTER_TIME;
	LedIndicator(LED_SET_INDICATOR, LED_ON);
	while(g_SystemVar.SystemCurrentStatus == g_SystemVar.SystemNextStatus)
	{
		HostMsgPend();
		switch(g_HostMsg.msgID)
		{
			case SYS_MSG_RF_DECODE_COMPLETE:
				RFRegisterHandle(g_RFDecode.CodeBuffer, RF_FROM_TRIGER);
				g_RFDecode.DecodeInterval = RF_DECODE_INTERVAL_TIME;
				g_HostMsg.msgID = SYS_MSG_NULL;
				HostMsgPost( SYS_MSG_LEFT_CURRENT_STATUS, g_SystemVar.SystemLastStatus);
				break;
				
			case SYS_MSG_LEFT_CURRENT_STATUS:
				LedIndicator(LED_SET_INDICATOR, LED_OFF);
				g_RFDecode.RegisterTimeOut = 0;
				g_SystemVar.SystemNextStatus = g_HostMsg.Param;
				HostMsgPost( SYS_MSG_INIT_CURRENT_STATUS, NULL);
				break;
		}
		
		SystemSever(&g_HostMsg);
	}
	SysPrintf("\r\nExit system RF register status.");
}

////////////////////////////////////////////////////////////////////////////////////
void AddRfAdrItem(unsigned int RfStoreAdr)
{
	unsigned int *pTemp;

	SysPrintf("\r\nAddRfAdrItem: %xi", RfStoreAdr);
	EE_Read_Data(EE_RF_ADDRESS_MAP_START, g_HostCommonBuf, HOST_COMMON_BUF_SIZE);
	pTemp = (unsigned int *)g_HostCommonBuf;
	*(pTemp+g_SystemParam.RfAdrMapTotal) =  RfStoreAdr; 
	g_SystemParam.RfAdrMapTotal++;
	DelayNop(50);
	EE_WriteSector(EE_RF_ADDRESS_MAP_START, g_SystemParam.RfAdrMapTotal*FLASH_ADDRESS_SIZE, g_HostCommonBuf);
	SysParamToEeprom();
}

void DeleteRfAdrItem(unsigned int RfStoreAdr)
{
	unsigned int *pTemp;
	unsigned char Index;
	
	SysPrintf("\r\nDeleteRfAdrItem: %xi", RfStoreAdr);
	memset(g_HostCommonBuf, END_FLAG, HOST_COMMON_BUF_SIZE);
	EE_Read_Data(EE_RF_ADDRESS_MAP_START, g_HostCommonBuf, HOST_COMMON_BUF_SIZE);
	pTemp = (unsigned int *)g_HostCommonBuf;
	for(Index=0; Index<g_SystemParam.RfAdrMapTotal; Index++)
	{
		if(*pTemp == RfStoreAdr)
		{
			DelayNop(50);
			if(Index ==(g_SystemParam.RfAdrMapTotal-1))
			{
				memset(g_HostCommonBuf, END_FLAG, sizeof(unsigned int));
				EE_Write_Sector(EE_RF_ADDRESS_MAP_START+Index*sizeof(unsigned int), sizeof(unsigned int), g_HostCommonBuf);
			}
			else
			{
				EE_Write_Sector(EE_RF_ADDRESS_MAP_START+Index*sizeof(unsigned int), (g_SystemParam.RfAdrMapTotal-1-Index)*sizeof(unsigned int), g_HostCommonBuf+(Index+1)*sizeof(unsigned int));
			}
			
			break;
		}
		pTemp++;
	}
	g_SystemParam.RfAdrMapTotal--;
	SysParamToEeprom();
}

///////////////////////////////////////////////////////////////////////////////////////
