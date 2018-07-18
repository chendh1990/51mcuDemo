#ifndef __KEY_H
#define __KEY_H


#define KEY1_MASK 	    						0X01 
#define KEY2_MASK 	    						0X02 
#define KEY_SIREN_RESUCE_TIME 								3 // 3S
//////////////////////////////////////////////////////////////////////
#define KEY_SCAN_NO_TIMEOUT 								0
#define KEY_SCAN_LONG_PRESS_TIMEOUT 						40 // 7S 30 // 3s
#define KEY_SCAN_DEBOUNCE_TIMEOUT 							1
#define KEY_SCAN_DEBOUNCE_COUNT 							2

////////////////////////////////////////////////////////////////////////
enum
{
	KEY_DIGIT_0_VALUE=0,				
	KEY_DIGIT_1_VALUE,
	KEY_DIGIT_2_VALUE,
	KEY_DIGIT_3_VALUE,
	KEY_DIGIT_4_VALUE,					
	KEY_DIGIT_5_VALUE,
	KEY_DIGIT_6_VALUE,					
	KEY_DIGIT_7_VALUE,
	KEY_DIGIT_8_VALUE,					
	KEY_DIGIT_9_VALUE,
	KEY_ASTERISK_VALUE,					
	KEY_POUND_VALUE,
	KEY_DIAL_VALUE,	
	KEY_ARMED_VALUE,
	KEY_DISARMED_VALUE,
	KEY_HOME_VALUE,
};

enum
{
	KEY_PAD_IDLE_STATE = 0,
	KEY_PAD_SCAN_GPIO_STATE,	
	KEY_PAD_DEBOUNCE_STATE,
	KEY_PAD_WAIT_RELEASE_STATE,
	KEY_PAD_SCAN_END_STATE,
};

enum
{
	KEY_SETTING_ERROR=0,
	KEY_SETTING_SUCCESS,
	KEY_SETTING_CONTINUE,
};

enum
{
	KEY_NO_PREESED=0,
	KEY_SHORT_PREESED,
	KEY_LONG_PRESSED,
};

typedef enum{
	KEY_PRESS=0,
	KEY_RELEASE	
}Key_Status_t;

typedef enum{
	KEY_CAPTURE=0,
	KEY_DEBOUNCE	
}Scan_Stage_t;


/////////////////////////////////////////////////////
TaskTimeout_t ScanKey1(void);
TaskTimeout_t ScanKey2(void);
void KeyInit(void);
void KeyPadHandler(MSG_t *pMsg);
#endif

