#include "sys.h"
#include "uart.h"
#include "RF_remote.h"
#include "SysClk.h"

xdata Real_Time_t g_RTC;
/////////////////SYSTEM TICK SETTING//////////////////////////
void SysClkInit(void)
{
	TMOD=0X11;
	TR1 = 1;
	ET1 = 1;
////////initial rtc//////////
	g_RTC.Year = RTC_DEFAULT_YEAR;
	g_RTC.Month = RTC_DEFAULT_MONTH;
	g_RTC.Day = RTC_DEFAULT_DAY;
	g_RTC.Hour= RTC_DEFAULT_HOUR;
	g_RTC.Minute = RTC_DEFAULT_MINUTE;
	g_RTC.Second = RTC_DEFAULT_DAY;
	g_RTC.TickCount = 0;
}

void RtcTick(void)
{
	if(++g_RTC.TickCount == OS_TICKS_PER_SEC)
	{
		g_RTC.TickCount = 0;
		if(++g_RTC.Second == 60)
		{
			g_RTC.Second = 0;
			HostMsgPost(SYS_MSG_MINUTE_UPDATE, NULL);
			if(++g_RTC.Minute == 60)
			{
				g_RTC.Minute = 0;
				if(++g_RTC.Hour == 24)
				{
					g_RTC.Hour = 0;
					if(++g_RTC.Month == 13)
					{
						g_RTC.Month = 1;
						if(++g_RTC.Year == 100)
						{
							g_RTC.Year = 0;
						}
					}
				}
			}
		}
	}
}

void UserTickTimer(void)
{
	TL1 = 58;
	TH1 = 0X9E;
////UPDATE RTC///////////////
	RtcTick();
//////UART TXD&RXD CHECK/////
	UartTickCheck(UART_PC);
	UartTickCheck(UART_WIFI);
////////////////////////////////
	RFTickHandle();
}


