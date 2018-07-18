#include "sys.h"
#include "led.h"
#include "uart.h"

xdata LED_Var_t g_LedVar;

void LedRefresh(void)
{
	if(g_LedVar.SOSBlinkMode == LED_NO_BLINK)
	{
		p_SOS_LED = (g_LedVar.SOSStatus==LED_ON) ? LEVEL_HIGH : LEVEL_LOW;
	}
	else
	{
		p_SOS_LED = (g_LedVar.SOSBlinkStatus==LED_ON) ? LEVEL_HIGH : LEVEL_LOW;
	}

	if(g_LedVar.SetBlinkMode == LED_NO_BLINK)
	{
		p_SET_LED = (g_LedVar.SetStatus==LED_ON) ? LEVEL_LOW : LEVEL_HIGH;
	}
	else
	{
		p_SET_LED = (g_LedVar.SetBlinkStatus==LED_ON) ? LEVEL_LOW : LEVEL_HIGH;
	}

	if(g_LedVar.WifiBlinkMode == LED_NO_BLINK)
	{
		p_WIFI_LED = (g_LedVar.WifiStatus==LED_ON) ? LEVEL_LOW : LEVEL_HIGH;
	}
	else
	{
		p_WIFI_LED = (g_LedVar.WifiBlinkStatus==LED_ON) ? LEVEL_LOW : LEVEL_HIGH;
	}	

}

void LedInit(void)
{
#ifdef HW_T0_1 
///////p_SOS_LED///////
	P2M0 |= 0x01;
	P2M1 &= 0xfe;
#else
///////p_SOS_LED///////
	P3M0 |= 0x80;
	P3M1 &= 0x7F;
#endif	
///////p_SET_LED///////
	P2M0 |= 0x02;
	P2M1 &= 0xFD;	
///////p_WIFI_LED///////
	P1M0 |= 0x04;
	P1M1 &= 0xFB;	
	
	g_LedVar.SOSStatus = LED_OFF;
	g_LedVar.SetStatus = LED_OFF;
	g_LedVar.WifiStatus = LED_OFF;

	g_LedVar.SOSBlinkMode = LED_NO_BLINK;
	g_LedVar.SetBlinkMode = LED_NO_BLINK;
	g_LedVar.WifiBlinkMode = LED_NO_BLINK;

	g_LedVar.BlinkSynchroton = 0;
	SysTaskEnable(LED_TASK_ID, SYS_RUN_TASK_IMMEDIATELY);
}

void LedIndicator(Led_Index_t LedIndex, Led_Status_t OnOrOff)
{
	switch(LedIndex)
	{
		case LED_SOS_INDICATOR:
			g_LedVar.SOSStatus = OnOrOff;
			break;

		case LED_SET_INDICATOR:
			g_LedVar.SetStatus = OnOrOff;
			break;

		case LED_WIFI_INDICATOR:
			g_LedVar.WifiStatus = OnOrOff;
			break;			
	}
	
	LedRefresh();
}

void LedBlinkSet(Led_Index_t LedIndex, Led_Blink_Mode_t LedBlinkMode)
{
	switch(LedIndex)
	{
		case LED_SOS_INDICATOR:
			g_LedVar.SOSBlinkMode = LedBlinkMode;
			break;

		case LED_SET_INDICATOR:
			g_LedVar.SetBlinkMode = LedBlinkMode;
			break;

		case LED_WIFI_INDICATOR:
			g_LedVar.WifiBlinkMode = LedBlinkMode;
			break;			
	}	

	g_LedVar.BlinkSynchroton = 0;
}

void LedSysStatusIndicate(void)
{
	if(g_SystemParam.SystemStatus == SYSTEM_DISARMED_STATUS)
	{
		LedBlinkSet(LED_SYS_STATUS_INDICATOR, LED_NO_BLINK);
		LedIndicator(LED_SYS_STATUS_INDICATOR, LED_OFF);
	}
	else if(g_SystemParam.SystemStatus == SYSTEM_ARMED_STATUS)
	{
		LedBlinkSet(LED_SYS_STATUS_INDICATOR, LED_NO_BLINK);
		LedIndicator(LED_SYS_STATUS_INDICATOR, LED_ON);
	}
	else if(g_SystemParam.SystemStatus == SYSTEM_HOME_ARMED_STATUS)
	{
		LedBlinkSet(LED_SYS_STATUS_INDICATOR, LED_SLOW_BLINK);
	}
}


void LedCloseZoneBlink(void)
{
	LedBlinkSet(LED_SOS_INDICATOR, LED_NO_BLINK);
	LedSysStatusIndicate();
}

void LedAlarmIndicate(MSG_t *pMsg)
{
	switch(pMsg->msgID)
	{
		case SYS_MSG_HOST_SOS_ALARM:
		case SYS_MSG_RF_SOS_CMD:
		case SYS_MSG_RF_ACCESSORY_TAMPER:
		case SYS_MSG_RF_24HOUR_ZONE:
			LedBlinkSet(LED_SOS_INDICATOR, LED_FAST_BLINK);
			break;

		default:
			LedBlinkSet(LED_SOS_INDICATOR, LED_FAST_BLINK);
			break;
	}
}

void LedAllOff(void)
{
	p_SOS_LED = LEVEL_LOW;
	p_SET_LED = LEVEL_HIGH;
	p_WIFI_LED = LEVEL_HIGH;
}

TaskTimeout_t LedBlink(void)
{
	static bit Flag=0;

	if(g_LedVar.BlinkSynchroton == 0)
	{
		Flag = 1;
	}
	
	if(Flag)
	{
		if(g_LedVar.BlinkSynchroton == 20)
		{
			g_LedVar.BlinkSynchroton = 0;
		}
		g_LedVar.BlinkSynchroton++;
//////////////////////sos led//////////////////////////////		
		if(g_LedVar.SOSBlinkMode == LED_SLOW_BLINK)
		{
			if(g_LedVar.BlinkSynchroton == 1)
			{
				g_LedVar.SOSBlinkStatus = LED_ON;
			}
		}
		else
		{
			g_LedVar.SOSBlinkStatus = LED_ON;
		}
//////////////////////set led//////////////////////////////	
		if(g_LedVar.SetBlinkMode == LED_SLOW_BLINK)
		{
			if(g_LedVar.BlinkSynchroton == 1)
			{
				g_LedVar.SetBlinkStatus = LED_ON;
			}
		}
		else
		{
			g_LedVar.SetBlinkStatus = LED_ON;
		}
//////////////////////wifi led//////////////////////////////	
		if(g_LedVar.WifiBlinkMode == LED_SLOW_BLINK)
		{
			if(g_LedVar.BlinkSynchroton == 1)
			{
				g_LedVar.WifiBlinkStatus = LED_ON;
			}
		}
		else
		{
			g_LedVar.WifiBlinkStatus = LED_ON;
		}		
	}
	else
	{
		g_LedVar.SOSBlinkStatus = LED_OFF;
		g_LedVar.SetBlinkStatus = LED_OFF;
		g_LedVar.WifiBlinkStatus = LED_OFF;
	}
	Flag = ~Flag;
	
	return LED_REFRESH_TIME;
}

TaskTimeout_t LedTask(void)
{
	TaskTimeout_t TimeToRun;

	TimeToRun = LedBlink();
	LedRefresh();
	
	return TimeToRun;
}


