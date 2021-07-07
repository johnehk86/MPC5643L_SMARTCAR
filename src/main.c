/*********************************************************/
/*				Include Header File						*/
/*********************************************************/
#include "derivative.h"
#include "Init_Watchdog.h"
#include "Init_Modeclock.h"
#include "Init_Intc.h"
#include "Init_Pit.h"
/*********************************************************/

/*					Define								*/
/*********************************************************/


/*********************************************************/
/*			    Static Function Prototype		         */
/*********************************************************/


/*********************************************************/
/*			    Global Function Prototype	         	*/
/*********************************************************/
extern void enableIrq(void);
/*********************************************************/
/*			    	Variable	         				*/
/*********************************************************/
volatile int counter = 0;
extern uint32_t u32nuPitCounter; 


/*********************************************************/
/*			    	Function	         				*/
/*********************************************************/
int main(void)
{
	Init_WatchDog();			/*Watchdog Setting*/
	Init_ModeClock();			/*Mode Clock Setting*/
	Init_Intc();              /* Vecotr Table Configure and Enable Interrupts */
	Init_Pit();
	EnableInterrupt();

	SIU.PCR[99].B.PA = 0;  /*Pad Configuration GPO*/
	SIU.PCR[99].B.OBE = 1; /*Output Buffer Enable*/
	SIU.GPDO[99].R = 0x00; /*Initialize the GPIO99 output register*/

	/* Loop forever */
	while(1u) 
	{	  

		if(u32nuPitCounter <= 1000u)
	    {
			SIU.GPDO[99].R = 0x00;
	    }
		else if(u32nuPitCounter <= 2000u)
	    {
			SIU.GPDO[99].R = 0x01;
	    }
		else
	    {
			u32nuPitCounter = 0u;
	    }

	   	counter++;
	}
}
