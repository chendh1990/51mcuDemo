#ifndef __POWER_H
#define __POWER_H

#define ADC_POWER   							0x80            //ADC power control bit
#define ADC_FLAG    							0x10            //ADC complete flag
#define ADC_START   							0x08            //ADC start control bit
#define ADC_SPEEDLL 							0x00            //420 clocks
#define ADC_SPEEDL  							0x20            //280 clocks
#define ADC_SPEEDH  							0x40            //140 clocks
#define ADC_SPEEDHH 							0x60            //70 clocks

#define ADRJ 									0x40            
#define ADC_BAT_IN 								0X03           
#define P13_ASF 								0X08	
#define ADC_BAT_3_2V 							0x7f          
#define AC_JACK_DEBOUNCE_PRESS					3
#define AC_JACK_DEBOUNCE_RELEASE				3
#define INQUIEY_INTERVAL_TIME 					30 //  3s
#define HANDLE_LOW_VOL_COUNT 					10
#define HANDLE_NORMAL_VOL_COUNT 				10

typedef enum{
	AC_JACK_ON=0,
	AC_JACK_OFF	
}AC_Jack_Status_t;

typedef enum{
	AC_JACK_CAPTURE=0,
	AC_JACK_DEBOUNCE	
}AC_Jack_Stage_t;

enum
{
	BATTERY_LOW=0,
	BATTERY_NORMAL,	
};

enum
{
	POWER_BATTERY_IN=0,
	POWER_DC_IN,	
};

void PowerInit(void);
TaskTimeout_t PowerACDetect(void);
void PowerAcSwitchHandle(void);
TaskTimeout_t BatteryVoltageCheck(void);
#endif

