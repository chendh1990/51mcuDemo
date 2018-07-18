#ifndef __GSMCODE_H
#define __GSMCODE_H

code unsigned char ATE0_W[]="ATE0&W\r\n";  
code unsigned char AT_DTMF_DEC[]="AT+DDET=1\r\n";
code unsigned char AT_CMGF_TEXT[]="AT+CMGF=1\r\n";
code unsigned char AT_CSCS_PDU[]="AT+CSCS=\"UCS2\"\r\n";
code unsigned char AT_CSMP_PDU[]="AT+CSMP=17,168,0,8\r\n";
code unsigned char AT_CMGR[]="AT+CMGR=";
code unsigned char AT_CMGD[]="AT+CMGD=";	
code unsigned char AT_CMGDA[]="AT+CMGDA=\"DEL ALL\"\r\n";
code unsigned char AT_CHFA[]="AT+CHFA=0\r\n";
code unsigned char AT_VTS[]="AT+VTS=";
code unsigned char AT_VTS1[]="\"3\"";
code unsigned char AT_VTS2[]="\"3,3\"";
code unsigned char AT_CMGS[]="AT+CMGS=";
code unsigned char ATA[]="ATA\r\n";
code unsigned char ATH[]="ATH\r\n";
code unsigned char ATD[]="ATD";
code unsigned char AT_CPIN[]="AT+CPIN?\r\n";
code unsigned char AT_IPR[]="AT+IPR=115200\r\n";
code unsigned char AT_CMIC[]="AT+CMIC=0,9\r\n"; 
code unsigned char AT_CLIP[]="AT+CLIP=1\r\n";
code unsigned char AT_CLVL[]="AT+CLVL=90\r\n";
code unsigned char AT_MORING[]="AT+MORING=0";
code unsigned char AT_CREG[]="AT+CREG?\r\n";
code unsigned char AT_CLCC[]="AT+CLCC\r\n";
code unsigned char AN_CMGL[]="+CMGL:";
code unsigned char AN_SK[]=">";
code unsigned char AN_READY[]="+CPIN: READY";
code unsigned char AN_RING[]="RING";
code unsigned char AN_CREG_OK[]="+CREG:";
code unsigned char AN_CLCC_OK[]="+CLCC: 1,0,0,0,0";
code unsigned char AN_CLIP[]="+CLIP: ";
code unsigned char AN_CMTI[]="+CMTI:";
code unsigned char AN_DTMF[]="+DTMF:";
code unsigned char AN_CMGR[]="+CMGR:";
code unsigned char AN_CMGS[]="+CMGS:";
code unsigned char AN_GSM_POWER_OFF[]="NORMAL POWER DOWN";
code unsigned char AN_BUSY[]="BUSY";
code unsigned char AN_NO_ANSWER[] = "NO ANSWER";
code unsigned char AN_NO_CARRIER[] = "NO CARRIER";
code unsigned char AN_OK[]="OK";
code unsigned char AN_ERROR[]="ERROR";
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

code unsigned char SIM_SMS_CSMP_TEX[]="AT+CSMP=17,168,0,0";



code unsigned char SIM_TOState[]="MO CONNECTED";

//HJC code unsigned char AN_CMGR[]="READ";//

code unsigned char SIM_SMS_SENT_PDU[]="AT+CMGS=";
code unsigned char SIM_SMS_SK[]=">";
//code unsigned char ERROR[]="ERROR";//           
code unsigned char Ask_No[]="+CMGR: 0,,0";   

////////DTMF

code unsigned char AT_GetSmsStatus[]="AT+CMGL=\"ALL\"";


#endif

