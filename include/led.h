#ifndef __LED_H
#define __LED_H

#define LED_REFRESH_TIME	  					2 // 5
#define LED_BLINK_REFRESH_TIME	  				5 // 500MS
#define LED_ALL_NO_BLINK	  					0x00
#define LED_ALL_OFF	  							0xFF
#define BLINK_LED_ON_TIME	  					5 // 500MS
#define BLINK_LED_OFF_FAST_TIME	  				5 // 500MS
#define BLINK_LED_OFF_SLOW_TIME	  				20 // 2S

#if defined(HW_T0_2)
#define LED_SOS_MASK	  						0x01
#define LED_SET_MASK	  						0x02
#define LED_WIFI_MASK	  						0x04
typedef enum//led nummber
{
	LED_SOS_INDICATOR=0,
	LED_NORMAL_ZONE_INDICATOR,
	LED_24H_ZONE_INDICATOR,
	LED_HOME_ZONE_INDICATOR,
	LED_DOOR_BELL_INDICATOR,
	LED_WIFI_INDICATOR,
////////////////////////////////////	
	LED_TOTAL_NUMBER,
	LED_ALL = 0xff,
} Led_Index_t;

#else
#define LED_SOS_MASK	  						0x01
#define LED_SET_MASK	  						0x02
#define LED_WIFI_MASK	  						0x04
typedef enum//led nummber
{
	LED_SOS_INDICATOR=0,
	LED_SET_INDICATOR,
	LED_WIFI_INDICATOR,
////////////////////////////////////	
	LED_TOTAL_NUMBER,
	LED_ALL = 0xff,
} Led_Index_t;
#endif

typedef enum//led status
{
	LED_ON=0,
	LED_OFF,
}Led_Status_t;

typedef enum//led blink status
{
	BLINK_ON=0,
	BLINK_OFF,
}Led_Blink_Status_t;

typedef enum//led blink mode
{
	LED_NO_BLINK=0,
	LED_FAST_BLINK,	
	LED_SLOW_BLINK,
}Led_Blink_Mode_t;

typedef struct{	
//////////////////LED STATUS///////////////////////	
	unsigned char SOSStatus				:1;	
	unsigned char SetStatus				:1;	
	unsigned char WifiStatus			:1;	
//////////////////LED BLINK STATUS///////////////////////		
	unsigned char SOSBlinkStatus		:1;
	unsigned char SetBlinkStatus		:1;
	unsigned char WifiBlinkStatus		:1;
//////////////////LED BLINK MODE///////////////////////		
	unsigned char SOSBlinkMode			:2;
	unsigned char SetBlinkMode			:2;
	unsigned char WifiBlinkMode			:2;

	unsigned char BlinkSynchroton		:4;

} LED_Var_t;

#define LED_SYS_STATUS_INDICATOR				LED_SET_INDICATOR
void LedInit(void);
void LedIndicator(Led_Index_t LedIndex, Led_Status_t OnOrOff);
void LedSysStatusIndicate(void);
void LedBlinkSet(Led_Index_t LedIndex, Led_Blink_Mode_t LedBlinkMode);
void LedCloseZoneBlink(void);
void LedAllOff(void);
void LedAlarmIndicate(MSG_t *pMsg);
void LedRefresh(void);
TaskTimeout_t LedTask(void);
#endif

