#include "sys.h"
#include "timer.h"
#include "uart.h"
#include "WIFI.h"

xdata uchar g_cTimerTableIndex;
xdata TimerSever_t g_cTimerTable[TIMER_TABLE_MAX];
/////////////////SYSTEM TICK SETTING//////////////////////////
void TimerInit(void)
{
	g_cTimerTableIndex = 0;
	SysTaskEnable(SYS_TIMER_TASK_ID, SYS_RUN_TASK_IMMEDIATELY);
}

///////////////TIMER APPLICATION///////////////////////////////////////////////////
TaskTimeout_t SysTimerUnitTask(void)
{
	uchar cIndex;
	
	for(cIndex=0; cIndex<g_cTimerTableIndex; cIndex++)
	{	
		if(g_cTimerTable[cIndex].iTimeOut > 0)
		{
			g_cTimerTable[cIndex].iTimeOut--;
		}
		else
		{
			if(*g_cTimerTable[cIndex].pName == 'H')
			{
				HostMsgPost(g_cTimerTable[cIndex].Msg.msgID, g_cTimerTable[cIndex].Msg.Param);
			}	
			
			if(cIndex != (g_cTimerTableIndex-1))
			{
				g_cTimerTable[cIndex].Msg.msgID = g_cTimerTable[g_cTimerTableIndex-1].Msg.msgID;
				g_cTimerTable[cIndex].Msg.Param = g_cTimerTable[g_cTimerTableIndex-1].Msg.Param;
				g_cTimerTable[cIndex].pName = g_cTimerTable[g_cTimerTableIndex-1].pName;
			}

			if(g_cTimerTableIndex != 0)
			{
				g_cTimerTableIndex--;
			}
		}
	}

	return SYS_RUN_TASK_IMMEDIATELY;
}

void TimerUnitDel(unsigned char *pName)
{
	uchar cIndex;

	for(cIndex=0; cIndex<g_cTimerTableIndex; cIndex++)
	{	
		if((*(g_cTimerTable[cIndex].pName++)==*pName++)
			&&(*g_cTimerTable[cIndex].pName==*pName))
		{
			if(cIndex != (g_cTimerTableIndex-1))
			{
				g_cTimerTable[cIndex].pName = g_cTimerTable[g_cTimerTableIndex-1].pName;
				g_cTimerTable[cIndex].Msg.msgID = g_cTimerTable[g_cTimerTableIndex-1].Msg.msgID;
				g_cTimerTable[cIndex].Msg.Param = g_cTimerTable[g_cTimerTableIndex-1].Msg.Param;
			}

			if(g_cTimerTableIndex != 0)
			{
				g_cTimerTableIndex--;
			}
		}
	}
}

void TimerUnitAdd(unsigned char *pName, MSG_t *pMsg, TimeOutVal_t Timeout)
{
	//SysPrintf("\r\nTimerUnitAdd.");
	if(g_cTimerTableIndex >= TIMER_TABLE_MAX)
	{
		//SysPrintf("\r\nTimerUnitAdd1.");
		return;
	}
	else
	{
		g_cTimerTable[g_cTimerTableIndex].pName = pName;
		g_cTimerTable[g_cTimerTableIndex].Msg.msgID = pMsg->msgID;
		g_cTimerTable[g_cTimerTableIndex].Msg.Param= pMsg->Param;
		g_cTimerTable[g_cTimerTableIndex].iTimeOut = Timeout;
		SysPrintf("\r\ntimer pMsg->msgID = %xi\r\npMsg->Param = %xi", pMsg->msgID, pMsg->Param);
		g_cTimerTableIndex++;
	}	
}

void SetTimeout(unsigned char *pName, MSG_t Msg, TimeOutVal_t Timeout)
{
	TimerUnitDel(pName);
	TimerUnitAdd(pName, &Msg, Timeout);
}

