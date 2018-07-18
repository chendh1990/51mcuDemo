#include "sys.h"
#include "uart.h"
#include "WIFI.h"
#include "HistoryLog.h"
#include "led.h"

#include "power.h"

void PowerInit(void)
{
#ifdef HW_T0_1
	P3M1 |= 0x80;
	P3M0 &= 0x7F;
#else
	P5M1 |= 0x20;
	P5M0 &= 0xDF;
#endif	
///////////////BAT CHECK INIT///////////////////////////////////
	//P1M1 |= 0x08;
	P1M1 &= 0xF7;
	P1M0 &= 0xF7;	
	P1ASF |= P13_ASF;  
	ADC_CONTR = 0;
	ADC_RES = 0;                   
	ADC_CONTR = ADC_POWER | ADC_SPEEDLL;
	CLK_DIV &=(~ADRJ); 
	g_SystemVar.PowerStatus = BATTERY_NORMAL;
	g_SystemVar.PowerLowVolCount = 0;
	g_SystemVar.PowerNormalVolCount = 0;
	if(p_AC == AC_JACK_ON)
	{
		g_SystemVar.ACStatus = POWER_DC_IN;
		//SysTaskEnable(BAT_CHECK_TASK_ID, SYS_RUN_TASK_IMMEDIATELY);
	}
	else
	{
		g_SystemVar.ACStatus = POWER_BATTERY_IN;
	}
	SysTaskEnable(AC_DETECT_TASK_ID, SYS_RUN_TASK_IMMEDIATELY);	
}

TaskTimeout_t PowerACDetect(void)
{
	static uchar ACJackStage=AC_JACK_CAPTURE;
	TaskTimeout_t TimeToRun;
	bit ACStatus;

	TimeToRun = SYS_RUN_TASK_IMMEDIATELY;
	//p_AC = 1;
	//_nop_();
	//_nop_();
	//_nop_();
	//_nop_();
	ACStatus = p_AC;
	if(g_SystemVar.ACStatus == POWER_DC_IN)
	{
		if(ACStatus == AC_JACK_OFF)
		{
			if(ACJackStage == AC_JACK_CAPTURE)
			{
				TimeToRun = AC_JACK_DEBOUNCE_PRESS;
				ACJackStage = AC_JACK_DEBOUNCE;
			}
			else //if(ScanStage == KEY_CAPTURE)
			{
				//SysPrintf( "\r\nAC lost.");
				g_SystemVar.ACStatus = POWER_BATTERY_IN;
				HostMsgPost(SYS_MSG_AC_SWITCH, g_SystemVar.ACStatus);
				ACJackStage = AC_JACK_CAPTURE;
			}
		}
	}
	else
	{
		if(ACStatus == AC_JACK_ON)
		{
			if(ACJackStage == AC_JACK_CAPTURE)
			{
				TimeToRun = AC_JACK_DEBOUNCE_RELEASE;
				ACJackStage = AC_JACK_DEBOUNCE;
			}
			else //if(ScanStage == KEY_DEBOUNCE)
			{
				//SysPrintf( "\r\nAC restore.");
				g_SystemVar.ACStatus = POWER_DC_IN;
				HostMsgPost(SYS_MSG_AC_SWITCH, g_SystemVar.ACStatus);
				ACJackStage = AC_JACK_CAPTURE;
			}
		}
	}
	
	return TimeToRun;
}

void PowerAcSwitchHandle(void)
{
#if 1
	if(g_SystemVar.ACStatus == POWER_DC_IN)
	{
		p_WIFI_EN = LEVEL_HIGH;
		g_WifiVar.cResetTimeOut = 2;
		SysPrintf( "\r\nOPEN WIFI");
		SysTaskEnable(BAT_CHECK_TASK_ID, TASK_DISABLE);
	}
	else if(g_SystemVar.ACStatus == POWER_BATTERY_IN)
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
		LedBlinkSet(LED_WIFI_INDICATOR, LED_FAST_BLINK);
		p_WIFI_EN = LEVEL_LOW;
		SysPrintf( "\r\nclose WIFI");
		//SysTaskEnable(BAT_CHECK_TASK_ID, SYS_RUN_TASK_IMMEDIATELY);
	}
	
#else
	unsigned int Param;
	
	if(pMsg->Param == POWER_DC_IN)
	{
		Param = (N_HOST<<12)|(CASE_AC_RECOVER<<8)|ZONE_AC;
	}
	else if(pMsg->Param == POWER_BATTERY_IN)
	{
		Param = (N_HOST<<12)|(CASE_ACLOST<<8)|ZONE_AC;
	}
	SaveAlarmLog(N_HOST, CASE_ACLOST, ZONE_AC);
	g_SystemVar.AutoUploadTime = g_SystemParam.AutoUploadTime;
	WifiMsgPost(WIFI_MSG_EVENT_TO_PLATFORM, Param);
#endif	
}

unsigned int GetBatAdcVal(void)
{
	unsigned int ad_result;
	
	ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_BAT_IN | ADC_START;
	_nop_();                        //Must wait before inquiry
	_nop_();
	_nop_();
	_nop_();	
	_nop_();                       
	_nop_();
	_nop_();
	_nop_();
	while(!(ADC_CONTR & ADC_FLAG));////Wait complete flag
	ADC_CONTR &= ~ADC_FLAG;         //Close ADC
	ad_result = (ADC_RES<<2)+ADC_RESL;

	return ad_result;
}

TaskTimeout_t BatteryVoltageCheck(void)
{
	unsigned int BatVal;
	TaskTimeout_t TimeToRun;

	TimeToRun = INQUIEY_INTERVAL_TIME;
	
	BatVal = GetBatAdcVal();
	SysPrintf( "\r\nBatAdcVal: %xi", BatVal);
	if(BatVal < ADC_BAT_3_2V)
	{
		if(g_SystemVar.PowerStatus == BATTERY_LOW)
		{			
			g_SystemVar.PowerNormalVolCount = 0;
			SysPrintf( "\r\nLow battery");
		}
		else
		{
			if(g_SystemVar.PowerLowVolCount == HANDLE_LOW_VOL_COUNT)
			{

				g_SystemVar.PowerStatus = BATTERY_LOW;					
				HostMsgPost(SYS_MSG_POWER_OFF, NULL);
			}
			else
			{
				g_SystemVar.PowerLowVolCount++;
			}	
		}
	}
	else
	{
		if(g_SystemVar.PowerStatus == BATTERY_NORMAL)
		{			
			g_SystemVar.PowerLowVolCount = 0;
		}
		else
		{
			if(g_SystemVar.PowerNormalVolCount== HANDLE_NORMAL_VOL_COUNT)
			{
				g_SystemVar.PowerStatus = BATTERY_NORMAL;
			}
			else
			{
				g_SystemVar.PowerNormalVolCount++;
			}	
		}
	}

	return TimeToRun;
}


