#include "sys.h"
#include "uart.h"
#include "eeprom.h"
#include "WIFI.h"
#include "RF_remote.h"
#include "SysClk.h"

xdata uart_var_t g_sUart[UART_NUMBER];
uchar xdata g_cUart1ReceiveBuf[UART1_BUFFER_SIZE];
uchar xdata g_cUart2ReceiveBuf[UART_WIFI_BUFFER_SIZE];
code uchar  HEX[]="0123456789ABCDEF";

void ClearUartBuffer(uart_number_t eUartNo)
{
	g_sUart[eUartNo].iPushIndex = 0;
	memset(g_sUart[eUartNo].cReveiveBuf, 0, eUartNo == UART_WIFI ? UART_WIFI_BUFFER_SIZE : UART1_BUFFER_SIZE);
}

void UartOpen(uart_number_t eUartNo)	
{
	if(eUartNo == UART_PC)
	{
		g_sUart[UART_PC].cReveiveBuf = g_cUart1ReceiveBuf;
	}
	else	
	{
		g_sUart[UART_WIFI].cReveiveBuf = g_cUart2ReceiveBuf;
	}

	g_sUart[eUartNo].iPopIndex = 0;
	g_sUart[eUartNo].iPushIndex = 0;
	g_sUart[eUartNo].bRxControl = ENABLE;
	g_sUart[eUartNo].bRxStart = TRUE;
}

void UartInit(void)
{
	ACC = P_SW1;
	ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
	P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD) 
	P_SW2 &= 0Xfe;            
	S2CON = 0x50;
	IE2 = 0x01;    
	IP2 |= 0X01;
	T2L = (65536 - (FOSC/4/BAUD_115200)); 
	T2H = (65536 - (FOSC/4/BAUD_115200))>>8;
	AUXR |= 0x14;    
////////////////////////////////
	SCON = 0X50;
	AUXR |= 0x01;		
	ES = 1;                    
//	EA = 1;	
///////////////////////////////////
	UartOpen(UART_PC);
	UartOpen(UART_WIFI);
}

void Uart1ISR(void) interrupt 4
{
	if(RI==1)
	{
		RI = 0;
		if(g_sUart[UART_PC].bRxControl == ENABLE)
		{
			g_sUart[UART_PC].cRxFrameOverCount = UART_RX_TIMEOUT;
			if(g_sUart[UART_PC].bRxStart == TRUE)
			{
				g_sUart[UART_PC].bRxStart = FALSE;
				g_sUart[UART_PC].iPushIndex = 0;
			}
			if(g_sUart[UART_PC].iPushIndex < UART1_BUFFER_SIZE)
			{
				g_sUart[UART_PC].cReveiveBuf[g_sUart[UART_PC].iPushIndex++] = SBUF;
			}
		}	
	}
	else if(TI==1)
	{
		TI = 0;
		g_sUart[UART_PC].bTxOver = TRUE;
	}
		
}

void Uart2ISR(void) interrupt 8
{
	if(S2CON & S2RI)
	{
		S2CON &= ~S2RI;
		if(g_sUart[UART_WIFI].bRxControl == ENABLE)
		{
			g_sUart[UART_WIFI].cRxFrameOverCount = UART_RX_TIMEOUT;
			if(g_sUart[UART_WIFI].bRxStart == TRUE)
			{
				g_sUart[UART_WIFI].bRxStart = FALSE;
				g_sUart[UART_WIFI].iPushIndex = 0;
			}
			if(g_sUart[UART_WIFI].iPushIndex < UART_WIFI_BUFFER_SIZE)
			{
				g_sUart[UART_WIFI].cReveiveBuf[g_sUart[UART_WIFI].iPushIndex++] = S2BUF;
			}
		}			
	}
	else if(S2CON & S2TI)
	{
		S2CON &= ~S2TI;
		g_sUart[UART_WIFI].bTxOver = TRUE;
	}
}

void UartTickCheck(uart_number_t eUartNo)
{	
	if(g_sUart[eUartNo].cRxFrameOverCount > 0)
	{
		if(--g_sUart[eUartNo].cRxFrameOverCount == 0)
		{
			g_sUart[eUartNo].bRxControl = DISABLE;
			HostMsgPost(SYS_MSG_RECEIVE_DATA_FROM_USART, eUartNo);
		}
	}

	if(g_sUart[eUartNo].cTxTimeOut > 0)
	{
		g_sUart[eUartNo].cTxTimeOut--;
	}
}
//////////////////////////////////////////////////////////////////////////////
void UartSendByte(uart_number_t eUartNo, uchar cData)
{
	g_sUart[eUartNo].cTxTimeOut = UART_TX_TIMEOUT;
	g_sUart[eUartNo].bTxOver = FALSE;
	if(eUartNo == UART_PC)
	{
		SBUF = cData;
	}	
	else if(eUartNo == UART_WIFI)
	{
		S2BUF = cData;
	}
	
	while(1)
	{
		if((g_sUart[eUartNo].bTxOver==TRUE) || (g_sUart[eUartNo].cTxTimeOut == 0))
			return;
	}
}

void UartSendString(uchar dev,uchar *pData)
{
	uint cCount;
	
	cCount = 0;
	while(1)
	{
		if((*pData == '\0')||(*pData == END_FLAG)||(cCount++ > 500))
			break;
		UartSendByte(dev, *pData++);
	}
}

void UartSendEnterString(uchar dev,uchar *pData)
{
	uchar cCount;
	
	cCount = 0;
	while(1)
	{
		if(cCount++ == 254)
			break;

		if((*pData=='\r')&&(*(pData+1)=='\n'))
		//if((*pData==0x0d)&&(*(pData+1)==0x0a))	
		{
			break;
		}	
		UartSendByte(dev, *pData);
		pData++;
	}
} 

void UartSendWcharString(uchar dev,uchar *pData)
{
	uchar cCount;
	
	cCount = 0;
	while(1)
	{
		if(cCount++ == 255)
			break;
		if((*pData==END_FLAG)&&(*(pData+1)==END_FLAG))
			break;
		UartSendByte(dev, *pData);
		pData++;
		UartSendByte(dev, *pData);
		pData++;
	}
} 

void UartSendData(uchar dev,uchar *pData,uchar cLenght)
{
	uchar cIndex;

	for(cIndex=0; cIndex<cLenght; cIndex++)
	{
		UartSendByte(dev, *pData++);
	}
}

uchar IntToStr(uchar *pStr, uint iVal, data_type_t eDataType)
{
	uchar cIndex;
	uchar cStr[MAX_INT_DEC_LENGTH];
	uchar cDataWidth;
	uchar cTemp;
	uint iTemp;

	cDataWidth = 0;
	if(eDataType == DEC_TYPE)
	{
		iTemp = 10000;
		for(cIndex=0; cIndex<MAX_INT_DEC_LENGTH; cIndex++)
		{
			if(cIndex<(MAX_INT_DEC_LENGTH-1))
			{
				cTemp = iVal/iTemp;
				iVal = iVal%iTemp;
				cStr[cIndex] = HEX[cTemp];
			}	
			else
				cStr[cIndex] = HEX[iVal];
			if(cDataWidth == 0)
			{
				if(cStr[cIndex] != '0')
				{
					cDataWidth = MAX_INT_DEC_LENGTH - cIndex;
				}
			}
			
			iTemp /= 10;
		}
		if(cDataWidth == 0)
		{
			cDataWidth = 1;
		}
		memcpy(pStr, cStr+MAX_INT_DEC_LENGTH-cDataWidth, cDataWidth);
	}	
	else if(eDataType == HEX_TYPE)
	{		
		if(iVal > 0xff)
		{
			cDataWidth = 4;
			*pStr++ = HEX[(iVal>>12)&0x0f];
			*pStr++ = HEX[(iVal>>8)&0x0f];
			*pStr++ = HEX[(iVal>>4)&0x0f];
			*pStr++ = HEX[iVal&0x0f];
		}
		else
		{
			cDataWidth = 2;
			*pStr++ = HEX[(iVal>>4)&0x0f];
			*pStr++ = HEX[iVal&0x0f];			
		}
	}
	return cDataWidth;
}

void UsartPrint(uart_number_t dev,  uchar *pFormat, va_list vallist)
{
	uchar pTemp[5];
	uchar cRetVal;
	uint iArg;
	uchar cArg;
	uchar *cStr;
	
	while(1)
	{
		if(*pFormat == '%')
		{
			pFormat++;
			if(*pFormat == 'd')
			{	
				if(*(pFormat+1) == 'i')
				{
					pFormat++;
					iArg = va_arg(vallist,  uint);
				}
				else
					iArg = va_arg(vallist,  uchar);
				cRetVal = IntToStr(pTemp, iArg, DEC_TYPE);
				UartSendData(dev, pTemp, cRetVal);
			}
			else if(*pFormat == 'x')
			{
				if(*(pFormat+1) == 'i')
				{
					pFormat++;
					iArg = va_arg(vallist,  uint);
				}	
				else
					iArg = va_arg(vallist,  uchar);
				cRetVal = IntToStr(pTemp, iArg, HEX_TYPE);
				UartSendData(dev, pTemp, cRetVal);
			}
			else if(*pFormat == 'c')
			{
				cArg = va_arg(vallist,  uchar );
				UartSendByte(dev, cArg);
			}		
			else if(*pFormat == 's')
			{
				cStr = va_arg(vallist,  uchar *);
				UartSendString(dev, cStr);
			}
			else if(*pFormat == 'e')
			{
				cStr = va_arg(vallist,  uchar *);
				UartSendEnterString(dev, cStr);
			}	
			else if(*pFormat == 'u')
			{
				cStr = va_arg(vallist,  uchar *);
				UartSendWcharString(dev, cStr);
				break;
			}	
			else if(*pFormat == 'l')
			{
				cStr = va_arg(vallist,  uchar *);
				UartSendData(dev, (cStr+1), *cStr);
			}
			
		}
		else if(*pFormat == '\0')
		{
			break;
		}
		else
		{
			UartSendByte(dev, *pFormat);
		}
		pFormat++;
	}
}

void SysPrintf( uchar *pFormat, ...)
{
	if(g_SystemParam.DebugInforOutput == ENABLE)
	{
		va_list vallist;
		
		OSSemPend(SEM_PRINTF,0);	
		va_start(vallist,pFormat);
		UsartPrint(UART_PC, pFormat, vallist);
		va_end(vallist);
		OSSemPost(SEM_PRINTF);
	}
}

void CmdInquery(void)
{
	if(g_sUart[UART_PC].iPushIndex > 1)
		return;
	OSSemPend(SEM_PRINTF,0);	
	UartSendString(UART_PC, "\r\nUart command list below:");
	UartSendString(UART_PC, "\r\n[A]: Print all of system parameters.");
	UartSendString(UART_PC, "\r\n[B]: print GPIO status.");
	UartSendString(UART_PC, "\r\n[C]: Set server address, e.g \"C192.168.001.015:8800\": store the server IP address \"192.168.1.15\" and port number \"8800\" to host.");
	UartSendString(UART_PC, "\r\n[D]: Set siren mute(D0.mute on D1.mute off).");
	UartSendString(UART_PC, "\r\n[E]: Debug WIFI module, e.g \"EAT\": send \"AT\" to WIFI module.");
	UartSendString(UART_PC, "\r\n[F]: print on.");
	UartSendString(UART_PC, "\r\n[G]: print off.");
	OSSemPost(SEM_PRINTF);
}
///////////////////////////////////////////////////////////////////////////
void PrintParam(void)
{
	if(g_sUart[UART_PC].iPushIndex > 1)
		return;	
	SysPrintf("\r\nF/W version is %xi and Compiled at %s, %s", SOFTWARE_VERSION, __DATE__, __TIME__);
	SysPrintf( "\r\nSystem status: %d ( 1.Disarm 2.Arm 3.At home arm ).", g_SystemParam.SystemStatus);
	SysPrintf( "\r\nPower supply mode: %d ( 0.Battery 1.AC/DC Adaptor ).", g_SystemVar.ACStatus);
	SysPrintf( "\r\nSiren mute status: %d ( 0.Mute on 1.Mute off ).", g_SystemParam.SirenMuteFlag);
	SysPrintf( "\r\nSystem password: %c%c%c%c.", g_SystemParam.SystemPassword[0],g_SystemParam.SystemPassword[1],g_SystemParam.SystemPassword[2],g_SystemParam.SystemPassword[3]);	
	SysPrintf( "\r\nHost ID: %x.%x.%x.", g_SystemParam.HostID[0],g_SystemParam.HostID[1],g_SystemParam.HostID[2]);	
	SysPrintf( "\r\nHost mac address: %x:%x:%x:%x:%x:%x.", g_SystemParam.HostMacAdr[0],g_SystemParam.HostMacAdr[1],g_SystemParam.HostMacAdr[2],g_SystemParam.HostMacAdr[3],g_SystemParam.HostMacAdr[4],g_SystemParam.HostMacAdr[5]);	
	SysPrintf( "\r\nHost IP address: %d.%d.%d.%d.", g_SystemParam.HostIPAdr[0],g_SystemParam.HostIPAdr[1],g_SystemParam.HostIPAdr[2],g_SystemParam.HostIPAdr[3]);
	SysPrintf( "\r\nHost sub mask address: %d.%d.%d.%d.", g_SystemParam.HostSubMask[0],g_SystemParam.HostSubMask[1],g_SystemParam.HostSubMask[2],g_SystemParam.HostSubMask[3]);
	SysPrintf( "\r\nHost gate address: %d.%d.%d.%d.", g_SystemParam.HostGate[0],g_SystemParam.HostGate[1],g_SystemParam.HostGate[2],g_SystemParam.HostGate[3]);
	SysPrintf( "\r\nTCP server address: %d.%d.%d.%d.", g_SystemParam.TcpIpServerAdr[0],g_SystemParam.TcpIpServerAdr[1],g_SystemParam.TcpIpServerAdr[2],g_SystemParam.TcpIpServerAdr[3]);	
	SysPrintf( "\r\nTCP Server port: %xi", g_SystemParam.TcpIpServerPort);
	SysPrintf( "\r\nUDP port: %xi", g_SystemParam.UdpLocalPort);
	SysPrintf( "\r\nSiren lasting time: %d minutes.", g_SystemParam.SirenLastingTime);
	SysPrintf( "\r\nDelay arm time: %d seconds.", g_SystemParam.DelayArmTime);
	SysPrintf( "\r\nDelay alarm time: %d seconds.", g_SystemParam.DelayAlarmTime);	
	SysPrintf( "\r\nAuto arm time: %xi", g_SystemParam.AutoArmTime);
	SysPrintf( "\r\nAuto disarm time: %xi.", g_SystemParam.AutoDisarmTime);		
	SysPrintf( "\r\nTotals of registered RC: %d", g_SystemParam.RegisteredRemoteNum);
	SysPrintf( "\r\nTotals of registered sensor: %d", g_SystemParam.RegisteredWirelessAccessoryNum);
	SysPrintf( "\r\nTotals of registered door bell: %d", g_SystemParam.RegisteredDoorBellNum);		
	SysPrintf( "\r\nTotals of history log: %d", g_SystemParam.HistoryLogNum);	

	SysPrintf( "\r\ng_SystemVar.UploadPlatformControl: %d", g_SystemVar.UploadPlatformControl);
	SysPrintf( "\r\ng_WifiVar.cResetTimeOut: %d", g_WifiVar.cResetTimeOut);
#if 1
	{
		uchar Index;
		
		SysPrintf( "\r\n RC CODE:");
		EE_Read_Data(EE_REMOTE_CONTROL_START_ADDRESS, g_HostCommonBuf, 80);
		for(Index=0; Index<80; Index++)
		{
			if((Index%8==0)&&(Index>0))
				SysPrintf( "\r\n RC CODE:");
			SysPrintf( " %x", g_HostCommonBuf[Index]);
			
		}
		SysPrintf( "\r\n RF ADR MAP:");
		EE_Read_Data(EE_RF_ADDRESS_MAP_START, g_HostCommonBuf, 80);
		for(Index=0; Index<80; Index++)
		{
			if((Index%8==0)&&(Index>0))
				SysPrintf( "\r\n RC CODE:");
			SysPrintf( " %x", g_HostCommonBuf[Index]);
			
		}
		SysPrintf( "\r\n zone CODE:");
		EE_Read_Data(EE_WIRELESS_ACCESSORY_START_ADDRESS, g_HostCommonBuf, 80);
		for(Index=0; Index<80; Index++)
		{
			if((Index%8==0)&&(Index>0))
				SysPrintf( "\r\n RC CODE:");
			SysPrintf( " %x", g_HostCommonBuf[Index]);
		}
		SysPrintf( "\r\n zone NAME:");
		EE_Read_Data(EE_ZONE_NAME_FIRST_START_ADDRESS, g_HostCommonBuf, 128);
		for(Index=0; Index<128; Index++)
		{
			if((Index%32==0)&&(Index>0))
				SysPrintf( "\r\n RC CODE:");
			SysPrintf( " %x", g_HostCommonBuf[Index]);
		}
			
		SysPrintf( "\r\n door bell CODE:");
		EE_Read_Data(EE_WIRELESS_DOOR_BELL_START_ADDRESS, g_HostCommonBuf, 80);
		for(Index=0; Index<80; Index++)
		{
			if((Index%8==0)&&(Index>0))
				SysPrintf( "\r\n RC CODE:");
			SysPrintf( " %x", g_HostCommonBuf[Index]);
		}		
	}
#endif	
}

void GPIOStatus(void)
{
	if(g_sUart[UART_PC].iPushIndex > 1)
		return;
	SysPrintf( "\r\np_WIFI_LED: %c ", p_WIFI_LED==0?'0':'1');
	SysPrintf( "\r\np_SET_LED: %c ", p_SET_LED==0?'0':'1');
	SysPrintf( "\r\np_SOS_LED: %c ", p_SOS_LED==0?'0':'1');
	SysPrintf( "\r\np_9300_EN: %c ", p_9300_EN==0?'0':'1');
	SysPrintf( "\r\np_8002_EN: %c ", p_8002_EN==0?'0':'1');
	SysPrintf( "\r\np_RF_EN: %c ", p_RF_EN==0?'0':'1');
	SysPrintf( "\r\np_WIFI_RST: %c ", p_WIFI_RST==0?'0':'1');
	SysPrintf( "\r\np_WIFI_EN: %c ", p_WIFI_EN==0?'0':'1');
	SysPrintf( "\r\np_RF: %c ", p_RF==0?'0':'1');
	SysPrintf( "\r\np_AC: %c ", p_AC==0?'0':'1');
	SysPrintf( "\r\np_S1: %c ", p_S1==0?'0':'1');
	SysPrintf( "\r\np_S2: %c ", p_S2==0?'0':'1');	
}

void SetMuteStatus(void)
{
	if((memcmp(g_sUart[UART_PC].cReveiveBuf, "D0", 2) == 0)
	||(memcmp(g_sUart[UART_PC].cReveiveBuf, "D1", 2) == 0))
	{
		SysPrintf("\r\nUart set siren mute status.");
		g_SystemParam.SirenMuteFlag = g_sUart[UART_PC].cReveiveBuf[1] - '0';
		SysParamToEeprom();
	}
}
void SetIP(void)
{
	uchar *pTemp;
	
	pTemp = g_sUart[UART_PC].cReveiveBuf + 1;

	g_SystemParam.TcpIpServerAdr[0] = ((*pTemp)-'0')*100 + (*(pTemp+1)-'0')*10 + (*(pTemp+2)-'0');
	pTemp += 4;
	g_SystemParam.TcpIpServerAdr[1] = ((*pTemp)-'0')*100 + (*(pTemp+1)-'0')*10 + (*(pTemp+2)-'0');
	pTemp += 4;
	g_SystemParam.TcpIpServerAdr[2] = ((*pTemp)-'0')*100 + (*(pTemp+1)-'0')*10 + (*(pTemp+2)-'0');
	pTemp += 4;
	g_SystemParam.TcpIpServerAdr[3] = ((*pTemp)-'0')*100 + (*(pTemp+1)-'0')*10 + (*(pTemp+2)-'0');
	pTemp += 4;
	g_SystemParam.TcpIpServerPort = ((*pTemp)-'0')*1000 + (*(pTemp+1)-'0')*100 + (*(pTemp+2)-'0')*10 + (*(pTemp+3)-'0');
	
	SysParamToEeprom();
}

void DebugWIFI(void)
{
	WifiPrintf(&g_sUart[UART_PC].cReveiveBuf[1]);
}

void PrintOn(void)
{
	if(g_sUart[UART_PC].iPushIndex > 1)
		return;
	g_SystemParam.DebugInforOutput = ENABLE;
	SysParamToEeprom();
	SysPrintf( "\r\nDebug usart on!");
}

void PrintOff(void)
{
	if(g_sUart[UART_PC].iPushIndex > 1)
		return;
	g_SystemParam.DebugInforOutput = DISABLE;
	SysParamToEeprom();
	SysPrintf( "\r\nDebug usart off!");
}

Uart_Debug_t UartCMDFunSet[] =
{
	{ 
		"?",
		CmdInquery,
	},
	
	{ 
		"A",
		PrintParam,
	},

	{
		"B",
		GPIOStatus,	
	},

	{
		"C",
		SetIP,	
	},

	{
		"D",
		SetMuteStatus,	
	},
	
	{ 
		"E",
		DebugWIFI,
	},
	
	{
		"F",
		PrintOn,	
	},

	{ 
		"G",
		PrintOff,
	},
	
	{
		NULL,
		NULL,
	},	
};


void UartFromPC(void)
{
	Uart_Debug_t *pTemp;
	
	pTemp = UartCMDFunSet;
	while(pTemp->fun!=NULL)
	{		
		if(memcmp(g_sUart[UART_PC].cReveiveBuf, pTemp->UartCMD, 1)==0)
		{
			pTemp->fun();
			break;
		}
		pTemp++;
	}

	 ClearUartBuffer(UART_PC);
}

void UartReceiveHandler(MSG_t *pMsg)
{
	SysPrintf("\r\nUart %xi receive handler.", pMsg->Param);
	if(pMsg->Param == UART_PC) //debug uart
	{
		UartFromPC();
		g_sUart[UART_PC].bRxControl = ENABLE;
	}
	else if(pMsg->Param == UART_WIFI) 
	{
		WifiReceiveDataHandle();
		g_sUart[UART_WIFI].bRxControl = ENABLE;
	}
}

