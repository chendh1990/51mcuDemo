#ifndef __UART_H
#define __UART_H

#define UART1_BUFFER_SIZE					40
#define UART_WIFI_BUFFER_SIZE				500
#define UART_TX_TIMEOUT						4 // 40MS
#define UART_RX_TIMEOUT						6 // 40MS

#define MAX_PRINT_LENGTH					200
#define MAX_INT_DEC_LENGTH					5

enum{
	UART_RECEIVE_CMD=0,
//////////////////////////////		
	UART_UNIT_CMD_LAST,		
};

typedef enum {
	DEC_TYPE=0,					
	HEX_TYPE,
} data_type_t;

typedef enum {
	UART_PC=0,					
	UART_WIFI,
	UART_NUMBER,
} uart_number_t;

typedef struct
{
	uint iPushIndex;			
	uint iPopIndex;
	uchar *cReveiveBuf;	
	uchar cRxFrameOverCount;
	uchar cTxTimeOut;
	uchar bTxOver			:1;
	uchar bRxControl		:1;
	uchar bRxStart			:1;
}uart_var_t;

typedef struct{
	unsigned char *UartCMD;
	void  (*fun)();
} Uart_Debug_t;

extern xdata uart_var_t  g_sUart[UART_NUMBER];

extern code uchar HEX[];
void UartInit(void);
void UartTickCheck(uart_number_t eUartNo);
void UartReceiveHandler(MSG_t *pMsg);
void ClearUartBuffer(uart_number_t eUartNo);
void SysPrintf( uchar *pFormat, ...);
void UsartPrint(uart_number_t dev,  uchar *pFormat, va_list vallist);
#endif 

