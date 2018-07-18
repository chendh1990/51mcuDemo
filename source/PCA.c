#include "sys.h"
#include "PCA.h"
#include "tone.h"


void PCAInit(void)
{
	ACC = P_SW1;
	ACC &= ~(CCP_S0 | CCP_S1);      //CCP_S0=0 CCP_S1=0
	P_SW1 = ACC;                    //(P1.2/ECI, P1.1/CCP0, P1.0/CCP1, P3.7/CCP2)
	CCON = 0;                     
	CL = 0;                       
	CH = 0;
	CMOD = 0x00; 
//	IP |= 0x80;
////////////PCA0//////////////
	CCAPM0 = 0;
	CR = 1;                    
//	EA = 1;


}


void PCA0_Isr(void)
{
	CCF0 = 0;  
	PWMIsr();
}

void PCA1_Isr(void)
{		
	CCF1 = 0;
}

void PCA_Isr() interrupt 7 using 1
{
	if(CCF0 == 1)
	{
		PCA0_Isr();
	}
	
	if(CCF1 == 1)
	{		
		PCA1_Isr();
	}

}


///////////////////////////////////////////////////////////////////////////

