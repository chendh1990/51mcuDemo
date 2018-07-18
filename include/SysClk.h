#ifndef __SYS_CLK_H
#define __SYS_CLK_H

#define RTC_DEFAULT_YEAR					15
#define RTC_DEFAULT_MONTH					01
#define RTC_DEFAULT_DAY						01
#define RTC_DEFAULT_HOUR					01
#define RTC_DEFAULT_MINUTE					01
#define RTC_DEFAULT_SECOND					01

typedef struct
{
	unsigned char Year;
	unsigned char Month;
	unsigned char Day;
	unsigned char Hour;	
	unsigned char Minute;	
	unsigned char Second;
	unsigned char TickCount;	
} Real_Time_t;

extern xdata Real_Time_t g_RTC;
void SysClkInit(void);
void UserTickTimer(void);
#endif

