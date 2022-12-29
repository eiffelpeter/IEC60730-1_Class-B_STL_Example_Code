/******************************************************************************
 * @file     wdt_wwdt_reset_test.c
 * @version  V0.10
 * $Revision: 1 $
 * $Date: 18/06/15 2:24p $
 * @brief    IEC60730 Interrupt Test
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "../iec60730_test.h"

/*-------------------------------------------*/
/*         	 wdt reset test 		 	     */
/*-------------------------------------------*/
void WDT_reset_test(void)
{
	/* unlock register */
    SYS_UnlockReg();
	
    /* clear WDT reset flag */
    WDT_CLEAR_RESET_FLAG();
	
    /* reset counter */	
    WDT_RESET_COUNTER();	
	
    /* set WDT time out period (16 ticks) and enable WDT/WDT_INT/WDT_RST */
    WDT->CTL = WDT_TIMEOUT_2POW4 | WDT_CTL_WDTEN_Msk | WDT_CTL_RSTEN_Msk;

	/* lock register */
    SYS_LockReg();

	/* wait for WDT timeout */
	while(1);
}

/*-------------------------------------------*/
/*         	 wwdt reset test 		 	     */
/*-------------------------------------------*/
void WWDT_reset_test(void)
{
	/* unlock register */
    SYS_UnlockReg();
	
    /* clear WWDT reset flag */
    WWDT_CLEAR_RESET_FLAG();
	
    /* reload counter */	
    WWDT->RLDCNT = 0x00;
	
    /* set WDT time out period (16 ticks) and enable WDT/WDT_INT/WDT_RST */
    WWDT->CTL = (0x80 <<WWDT_CTL_CMPDAT_Pos) | WWDT_PRESCALER_1 | WWDT_CTL_WWDTEN_Msk;

	/* lock register */
    SYS_LockReg();

	/* wait for WWDT timeout */
	while(1);
}

/*-------------------------------------------*/
/*         	 wdt & wwdt reset test 		     */
/*-------------------------------------------*/
uint32_t IEC60730_WDT_WWDT_Reset_Test(uint32_t* WDTTestPass)
{
    /* check if WDT reset flag */
	if (!WDT_GET_RESET_FLAG()) {
		printf("\n");
		printf("WDT no reset flag !!\n");
		printf("do WDT reset test !!\n");
		WDT_reset_test();
	}

    /* check if WDT reset flag */
	if (!WWDT_GET_RESET_FLAG()) {
		printf("\n");
		printf("WWDT no reset flag !!\n");	
		printf("do WWDT reset test !!\n");		
		WWDT_reset_test();
	}
	
	/* clear WDT & WWDT reset flag */
    SYS_UnlockReg();
    WDT_CLEAR_RESET_FLAG();
    WWDT_CLEAR_RESET_FLAG();
    SYS_LockReg();	

	/* clear WDT & WWDT reset flag */
	*WDTTestPass = 1;
	return 0;
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
