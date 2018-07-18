#ifndef __WIRE_H
#define __WIRE_H

enum
{
	WIRE_LINE_1=0,
	WIRE_LINE_2,
};

enum
{
	WIRE_NULL=0,
	WIRE_INPUT,
};

void Wire_Speaker_Enable(void);
void Wire_Speaker_Disable(void);
void Wire_Lock_Open(void);
void Wire_Lock_Close(void);
void Wire_Init(void);
unsigned char Wrie_Get_Val(unsigned char line_num);
void Wire_Input_Capture(void);
void Wire_L2_Time_Out(void);
void Wire_L1_Time_Out(void);
#endif
