#include "sys.h"
#include "timer.h"
#include "uart.h"
#include "PCA.h"
#include "tone.h"
#include "RF_remote.h"
#include "led.h"

VoidFun PWMIsr;

xdata unsigned char PWMFrequenceIndex;
xdata unsigned char PWMRepeatTime;
xdata unsigned char BeepToneType;
xdata unsigned int PWMTime;
unsigned int PCC_Value;
xdata Beep_Var_t g_BeepTone;

code unsigned int  SirenFrequenceTable[SIREN_FREQUENCE_TABLE_LENGTH]=
{	
	(FOSC1*13), 
	(FOSC1*18),
	(FOSC1*26), 
	(FOSC1*32),
	(FOSC1*38), 
	(FOSC1*44),
	(FOSC1*50), 
	(FOSC1*56),
	(FOSC1*62), 
	(FOSC1*69),
	(FOSC1*76), 
	(FOSC1*82),
	(FOSC1*88), 
	(FOSC1*94),
	(FOSC1*100),						
};

void LM386Set(unsigned char OnOrOff)
{
	if(OnOrOff == LM386_OFF)
	{
		p_8002_EN = LM386_OFF;
	}
	else
	{
		p_8002_EN = LM386_ON;
	}
}

void SirenIsr(void)
{
	CCAP0L = PCC_Value;
	CCAP0H = PCC_Value >> 8;          
	PCC_Value += PWMTime;  
	p_PWM = ~p_PWM;
	PWMRepeatTime++;
	if(PWMRepeatTime > MAX_SIREN_SAME_FREQUENCE_TIME)
	{
		PWMRepeatTime = 0;	
  		PWMFrequenceIndex--;
  		PWMTime = SirenFrequenceTable[PWMFrequenceIndex];
		
		if(PWMFrequenceIndex == 0)
		{
			PWMFrequenceIndex = SIREN_FREQUENCE_TABLE_LENGTH;
		}
	}

}

void PWMOn(void)
{
	g_SystemVar.PWMFlag = TRUE;
	g_BeepTone.State = BEEP_IDLE;
	CCAPM0 = 0;
	p_PWM = 1;
	LM386Set(LM386_ON);
	PWMRepeatTime = 0;
	PWMFrequenceIndex = SIREN_FREQUENCE_TABLE_LENGTH - 1; 
	PCC_Value = SirenFrequenceTable[PWMFrequenceIndex];
	PWMTime = SirenFrequenceTable[PWMFrequenceIndex];
	PWMIsr = SirenIsr;
	CMOD = 0;  
	CCAP0L = PCC_Value;
	CCAP0H = PCC_Value >> 8;  
	CCAPM0 = 0x49;        
	CCON = 0x40; 
}

void PWMOff(void)
{
	g_SystemVar.PWMFlag = FALSE;
	CCAPM0 = 0;
	p_PWM = 0;
	LM386Set(LM386_OFF);
}

void ToneRoutineHandle(void)
{
	if(g_SystemVar.PlayStartupToneCount > 0)
	{
		if(--g_SystemVar.PlayStartupToneCount == 0)
		{
			HostMsgPost(SYS_MSG_PLAY_PROMPT_TONE, STARTUP_TONE);
		}
	}
		
	if(g_SystemVar.PlayDoorTimeout > 0)
	{
		if(--g_SystemVar.PlayDoorTimeout == 0)
		{
			HostMsgPost(SYS_MSG_PLAY_DOOR_BELL_TIMEOUT, NULL);
		}
	}

	if(g_SystemVar.SirenLastingTime > 0)
	{
		if(--g_SystemVar.SirenLastingTime == 0)
		{
			HostMsgPost(SYS_MSG_SIREN_TIME_OUT, NULL);
		}
	}
}

void BeepInint(void)
{
	g_BeepTone.State = BEEP_IDLE;

	SysTaskEnable(BEEPER_TASK_ID, SYS_RUN_TASK_IMMEDIATELY);
}

void BeepTone(unsigned char BeepType)
{
	if((g_SystemVar.PWMFlag == TRUE) 
		||(g_BeepTone.State != BEEP_IDLE)
		||(g_SystemVar.PlayDoorBell == TRUE))
		return;
	SysPrintf( "\r\nBeepTone");
	g_BeepTone.State = BEEP_PLAY;
	g_BeepTone.Mode = BeepType;
	g_BeepTone.OnTime = 1;
	switch(BeepType)
	{
		case BEEP_NORMAL:
			g_BeepTone.OnTime = 1;
			break;

		case BEEP_TWO:
			g_BeepTone.OnTime = 2;
			break;	

		case BEEP_CONFIRM:
			g_BeepTone.OnTime = 2;
			break;	
	}
}

void BeepPlay(void)
{
	LM386Set(LM386_ON);
	PWMIsr = BeepIsr;
	PCC_Value = 625;  // 4khz
	CCAP0L = PCC_Value;
	CCAP0H = PCC_Value >> 8;
	CCAPM0 = 0x49;
}

void BeepStop(void)
{
	PWMOff();
}

TaskTimeout_t BeepTask(void)
{
	TaskTimeout_t cTimeToRun;
	
	cTimeToRun = SYS_RUN_TASK_IMMEDIATELY;
	switch(g_BeepTone.State)
	{
		case BEEP_PLAY:
			BeepPlay();
			cTimeToRun = 1;//SYS_RUN_TASK_IMMEDIATELY;
			if(g_BeepTone.Mode == BEEP_CONFIRM)
			{
				if(g_BeepTone.OnTime == 1)
				{
					cTimeToRun = 3;
				}
			}
			g_BeepTone.State = BEEP_STOP;
			break;
			
		case BEEP_STOP:
			BeepStop();
			g_BeepTone.OnTime--;
			g_BeepTone.State = g_BeepTone.OnTime==0 ? BEEP_IDLE : BEEP_PLAY;
			break;
	}
	
	return cTimeToRun;
}

void ToneInit(void)
{
#if defined(HW_T0_2)
//////p_8002_EN port set////////////////
	P2M0 |= 0x10; 
	P2M1 &= 0xEF;
//////p_PWM port set////////////////
	P2M0 |= 0x08; 
	P2M1 &= 0xF7;		
//////p_WT port set////////////////
	P2M0 |= 0x03; 
	P2M1 &= 0xFC;		
	p_WT_DATA = 0;
	p_WT_RESET = 0;
//////////////////////////
#else
//////p_8002_EN port set////////////////
	P2M0 |= 0x10; 
	P2M1 &= 0xEF;
//////p_PWM port set////////////////
	P2M0 |= 0x08; 
	P2M1 &= 0xF7;		
//////p_WT port set////////////////
	P3M0 |= 0x08; 
	P3M1 &= 0xF7;		
	p_WT_DATA = 0;
	p_WT_RESET = 0;
//////////////////////////
#endif
	LM386Set(LM386_OFF);
	BeepInint();
}

void BeepIsr(void)
{
	CCAP0L = PCC_Value;
	CCAP0H = PCC_Value >> 8;          
	PCC_Value += 625;  // 4khz
	p_PWM = ~p_PWM;
}

void OpenSiren(void)
{
	if(g_SystemParam.SirenLastingTime == 0)
		g_SystemVar.SirenLastingTime = 60;
	else
		g_SystemVar.SirenLastingTime = g_SystemParam.SirenLastingTime*60;
#ifdef MAX_SIREN_3_MINUTE
	if(g_SystemVar.SirenLastingTime > 180)// 3minute=3*60=180
	{
		g_SystemVar.SirenLastingTime = 180;
	}
#endif
	if((g_SystemParam.SirenMuteFlag==SIREN_MUTE)||(g_SystemParam.SirenLastingTime==0))
	{
		return;
	}
	
	SysPrintf( "\r\nOpenSiren");
	g_SystemVar.SirenStatus = SIREN_ON;
	PWMOn();
}



void CloseSiren(void)
{
	SysPrintf( "\r\nCloseSiren");
	p_PWM = 0;
	LedCloseZoneBlink();
	LedRefresh();
	g_SystemVar.SirenStatus = SIREN_OFF;
	g_SystemVar.SirenResuceCount = 0;
	g_SystemVar.SirenLastingTime = 0;
	PWMOff();
}

void PlayPromptTone(MSG_t *pMsg)
{
	unsigned char index;
	unsigned char PromptID;
	
	if(g_SystemVar.PWMFlag == TRUE)
	{
		return;
	}	
	g_SystemVar.PlayDoorBell = TRUE;
	g_BeepTone.State = BEEP_IDLE;
	PromptID = (unsigned char)pMsg->Param;
	//SysPrintf("\r\nPromptID = %d", PromptID);
	if(PromptID == BELL_TONE)
	{
		g_SystemVar.PlayDoorTimeout = PLAY_DOOR_BELL_TIME;
	}
	else if(PromptID == STARTUP_TONE)
	{
		g_SystemVar.PlayDoorTimeout = PLAY_STARTUP_TONE_TIME;
	}
	p_WT_DATA = 0;
	p_WT_RESET= 1;
	OSWait(K_TMO,OS_TICKS_PER_SEC/50);
	p_WT_RESET= 0;
	OSWait(K_TMO,OS_TICKS_PER_SEC/50);
	for(index=0; index<PromptID; index++)
	{
		p_WT_DATA = 1;
		OSWait(K_TMO,OS_TICKS_PER_SEC/50);
		p_WT_DATA = 0;
		OSWait(K_TMO,OS_TICKS_PER_SEC/50);
	}
	LM386Set(LM386_ON);
}

void PlayDoorBellTimeOut(void)
{	
	SysPrintf( "\r\nPlayDoorBellTimeOut");
	p_WT_DATA = 0;
	p_WT_RESET = 0;
	g_SystemVar.PlayDoorBell = FALSE;
	g_SystemVar.PlayDoorTimeout = 0;

	if(g_SystemVar.PWMFlag == FALSE)
	{
		LM386Set(LM386_OFF);
	}	
}

///////////////////////////////////////////////////////////////////

