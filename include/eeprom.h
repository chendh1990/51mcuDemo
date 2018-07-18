#ifndef __EEPROM_H
#define __EEPROM_H

///////////////////////hjc//////////////////////////
#define EE_DATA_INIT_FLAG									0xaa

#define EE_SECTOR_SIZE     									512 //sector size is 512byte

#define EE_START_ADDRESS			        				0x0000 
//////////////////////////////////////////////////////////////////////////////////////
//					EEPROM MAP(STC15L2K48S2 EEPROM SIZE 13K(0X0000~0X33FF), TOTAL OF SECTORS: 26) 
////////////////////////////////////////////////////////////////////////////////////// 
#define EE_1TH_SECTOR_BASE_ADDRESS			        		EE_START_ADDRESS	
#define EE_SYS_PARAM_ADR        							EE_1TH_SECTOR_BASE_ADDRESS   

#define EE_2TH_SECTOR_BASE_ADDRESS			        		EE_START_ADDRESS+EE_SECTOR_SIZE	
#define EE_SYS_PARAM_BACKUP_ADR								EE_2TH_SECTOR_BASE_ADDRESS

#define EE_3TH_SECTOR_BASE_ADDRESS       					EE_2TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE    
#define EE_HISTORY_LOG_START_ADDRESS						EE_3TH_SECTOR_BASE_ADDRESS

#define EE_4TH_SECTOR_BASE_ADDRESS       					EE_3TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   

#define EE_5TH_SECTOR_BASE_ADDRESS       					EE_4TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   
#define EE_WIRELESS_DOOR_BELL_START_ADDRESS					EE_5TH_SECTOR_BASE_ADDRESS

#define EE_6TH_SECTOR_BASE_ADDRESS       					EE_5TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   
#define EE_WIRELESS_DOOR_BELL_NAME_ADDRESS					EE_6TH_SECTOR_BASE_ADDRESS

#define EE_7TH_SECTOR_BASE_ADDRESS       					EE_6TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   
#define EE_WIRELESS_ACCESSORY_START_ADDRESS					EE_7TH_SECTOR_BASE_ADDRESS

#define EE_8TH_SECTOR_BASE_ADDRESS       					EE_7TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   
#define EE_ZONE_NAME_FIRST_START_ADDRESS					EE_8TH_SECTOR_BASE_ADDRESS

#define EE_9TH_SECTOR_BASE_ADDRESS       					EE_8TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   
#define EE_ZONE_NAME_SECOND_START_ADDRESS       			EE_9TH_SECTOR_BASE_ADDRESS

#define EE_10TH_SECTOR_BASE_ADDRESS			        		EE_9TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE	
#define EE_ZONE_NAME_THIRD_START_ADDRESS       				EE_10TH_SECTOR_BASE_ADDRESS

#define EE_11TH_SECTOR_BASE_ADDRESS       					EE_10TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE    
#define EE_ZONE_NAME_FORTH_START_ADDRESS       				EE_11TH_SECTOR_BASE_ADDRESS

#define EE_12TH_SECTOR_BASE_ADDRESS       					EE_11TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   
#define EE_REMOTE_CONTROL_START_ADDRESS						EE_12TH_SECTOR_BASE_ADDRESS 

#define EE_13TH_SECTOR_BASE_ADDRESS       					EE_12TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   
#define EE_REMOTE_CONTROL_NAME_ADDRESS						EE_13TH_SECTOR_BASE_ADDRESS

#define EE_14TH_SECTOR_BASE_ADDRESS       					EE_13TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   

#define EE_15TH_SECTOR_BASE_ADDRESS       					EE_14TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   

#define EE_16TH_SECTOR_BASE_ADDRESS       					EE_15TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   

#define EE_17TH_SECTOR_BASE_ADDRESS       					EE_16TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   

#define EE_18TH_SECTOR_BASE_ADDRESS       					EE_17TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE    

#define EE_19TH_SECTOR_BASE_ADDRESS       					EE_18TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   

#define EE_20TH_SECTOR_BASE_ADDRESS       					EE_19TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   

#define EE_21TH_SECTOR_BASE_ADDRESS       					EE_20TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   
#define EE_RF_ADDRESS_MAP_START								EE_21TH_SECTOR_BASE_ADDRESS

#define EE_22TH_SECTOR_BASE_ADDRESS       					EE_21TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   

#define EE_23TH_SECTOR_BASE_ADDRESS       					EE_22TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   

#define EE_24TH_SECTOR_BASE_ADDRESS       					EE_23TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   
#define EE_BACK_UP_SECTOR_ADDRESS			        		EE_24TH_SECTOR_BASE_ADDRESS

#define EE_25TH_SECTOR_BASE_ADDRESS       					EE_24TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE    

#define EE_26TH_SECTOR_BASE_ADDRESS       					EE_25TH_SECTOR_BASE_ADDRESS+EE_SECTOR_SIZE   


//////////////////////IAP COMMAND/////////////////////////////////////////
#define CMD_IDLE    				0         
#define CMD_READ    				1        
#define CMD_PROGRAM 				2   
#define CMD_ERASE   				3      
#define URMD    					0     


#define ENABLE_IAP					0x80           //if SYSCLK<30MHz
//#define ENABLE_IAP 				0x81           //if SYSCLK<24MHz
//#define ENABLE_IAP  				0x82           //if SYSCLK<20MHz
//#define ENABLE_IAP 				0x83           //if SYSCLK<12MHz
//#define ENABLE_IAP 				0x84           //if SYSCLK<6MHz
//#define ENABLE_IAP 				0x85           //if SYSCLK<3MHz
//#define ENABLE_IAP 				0x86           //if SYSCLK<2MHz
//#define ENABLE_IAP 				0x87           //if SYSCLK<1MHz

//////////////////////////////////////////////////////////////////////////////
#define FLASH_ADDRESS_SIZE										2

unsigned char EE_Write_Sector(unsigned int begin_addr, unsigned int counter, unsigned char *da);
void IapEraseSector(unsigned int addr);
unsigned char IapReadByte(unsigned int addr);
void IapProgramByte(unsigned int addr, unsigned char dat);
void EE_Data_Default(void);
void EE_Read_Data(unsigned int adr, unsigned char *buf, unsigned char size);
unsigned char EE_WriteSector(unsigned int begin_addr, unsigned int counter, unsigned char *da);
void SysParamFromEeprom(void);
void SysParamToEeprom(void);
#endif

