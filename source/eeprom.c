#include "sys.h"	   
#include "eeprom.h"
#include "RF_remote.h"
#include "uart.h"

code uchar HostDefaultIP[4]={192,168,1,18};
code uchar HostDefaultGateIP[4]={192,168,1,1};
code uchar HostDefaultSubMask[4]={255,255,255,0};
code uchar HostDefaultMac[6]={0x12,0x34,0x56,0x78,0x9a,0xbc};
code uchar ServerDefaultIP[4]={112,74,205,113};//{220,249,248,66};//{192,168,3,174};//{112,74,205,113};
void IapIdle()
{
	IAP_CONTR = 0;                 
	IAP_CMD = 0;                 
	IAP_TRIG = 0;             
	IAP_ADDRH = 0x80;       
	IAP_ADDRL = 0;
}

unsigned char IapReadByte(unsigned int addr)
{
	unsigned char dat;  
	
	IAP_CONTR = ENABLE_IAP;       
	IAP_CMD = CMD_READ;           
	IAP_ADDRL = addr;            
	IAP_ADDRH = addr >> 8;          
	IAP_TRIG = 0x5a;              
	IAP_TRIG = 0xa5;            
	_nop_();                        
	dat = IAP_DATA;           
	IapIdle();                      
	return dat;                   
}


void IapProgramByte(unsigned int addr, unsigned char dat)
{
	IAP_CONTR = ENABLE_IAP;       
	IAP_CMD = CMD_PROGRAM;  
	IAP_ADDRL = addr;             
	IAP_ADDRH = addr >> 8;      
	IAP_DATA = dat;            
	IAP_TRIG = 0x5a;             
	IAP_TRIG = 0xa5;          
	_nop_();                      
	IapIdle();
}


void IapEraseSector(unsigned int addr)
{
	IAP_CONTR = ENABLE_IAP;       
	IAP_CMD = CMD_ERASE;     
	IAP_ADDRL = addr;          
	IAP_ADDRH = addr >> 8;       
	IAP_TRIG = 0x5a;       
	IAP_TRIG = 0xa5;  
	_nop_();     
	_nop_();
	IapIdle();
}

unsigned char  EE_Write_Sector(unsigned int begin_addr, unsigned int counter, unsigned char *da)
{

	unsigned int index = 0;
	unsigned int Byte_Addr = 0;
	unsigned int sector_addr = 0;
	unsigned char temp;
	unsigned char Copy_Flag;
	unsigned char Inc_Num;

	if(counter > EE_SECTOR_SIZE)
		return FALSE;
	Byte_Addr = begin_addr & 0x01ff;

	if((Byte_Addr + counter) > EE_SECTOR_SIZE)
		return FALSE;
	OSSemPend(SEM_STC_FLASH,0);
	IapEraseSector(EE_BACK_UP_SECTOR_ADDRESS);
	sector_addr = (begin_addr & 0xfe00);
	Copy_Flag = DISABLE;
	Inc_Num = 0;
	for(index = 0; index < EE_SECTOR_SIZE; index++)
	{
		if((index == Byte_Addr)
			||(Copy_Flag == ENABLE))
		{
			Copy_Flag = ENABLE;
			temp = *da++;
			IapProgramByte(EE_BACK_UP_SECTOR_ADDRESS+index, temp);
			Inc_Num++;
			if(Inc_Num == counter)
			{
				Copy_Flag = DISABLE;
			}	
		}
		else
		{
			temp = IapReadByte(sector_addr+index);
			IapProgramByte(EE_BACK_UP_SECTOR_ADDRESS+index, temp);			
		}
	}
	IapEraseSector(sector_addr);
	for(index = 0; index< EE_SECTOR_SIZE; index++)
	{
		temp = IapReadByte(EE_BACK_UP_SECTOR_ADDRESS+index);
		IapProgramByte(sector_addr+index, temp);
	}
	
	IapIdle();
	OSSemPost(SEM_STC_FLASH);
	return TRUE;

}

unsigned char EE_WriteSector(unsigned int begin_addr, unsigned int counter, unsigned char *da)
{

	unsigned int index;
	unsigned int Byte_Addr;
	unsigned int sector_addr;
	
	if(counter > EE_SECTOR_SIZE)
		return FALSE;
	Byte_Addr = begin_addr & 0x01ff;

	if((Byte_Addr + counter) > EE_SECTOR_SIZE)
		return FALSE;
	OSSemPend(SEM_STC_FLASH,0);
	if(Byte_Addr == 0)
	{
		sector_addr = (begin_addr & 0xfe00);
		IapEraseSector(sector_addr);
	}
	for(index = 0; index < counter; index++)
	{
		IapProgramByte(begin_addr+index, *da++);
	}
	OSSemPost(SEM_STC_FLASH);
	return TRUE;

}

void EE_Read_Data(unsigned int adr, unsigned char *buf, unsigned char size)
{
	unsigned char index;
	
	OSSemPend(SEM_STC_FLASH,0);
	for(index = 0; index< size; index++)
	{
		*buf++ = IapReadByte(adr+index);
	}
	OSSemPost(SEM_STC_FLASH);
}

unsigned int CaculateChecksum(unsigned char *buf, unsigned char len)
{
	unsigned char index;
	unsigned int check_sum;
	
	check_sum = 0;
	for(index=0; index<len; index++)
	{
		check_sum += *buf++;
	}

	return check_sum;
}

void SysParamLoadDefault(void)
{
	g_SystemParam.ParamInitFlag = EE_DATA_INIT_FLAG;
	g_SystemParam.SystemStatus = SYSTEM_DISARMED_STATUS;
#ifdef MAX_SIREN_3_MINUTE
	g_SystemParam.SirenLastingTime = 1;
#else
	g_SystemParam.SirenLastingTime = 0x05;
#endif
	g_SystemParam.AutoUploadTime = AUTO_UPLOAD_SYSTEM_STATUS_TIME;
	g_SystemParam.SystemLanguage = LANGUAGE_ENGLISH;
	memcpy(g_SystemParam.SystemPassword, "1234", 4);
	g_SystemParam.DebugInforOutput = 0;//ENABLE;//DISABLE;
	g_SystemParam.RegisteredRemoteNum = 0;
	g_SystemParam.RegisteredDoorBellNum = 0;
	g_SystemParam.RegisteredWirelessAccessoryNum = 0;
	g_SystemParam.HistoryLogNum = 0;
	g_SystemParam.SirenMuteFlag = 1;
	g_SystemParam.RingTime = 3;
	g_SystemParam.CallRecycleTime = 3;
	g_SystemParam.AccompanyToneFlag = FALSE;
	g_SystemParam.ArmDisarmSmsInforFlag = FALSE;
	g_SystemParam.AutoArmDisarmFlag = FALSE;
	g_SystemParam.AcSwitchInforFlag = FALSE;;

	g_SystemParam.HostID[0] = CBYTE[CPU_ID_ADD_END-3];
	g_SystemParam.HostID[1] = CBYTE[CPU_ID_ADD_END-4];
	g_SystemParam.HostID[2] = CBYTE[CPU_ID_ADD_END-5];

	g_SystemParam.HostMacAdr[0] = CBYTE[CPU_ID_ADD_END];
	g_SystemParam.HostMacAdr[1] = CBYTE[CPU_ID_ADD_END-1];
	g_SystemParam.HostMacAdr[2] = CBYTE[CPU_ID_ADD_END-2];
	g_SystemParam.HostMacAdr[3] = CBYTE[CPU_ID_ADD_END-3];
	g_SystemParam.HostMacAdr[4] = CBYTE[CPU_ID_ADD_END-4];
	g_SystemParam.HostMacAdr[5] = CBYTE[CPU_ID_ADD_END-5];	

	memcpy(g_SystemParam.HostIPAdr, HostDefaultIP, IP_ADDRESS_LENGTH);
	memcpy(g_SystemParam.HostSubMask, HostDefaultSubMask, IP_ADDRESS_LENGTH);
	memcpy(g_SystemParam.HostGate, HostDefaultGateIP, IP_ADDRESS_LENGTH);
	memcpy(g_SystemParam.TcpIpServerAdr, ServerDefaultIP, IP_ADDRESS_LENGTH);

	g_SystemParam.TcpIpServerPort = TCP_DEFAULT_PORT;
	g_SystemParam.UdpLocalPort = UDP_DEFAULT_PORT;
	g_SystemParam.UdpSmartLinkPort = UDP_SMART_LINK_DEFAULT_PORT;
	
	g_SystemParam.DelayArmTime = 0;
	g_SystemParam.DelayAlarmTime = 0;
	g_SystemParam.AutoArmTime = 0;
	g_SystemParam.AutoDisarmTime = 0;

	g_SystemParam.RfAdrMapHead = 0;
	g_SystemParam.RfAdrMapTotal = 0;
	g_SystemParam.CheckSum = CaculateChecksum((unsigned char *)&g_SystemParam, sizeof(g_SystemParam)-2);
}

void SysParamFromEeprom(void)
{
	unsigned int check_sum;
	
	EE_Read_Data(EE_SYS_PARAM_ADR, (unsigned char *)&g_SystemParam, sizeof(g_SystemParam));
	check_sum = CaculateChecksum((unsigned char *)&g_SystemParam, sizeof(g_SystemParam)-2);
	if(check_sum != g_SystemParam.CheckSum)
	{
		SysPrintf("\r\nsystem param from back up.");
		EE_Read_Data(EE_SYS_PARAM_BACKUP_ADR, (unsigned char *)&g_SystemParam, sizeof(g_SystemParam));
		check_sum = CaculateChecksum((unsigned char *)&g_SystemParam, sizeof(g_SystemParam)-2);
		if(check_sum != g_SystemParam.CheckSum)
		{
			SysPrintf("\r\nsystem param from default.");
			SysParamLoadDefault();
			EE_WriteSector(EE_SYS_PARAM_ADR, sizeof(g_SystemParam), (unsigned char *)&g_SystemParam);
			EE_WriteSector(EE_SYS_PARAM_BACKUP_ADR, sizeof(g_SystemParam), (unsigned char *)&g_SystemParam);
			IapEraseSector(EE_RF_ADDRESS_MAP_START);
		}
	}
}

void SysParamToEeprom(void)
{
	unsigned int check_sum;
	
	check_sum = CaculateChecksum((unsigned char *)&g_SystemParam, sizeof(g_SystemParam)-2);
	g_SystemParam.CheckSum = check_sum;
	EE_WriteSector(EE_SYS_PARAM_ADR, sizeof(g_SystemParam), (unsigned char *)&g_SystemParam);
	EE_WriteSector(EE_BACK_UP_SECTOR_ADDRESS, sizeof(g_SystemParam), (unsigned char *)&g_SystemParam);		
}

void EE_Data_Default(void)
{	
	unsigned char Index;
	
	SysPrintf("\r\nSystem default setting!");
	OSSemPend(SEM_STC_FLASH,0);
	for(Index=0; Index<EE_SECTOR_TOTAL; Index++)
	{
		IapEraseSector(EE_1TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE*Index);
		DelayNop(100);
	}
	OSSemPost(SEM_STC_FLASH);
	SysParamLoadDefault();
	EE_WriteSector(EE_SYS_PARAM_ADR, sizeof(g_SystemParam), (unsigned char *)&g_SystemParam);
	EE_WriteSector(EE_SYS_PARAM_BACKUP_ADR, sizeof(g_SystemParam), (unsigned char *)&g_SystemParam);
}


