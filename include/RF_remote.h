#ifndef __RF_REMOTE_H
#define __RF_REMOTE_H

#define RF_CODE_LENGTH											3
#define RF_CODE_STORE_LENGHT									8
#define RF_BUFFER_LENGTH     									3// 3BYTE
#define RF_CODE_BIT_LENGTH     									24 //24BIT
#define RF_CODE_FIRST_BYTE     									8
#define RF_CODE_SECOND_BYTE     								16
#define MAX_STORE_REMOTE_NUM     								10
#define MAX_STORE_WIRELESS_ACCESSORY_NUM     					50 
#define MAX_STORE_WIRELESS_DOOR_BELL_NUM     					10
#define RF_CONTROL_ENABLE										0X01
#define RF_CONTROL_DISABLE										0X00
//////////////////////////////////////////////////////////////////////////////
#define MAX_RF_UNIT_IN_SECTOR									22 // 512/23 := 22



///////////////////////////////////////////////////////////////////////////////////
#define MAX_FIRST_BLOCK_ZONE_NAME_NUM     					16
#define MAX_SECOND_BLOCK_ZONE_NAME_NUM     					32
#define MAX_THIRD_BLOCK_ZONE_NAME_NUM     					48

#define RF_NAME_LENGHT										32
#define RF_NAME_UNICODE_LENGHT								16

#define RF_STORE_ADDRESS_MAP_NULL							0xFF
#define RF_STORE_ADDRESS_NODE_SIZE							4
#define MAX_RF_STORE_ADDRESS_NODE_TOTAL						128


//////////////////////////////////RF SEND/////////////////////////////////////
#define RF_SEND_HIGH_LEVEL_LENGTH	     						500 //500us
#define RF_SEND_ONE_FRAME_CODE_LENGHT   						25
#define RF_SEND_ONE_CODE_LENGHT   								25 // head (1T) +data(3bytes*8=24T)

#define RF_SEND_HEAD_HIGH_LENGHT   								1
#define RF_SEND_HEAD_LOW_LENGHT   								31
#define RF_SEND_BIT0_HIGH_LENGHT   								1
#define RF_SEND_BIT0_LOW_LENGHT   								3
#define RF_SEND_BIT1_HIGH_LENGHT   								3
#define RF_SEND_BIT1_LOW_LENGHT   								1

#define RF_SEND_BYTE_LENGTH   									8
//////////////////////////////////////////////////////////////////////////////
#define MAX_REGISTER_TIME 										15 // 3s
#define MAX_HANDLE_REMOTE_CODE_TIME 							3 // 3s
#define RF_DECODE_INTERVAL_TIME 								120 //80 //5MS*80=400MS

/////////////////////////////////low level lenght/////////////////////////////
// 500us*31  
#define HEAD_MAX     											54250//700US	46500//600US*31//40000 //16MS
#define HEAD_MIN         										23250//300US*3131000//400us	23250//300US*31//37500 //15MS
//500US     
#define	L_MAX													1750//700US	1500//600US
#define L_MIN													750//300US1000//400us	750//300US//1000
// 500us*3
#define H_MAX													5250//700US		4500//600US*3//4000
#define H_MIN           										2250//300US*33000//400us	2250//300US*3//3500
///////////////////////////RF COMMAND SETS///////////////////////////////
#define RF_COMMAND_ARMED     									8
#define RF_COMMAND_DISARMED     								1
#define RF_COMMAND_HOME_ARMED     								4
#define RF_COMMAND_SOS_ALARM     								2 
#define RF_COMMAND_DOOR_BELL     								5 
//////////////////////////////////////////////////////////////////////////
//								 D3  D2  D1  D0
//RF_COMMAND_24_HOUR_ZONE        1   0   1   1      
//RF_COMMAND_DELAY_ZONE	 	 	 1   0   1   0
//RF_COMMAND_NORMAL_ZONE         0   1   1   1
//RF_COMMAND_AT_HOME_ZONE        0   1   1   0
/////////////////////////////////////////////////////////////////////////////
#define RF_COMMAND_24_HOUR_ZONE			     					11
#define RF_COMMAND_DELAY_ZONE			     					10
#define RF_COMMAND_NORMAL_ZONE		     						7
#define RF_COMMAND_NORMAL_ZONE_1		     					15
#define RF_COMMAND_AT_HOME_ZONE		     						6
#define RF_COMMAND_AT_HOME_ZONE_1		     					3

#define RF_COMMAND_ACCESSORY_LOW_VOLTAGE     					13
#define RF_COMMAND_ACCESSORY_TAMPER	     						12
//////////////////////////////////////////////////////////////////////
#define RF_REGISTER_TIMEOUT 									30


#define RF_MATCH_FAIL											0

enum
{
	RF_REGISTER_FAIL = 0,
	RF_REGISTER_SUCCESS,
};
enum
{
	RF_RC = 0,
	RF_ZONE,
	RF_DOOR_BELL,
	RF_UNDEFINE,
};

typedef enum
{
	RF_FROM_TRIGER = 0,
	RF_FROM_APP,
}RF_Regiseter_Mode_t;

typedef struct{
	unsigned char BitCounter;
	unsigned char Address1;
	unsigned char Address2;
	unsigned char Data;
	unsigned char Decodestart;
	unsigned char DecodeControl;
	unsigned char DecodeCheck;
	unsigned char MaxHandleTime;
	unsigned char RegisterTimeOut;	
	unsigned char DecodeInterval;
	unsigned char CodeBuffer[RF_BUFFER_LENGTH];
} RF_Receive_t;

typedef struct{
	unsigned char RFCode[RF_CODE_LENGTH];
	unsigned char Mode;
	unsigned char NameFlag;
	unsigned char CodeLocate;
	unsigned char Type;
	unsigned char Control;
} RF_Store_t;

typedef struct{
	unsigned int RfStoreAdr;
	unsigned char Last;
	unsigned char Next;
} RF_Adr_Node_t;

extern xdata RF_Receive_t g_RFDecode;
extern xdata RF_Store_t g_RFStore;
extern xdata RF_Adr_Node_t g_RfAdrNode;

void RFEnable(void);
void RFDisable(void);
void RFInit(void);
void RFRegisterHandle(unsigned char *pBuf, RF_Regiseter_Mode_t RegisterMode);
unsigned char RfSearchMatchCode(unsigned char *pBuf, unsigned char RFType);
unsigned char GetRFtype(unsigned char OperateCode);
unsigned int GetZoneNameAdr(unsigned char index);
void SystemRfRegisterStatus(void);
void RFReceiveHandler(void);
void RFRoutineHandle(void);
void RFTickHandle(void);
void RFAllRemove(void);
unsigned char RfSearchValidStoreLocate(unsigned char RFType);
//////////////////////////////////////////////////////////////
void AddRfAdrItem(unsigned int RfStoreAdr);
void DeleteRfAdrItem(unsigned int RfStoreAdr);
#endif

