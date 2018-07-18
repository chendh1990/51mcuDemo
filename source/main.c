///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// ����˵������
// ��Ʒ����  ��	DOOR BELL
// ��Ƭ���ͺţ�	STC15L2K48S2
// ʱ��ѡ��  ��	30 MHZ
// ��      �ڣ�	2015-06-29
// ��      ˾��	��Ƽ�
////////////////////////////////////////////////////////////////////////////////

#include "sys.h"
#include "key.h"
#include "uart.h"
#include "tone.h"
#include "WIFI.h"

extern void SysClkInit(void);
extern void PCAInit(void);
extern void RFInit(void);

void main(void)
{
	SysClkInit();
	OSStart();
}

void TaskA(void)
{
	OSSemCreate(SEM_PRINTF, 1);
	OSSemCreate(SEM_STC_FLASH, 1);  
	SysInit();
	SysPrintf("\r\nstart on TaskA!");
	HostMsgPost( SYS_MSG_INIT_CURRENT_STATUS, NULL);
	while(1)
	{
		SystemStatusMachine(g_SystemVar.SystemNextStatus);
	}
}

void TaskB(void)
{
	SysPrintf("\r\nstart on TaskB!");
	while(1)
	{	
		SysRunTask();
		OSWait(K_TMO,OS_TICKS_PER_SEC/10);
	}
}

void TaskC(void)
{
	SysPrintf("\r\nstart on TaskC!");
	while(1)
	{
		OSWait(K_TMO,OS_TICKS_PER_SEC/5);
	}
}

void TaskD(void)
{
	SysPrintf("\r\nstart on TaskD!");
	while(1)
	{
		OSWait(K_TMO,OS_TICKS_PER_SEC/2);
	}
}

void TaskE(void)
{
	SysPrintf("\r\nstart on TaskE!");
	while(1)
	{
		OSWait(K_TMO,OS_TICKS_PER_SEC/2);
	}
}

