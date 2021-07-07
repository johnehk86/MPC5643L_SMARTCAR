/*********************************************************/
/*				Include Header File						*/
/*********************************************************/
#include "Init_Modeclock.h"

/*********************************************************/
/*					Define								*/
/*********************************************************/
#define AUX0_clk_DIV0   3
#define AUX0_clk_DIV1   3

#define FCCU_CFK_KEY 	0x618B7A50
#define FCCU_NCFK_KEY 	0xAB3498FE

/*********************************************************/
/*			    Static Function Prototype		         */
/*********************************************************/
static void Init_ModePll(void); 
static void Init_ClockDiv(void);
static void clear_CF(void);
static void clear_NCF(void);

/*********************************************************/
/*			    	Variable	         				*/
/*********************************************************/


/*********************************************************/
/*			    	Function	         				*/
/*********************************************************/
void Init_ModeClock(void)
{
    Init_ModePll();
    Init_ClockDiv();
}

static void Init_ModePll(void) 
{
	if (ME.GS.B.S_CURRENT_MODE == 2) 
	{
		clear_CF();
		clear_NCF();

		RGM.FES.R = 0xFFFF;	  			/*Clear the FES*/
		RGM.FBRE.B.BE_SOFT_FUNC = 0;    /*RESET_B is asserted on a software 'functional' reset event*/
		ME.MCTL.R = 0x00005AF0; 		/*Change the mode to "reset" & Key*/
		ME.MCTL.R = 0x0000A50F;         /*Change the mode to "reset" & Key*/	  
	}

    CGM.OSC_CTL.R = 0x00800001;
    ME.DRUN.B.XOSCON = 1;
    /* RE  enter the drun mode, to update the configuration */
    ME.MCTL.R = 0x30005AF0;                /* Mode & Key */
    ME.MCTL.R = 0x3000A50F;                /* Mode & Key inverted */
  
    while(0 == ME.GS.B.S_XOSC) {};               /* Wait for mode entry to complete */
    while(1 == ME.GS.B.S_MTRANS) {};            /* Wait for mode entry to complete */
    while(3 != ME.GS.B.S_CURRENT_MODE) {};       /* Check DRUN mode has been entered */ 

    ME.MER.R = 0x0000001D;        /* Enable DRUN, RUN0, SAFE, RESET modes */
                                  /* Initialize PLL before turning it on: */
    /* fsys = fcrystal*ndiv/idf/odf */
    /* fvco must be from 256 MHz to 512 MHz */
    /* we want fsys = 80 MHz. fvco = fsys*odf = 80 MHz * 4 = 320 MHz */
    /* fsys =  40*64/8/4 = 80 MHz */

    CGM.AC3SC.R = 0x01000000; /* Select Xosc as PLL0 source clock */
    CGM.AC4SC.R = 0x01000000; /* Select Xosc as PLL1 source clock */


    /* 80 MHz */
    CGM.FMPLL[0].CR.B.IDF = 0x7;    /* FMPLL0 IDF=7 --> divide by 8 */
    CGM.FMPLL[0].CR.B.ODF = 0x1;    /* FMPLL0 ODF=1 --> divide by 4*/
    CGM.FMPLL[0].CR.B.NDIV = 64;    /* FMPLL0 NDIV=64 --> divide by 64 */

    CGM.FMPLL[0].CR.B.EN_PLL_SW = 1; 	/* enable progressive clock switching */ 
    
    ME.RUNPC[0].R = 0x000000FE; /* enable peripherals run in all modes */
    ME.LPPC[0].R = 0x00000000;  /* disable peripherals run in LP modes */

    ME.RUN[0].R = 0x001F0070;       /* RUN0 cfg: 16MHzIRCON,OSC0ON,PLL0ON,syclk=16 MHz IRC */        

    /* Mode Transition to enter RUN0 mode: */
    ME.MCTL.R = 0x40005AF0;         /* Enter RUN0 Mode & Key */
    ME.MCTL.R = 0x4000A50F;         /* Enter RUN0 Mode & Inverted Key */  
    while (ME.IS.B.I_MTC != 1) {}    /* Wait for mode transition to complete */    
    ME.IS.R = 0x00000001;           /* Clear Transition flag */ 
    
    /* wait for PLL to lock - will not lock before DRUN re-entry */
    while(0 == CGM.FMPLL[0].CR.B.S_LOCK) {};
	   
    
    ME.RUN[0].R = 0x001F0074;       /* RUN0 cfg: 16MHzIRCON,OSC0ON,PLL0ON,syclk=PLL */    

    /* Mode Transition to enter RUN0 mode: */
    ME.MCTL.R = 0x40005AF0;         /* Enter RUN0 Mode & Key */
    ME.MCTL.R = 0x4000A50F;         /* Enter RUN0 Mode & Inverted Key */  
    while (ME.IS.B.I_MTC != 1) {}    /* Wait for mode transition to complete */    
    ME.IS.R = 0x00000001;           /* Clear Transition flag */    

    /* enable CLKOUT on PB6 */
    /* ALT1 - PCR[22] - PA = 0b01 */
    SIU.PCR[22].R = 0x0600;
    
    /* set CLKOUT divider of 4 */
    CGM.OCDSSC.B.SELDIV = 0b10;  /* output selected Output Clock divided by 4 */
    CGM.OCDSSC.B.SELCTL = 0b0010;  /* system PLL */
    CGM.OCEN.B.EN = 1; 			 /* enable CLKOUT signal */
}

static void Init_ClockDiv(void) 
{
    CGM.SC_DC.B.DIV = 0x03;     /*System Clock Divider = 4, Peripheral Clock = 80MHz/4 =20MHz*/ 
    
    CGM.AC0SC.R = 0x04000000;  /* MPC56xxL: Select IPBClk for aux clk 0  */
    /* MPC56xxL: Enable aux clk 0 dividers */
    CGM.AC0DC.B.DE0 = 1;
    CGM.AC0DC.B.DIV0 = AUX0_clk_DIV0;
    CGM.AC0DC.B.DE1 = 1;
    CGM.AC0DC.B.DIV1 = AUX0_clk_DIV1;
}

static void clear_CF(void) {
	uint32_t i;
	/*uint32_t a[4] ={0,};*/
  
	for(i=0;i<4;i++) {
		FCCU.CFK.R = FCCU_CFK_KEY;
		FCCU.CF_S[i].R = 0xFFFFFFFF;

		while(FCCU.CTRL.B.OPS != 0x3){}           /* wait for the completion of the operation */
		/*a[i]=FCCU.CF_S[i].R;*/
	}
}

static void clear_NCF(void) {
	uint32_t i;
	/*uint32_t b[4] ={0u,};*/

	for(i=0;i<4;i++){
		FCCU.NCFK.R = FCCU_NCFK_KEY;
		FCCU.NCF_S[i].R = 0xFFFFFFFF;

		while(FCCU.CTRL.B.OPS != 0x3){}           /* wait for the completion of the operation */
		/*b[i]=FCCU.NCF_S[i].R;*/
	}
}

