#include "sys.h"
#include "key.h"
#include "uart.h"
#include "timer.h"
#include "led.h"
#include "tone.h"
#include "eeprom.h"
#include "WIFI.h"
#include "power.h"

///////////////////////////SOS key///////////////////////////////////////
void Key1Init(void)
{
	AUXR2 &= 0xEF; //CLOSE EXT2 INTERRUPT
	P3M1 |= 0x40;
	P3M0 &= 0xBF;
	g_SystemVar.Key1Status = KEY_PAD_IDLE_STATE;

	SysTaskEnable(SCAN_KEY_1_TASK_ID, SYS_RUN_TASK_IMMEDIATELY);
}

TaskTimeout_t ScanKey1(void)
{
	bit KeyStatus;
	TaskTimeout_t cTimeToRun;
	
	cTimeToRun = SYS_RUN_TASK_IMMEDIATELY;
	switch(g_SystemVar.Key1Status)
	{
		case KEY_PAD_IDLE_STATE:
			SysPrintf("\r\nKEY_PAD_IDLE_STATE.");
			KeyStatus = p_S1;
			g_SystemVar.Key1DebounceCounter = 0;
			if(KeyStatus == KEY_RELEASE)
			{
				g_SystemVar.Key1Status = KEY_PAD_SCAN_GPIO_STATE;
			}
			break;
			
		case KEY_PAD_SCAN_GPIO_STATE:
			KeyStatus = p_S1;
			if(KeyStatus == KEY_PRESS)
			{
				g_SystemVar.Key1Status = KEY_PAD_DEBOUNCE_STATE;
			}
			break;
			
		case KEY_PAD_DEBOUNCE_STATE:
			KeyStatus = p_S1;
			if(KeyStatus == KEY_PRESS)
			{
				g_SystemVar.Key1DebounceCounter++;
				if(g_SystemVar.Key1DebounceCounter == KEY_SCAN_DEBOUNCE_COUNT)
				{
					g_SystemVar.Key1Status = KEY_PAD_WAIT_RELEASE_STATE;
				}	
			}
			else
			{
				g_SystemVar.Key1Status = KEY_PAD_SCAN_END_STATE;
			}
			break;
			
		case KEY_PAD_WAIT_RELEASE_STATE:
			KeyStatus = p_S1;
			if(KeyStatus == KEY_RELEASE)
			{
				g_SystemVar.Key1Status = KEY_PAD_SCAN_END_STATE;
				if(g_SystemVar.Key1DebounceCounter < KEY_SCAN_LONG_PRESS_TIMEOUT)
				{
					HostMsgPost(SYS_MSG_KEY_PRESS, SYS_MSG_KEY1_PRESS);
				}	
			}
			else
			{
				if(g_SystemVar.Key1DebounceCounter < KEY_SCAN_LONG_PRESS_TIMEOUT)
					g_SystemVar.Key1DebounceCounter++;
				if(g_SystemVar.Key1DebounceCounter == KEY_SCAN_LONG_PRESS_TIMEOUT)
				{
					g_SystemVar.Key1DebounceCounter++;
					HostMsgPost(SYS_MSG_KEY_PRESS, SYS_MSG_KEY1_LONG_PRESS);
				}
			}
			break;
			
		case KEY_PAD_SCAN_END_STATE:
			g_SystemVar.Key1Status = KEY_PAD_IDLE_STATE;
			break;			
	}
	
	return cTimeToRun;
}
///////////////////////REGISTER KEY//////////////////////////////////////////
void Key2Init(void)
{
	P3M1 |= 0x20;
	P3M0 &= 0xdF;
	
	g_SystemVar.Key2Status = KEY_PAD_IDLE_STATE;
	g_SystemVar.Key2DebounceCounter = 0;
	SysTaskEnable(SCAN_KEY_2_TASK_ID, SYS_RUN_TASK_IMMEDIATELY);
}

TaskTimeout_t ScanKey2(void)
{
	bit KeyStatus;
	TaskTimeout_t cTimeToRun;
	
	cTimeToRun = SYS_RUN_TASK_IMMEDIATELY;
	switch(g_SystemVar.Key2Status)
	{
		case KEY_PAD_IDLE_STATE:
			KeyStatus = p_S2;
			g_SystemVar.Key2DebounceCounter = 0;
			if(KeyStatus == KEY_RELEASE)
			{
				g_SystemVar.Key2Status = KEY_PAD_SCAN_GPIO_STATE;
			}
			break;
			
		case KEY_PAD_SCAN_GPIO_STATE:
			KeyStatus = p_S2;
			if(KeyStatus == KEY_PRESS)
			{
				g_SystemVar.Key2Status = KEY_PAD_DEBOUNCE_STATE;
			}
			break;
			
		case KEY_PAD_DEBOUNCE_STATE:
			KeyStatus = p_S2;
			if(KeyStatus == KEY_PRESS)
			{
				g_SystemVar.Key2DebounceCounter++;
				if(g_SystemVar.Key2DebounceCounter == KEY_SCAN_DEBOUNCE_COUNT)
				{
					g_SystemVar.Key2Status = KEY_PAD_WAIT_RELEASE_STATE;
				}
			}
			else
			{
				g_SystemVar.Key2Status = KEY_PAD_SCAN_END_STATE;
			}
			break;
			
		case KEY_PAD_WAIT_RELEASE_STATE:
			KeyStatus = p_S2;
			if(KeyStatus == KEY_RELEASE)
			{
				g_SystemVar.Key2Status = KEY_PAD_SCAN_END_STATE;
				if(g_SystemVar.Key2DebounceCounter < KEY_SCAN_LONG_PRESS_TIMEOUT)
				{
					HostMsgPost(SYS_MSG_KEY_PRESS, SYS_MSG_KEY2_PRESS);
				}
			}
			else
			{
				if(g_SystemVar.Key2DebounceCounter < KEY_SCAN_LONG_PRESS_TIMEOUT)
					g_SystemVar.Key2DebounceCounter++;
				if(g_SystemVar.Key2DebounceCounter==KEY_SCAN_LONG_PRESS_TIMEOUT)
				{
					g_SystemVar.Key2DebounceCounter++;
					HostMsgPost(SYS_MSG_KEY_PRESS, SYS_MSG_KEY2_LONG_PRESS);
				}
			}
			break;
			
		case KEY_PAD_SCAN_END_STATE:
			g_SystemVar.Key2Status = KEY_PAD_IDLE_STATE;
			break;			
	}
	
	return cTimeToRun;
}

void KeyInit(void)
{
	Key1Init();
	Key2Init();
}

void KeyPadHandler(MSG_t *pMsg)
{
	switch(pMsg->Param)
	{
		case SYS_MSG_KEY2_PRESS:
			if(g_SystemVar.ACStatus == POWER_DC_IN)
			{
				if(g_WifiVar.cSetConnectApFlag == FALSE)
				{
					HostMsgPost(SYS_MSG_WIFI_SET_AP_CONNECTED, NULL);
				}
				else
				{
					HostMsgPost(SYS_MSG_WIFI_EXIT_SET_AP_CONNECTED, NULL);
				}
			}
			BeepTone(BEEP_NORMAL);
			SysPrintf("\r\nSYS_MSG_KEY2_PRESS");
			break;

		case SYS_MSG_KEY1_PRESS:
			if(g_SystemVar.SystemCurrentStatus == SYSTEM_RF_REGISTER_STATUS)
			{
				HostMsgPost(SYS_MSG_LEFT_CURRENT_STATUS, g_SystemVar.SystemLastStatus);	
			}
			else
			{
				HostMsgPost(SYS_MSG_LEFT_CURRENT_STATUS, SYSTEM_RF_REGISTER_STATUS);
			}
			BeepTone(BEEP_NORMAL);
			break;
			
		case SYS_MSG_KEY2_LONG_PRESS:
			SysPrintf("\r\nSYS_MSG_KEY2_LONG_PRESS");
			HostMsgPost(SYS_MSG_POWER_OFF, NULL);
			BeepTone(BEEP_NORMAL);
			break;
			
		case SYS_MSG_KEY1_LONG_PRESS:
			SysPrintf("\r\nSYS_MSG_KEY1_LONG_PRESS");
			EE_Data_Default();
			BeepTone(BEEP_CONFIRM);
			HostMsgPost(SYS_MSG_WIFI_DEFAULT, NULL);
			break;			
	}
	
}

