#include "sys.h"
#include "SysClk.h"
#include "eeprom.h"

#include "HistoryLog.h"

void SaveAlarmLog(unsigned char Name, unsigned char Case, unsigned char Zone)
{
	unsigned long *pDate;
	unsigned long Date;
	
	if(g_SystemParam.HistoryLogNum > MAX_HISTORY_LOG_NUM)
		return;
	
	memset(g_HostCommonBuf, 0, HOST_COMMON_BUF_SIZE);
	EE_Read_Data(EE_HISTORY_LOG_START_ADDRESS, g_HostCommonBuf, HISTORY_LOG_LENGTH*g_SystemParam.HistoryLogNum);
	pDate = (unsigned long *)(g_HostCommonBuf+HISTORY_LOG_LENGTH*g_SystemParam.HistoryLogNum);
	Date = (g_RTC.Year&0x1f);
	*pDate = (Date<<27);
	Date = (g_RTC.Month&0x0f);
	*pDate |= (Date<<23);
	Date = (g_RTC.Day&0x1f);
	*pDate |= (Date<<18);
	Date = (g_RTC.Hour&0x1f);
	*pDate |= (Date<<13);
	Date = (g_RTC.Minute&0x7f);
	*pDate |= (Date<<6);
	Date = (g_RTC.Second&0x3f);
	*pDate |= Date;
	*(g_HostCommonBuf+HISTORY_LOG_LENGTH*g_SystemParam.HistoryLogNum + 4) = (Name<<4)|Case;
	*(g_HostCommonBuf+HISTORY_LOG_LENGTH*g_SystemParam.HistoryLogNum + 5) = Zone;	
	
	if(g_SystemParam.HistoryLogNum < MAX_HISTORY_LOG_NUM)
	{
		g_SystemParam.HistoryLogNum++;
		EE_WriteSector(EE_HISTORY_LOG_START_ADDRESS, HISTORY_LOG_LENGTH*g_SystemParam.HistoryLogNum, g_HostCommonBuf);
	}
	else
	{
		EE_WriteSector(EE_HISTORY_LOG_START_ADDRESS, HISTORY_LOG_LENGTH*g_SystemParam.HistoryLogNum, g_HostCommonBuf+HISTORY_LOG_LENGTH);
	}

	SysParamToEeprom();
}

