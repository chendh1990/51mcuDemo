#ifndef __HW_H
#define __HW_H

/////////CPU NAME/////////////////////////
//#define STC15F2K48S2
//#define STC15L2K48S2
//#define STC15L2K60S2
#define IAP15L2K61S2

/////////////cpu ID config////////////////////////////////////////////
#if defined(STC15F2K48S2) || defined(STC15L2K48S2)
#define CPU_ID_ADD  0XBFF9//48s2
#elif defined(STC15F2K60S2)
#define CPU_ID_ADD  0XEFF9//60s2
#elif defined(IAP15L2K61S2)
#define CPU_ID_ADD  0Xf3f9//61s2
#endif

#define CPU_ID_ADD_START	CPU_ID_ADD//最后的7个单元是 CPU ID
#define CPU_ID_ADD_END 		CPU_ID_ADD+6//最后的7个单元是 CPU ID

///////////pin assigment///////////////////////////////////////
sbit p_386_EN			=   P2^3;
sbit p_RES_080			=   P2^1;
sbit p_DAT_080			=   P2^0;
sbit p_Busy_080			=   P4^4;
sbit p_PWM	       		=   P2^5;
sbit p_Beep	       		=   P2^4;
sbit p_L2				=   P3^4;
sbit p_L1				=   P3^6;
sbit p_Lock				=   P3^2;
sbit p_Bat_AD			=   P1^5;
sbit p_Tamper_Key		=   P2^2;
sbit p_RF_OUT			=   P1^4;
sbit p_RF_IN				=   P3^7;
sbit p_DATA2				=   P1^1;
sbit p_DATA1				=   P1^2;
sbit p_OUT_FLAG			=   P0^7;
sbit p_RELAY				=   P0^2;
sbit p_SP				=   P4^1;
sbit p_AC				=   P1^6;
sbit p_RFID_IN			=   P3^5;
sbit p_RFID_OUT			=   P1^0;
sbit p_MouldePowerKey	=   P4^3;
sbit p_MouldeDTR		=   P4^2;
sbit p_WTV_EN			=   P2^6;

sbit p_LED1				=   P0^1;
sbit p_LED2				=   P4^5;
sbit p_LED3				=   P0^0;
sbit p_LED5				=   P2^7;

sbit p_LED6				=   P0^3;
sbit p_LED7				=   P0^4;
sbit p_LED8				=   P0^5;
sbit p_LED9				=   P0^6;
///////////////////////mv3/////////////////////////////
sbit P_KEYDAT0			= P1^7;
sbit P_KEYDAT1			= P1^2;
sbit P_KEYDAT2			= P1^1;
sbit P_KEYOUTF			= P0^7;

#endif 
