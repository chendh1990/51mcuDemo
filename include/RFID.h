#ifndef __RFID_H
#define __RFID_H

#define AUTO_LOCK_TIMEOUT 											15
#define RFID_DECODE_INTERVAL_TIME 									60 //5MS*100=500MS
#define RFID_IN_INTERVAL_TIME 										60 //5MS*100=500MS

#define RFID_BUFFER_LENGTH     										128
#define RFID_DECODE_LENGTH     										5
#define RFID_CODE_STORE_LENGHT     									6
#define MAX_REGISTER_RFID_NUM     									50

#define MAX_RFID_HANDLE_TIME     									5
#define MAX_RFID_RENAME_NUMBER     									4 
#define MATCH_RFID_FAIL     										0 
enum{
	WIRE_LOCK=0,
	WIRE_UNLOCK,
};

typedef struct{
	uint Level_Start;	
	uint Level_End;
	uchar Receive_Buffer[RFID_BUFFER_LENGTH];
	uchar Index;
	uchar Decode_Offset;
	uchar Decode_Data[RFID_DECODE_LENGTH];
	uchar Reversal_Flag;
	uchar DecodeControl;
	uchar MaxHandleTime;
	uchar DecodeInterval;
	uchar RFIDInFlag;	
} RFID_Receive_t;

extern RFID_Receive_t g_RFID_Var;

void RFID_Init(void);
uchar RFID_Register(void);
void RFID_Handle(void);
void WireLockSet(uchar LockStatus);
void RFIDRoutineHandle(void);
void RFIDTickHandle(void);
void RFID_PwmInit(void);
void RFID_Receive(void);
#endif

