#ifndef _TONE_H
#define _TONE_H

#define MAX_BEEP_SAME_FREQUENCE_TIME							450
#define MAX_SIREN_SAME_FREQUENCE_TIME							26
#define SIREN_FREQUENCE_TABLE_LENGTH							15
#define FOSC1													300/12
#define BEEP_SHORT_TIME											250
#define BEEP_LONG_TIME											500
#define MAX_SIREN_RING_TIME										9 //9 MIN

#define PLAY_DOOR_BELL_TIME										5 
#define PLAY_STARTUP_TONE_TIME									3 

enum{
	LM386_ON=0,
	LM386_OFF,	
};

enum{
	BELL_TONE=1,
	STARTUP_TONE,	
};

enum{
	BEEP_NORMAL=0,
	BEEP_TWO,
	BEEP_CONFIRM,
};

enum
{
	SIREN_MUTE=0,
	SIREN_ENABLE,
};

enum
{
	PWM_SIREN=0,
	PWM_BEEP,
};

enum
{
	SIREN_OFF=0,
	SIREN_ON,
};

enum
{
	BEEP_IDLE=0,
	BEEP_PLAY,
	BEEP_STOP,
};

typedef struct{
	unsigned char State;
	unsigned char Mode;
	unsigned char OnTime;
	
}Beep_Var_t;

extern VoidFun PWMIsr;
void LM386Set(unsigned char OnOrOff);
void ToneRoutineHandle(void);
void ToneInit(void);
void PWMOn(void);
void PWMOff(void);
void BeepTone(unsigned char BeepType);
void OpenSiren(void);
void CloseSiren(void);
void ClosePWM(void);
void BeepPlay(void);
void BeepIsr(void);
void SirenIsr(void);
void PlayPromptTone(MSG_t *pMsg);
void PlayDoorBellTimeOut(void);
TaskTimeout_t BeepTask(void);
#endif

