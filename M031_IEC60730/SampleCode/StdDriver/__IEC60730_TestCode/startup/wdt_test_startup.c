/******************************************************************************
 * @file     wdt_test_startup.c
 * @version  V0.10
 * $Revision: 2 $
 * $Date: 18/06/15 2:23p $
 * @brief    IEC60730 Interrupt Test
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "../iec60730_test.h"

extern uint32_t u32Timer0Cnt;
uint32_t volatile u32WdtInt;

/*-------------------------------------------*/
/*                init wdt                   */
/*-------------------------------------------*/
static uint32_t InitWdt(void)
{
    /* enable wdt interrupt */
    NVIC_EnableIRQ(WDT_IRQn);

	/* unlock register */
    SYS_UnlockReg();
	
	/* select WDT clock source */	
//    CLK_SetModuleClock(WDT_MODULE, CLK_CLKSEL1_WDTSEL_LIRC, 0);
	
    /* clear WDT */
    WDT_RESET_COUNTER();
    WDT_CLEAR_TIMEOUT_INT_FLAG();
	
    /* set WDT time out period (1024 ticks), enable WDT, and enable WDT interrupt */
    WDT->CTL = WDT_TIMEOUT_2POW10 | WDT_CTL_WDTEN_Msk | WDT_CTL_INTEN_Msk;

	/* lock register */
    SYS_LockReg();
	
	/* init flag */	
	u32WdtInt = FALSE;

    return TRUE;
}

/*-------------------------------------------*/
/*      check wdt clock source frequency     */
/*-------------------------------------------*/
static uint32_t CheckWdtClockSourceFreq(void)
{
#ifdef WDT_FROM_LXT	
	return __LXT;
#endif
#ifdef WDT_FROM_LIRC	
   	return __LIRC;
#endif
#ifdef WDT_FROM_HCLK_DIV2048
	#ifdef HCLK_FROM_HXT
   	return __HXT/2048;
	#endif
	#ifdef HCLK_FROM_HIRC
   	return __HIRC/2048;
	#endif
	#ifdef HCLK_FROM_PLL
   	return __HSI/2048;	
	#endif
#endif
}

/*-------------------------------------------*/
/*                init timer0                */
/*-------------------------------------------*/
static void InitTimer0(void)
{
	static uint32_t wdt_freq;
	
	/* check wdt clock freq */
	wdt_freq = CheckWdtClockSourceFreq();
	
    /* set timer0 in periodic mode and timebase (__LIRC/100 ticks per sec) */
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, wdt_freq/100);

    /* enable timer0 interrupt */
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);

	/* clear timer count */
	u32Timer0Cnt = 0;
}

/*-------------------------------------------*/
/*                wdt test                   */
/*-------------------------------------------*/
uint32_t IEC60730_WDT_Test(uint32_t* WDTTestPass)
{
    /* init timers and wdt */
    InitTimer0();
    TIMER_Start(TIMER0);

    /* init wdt */
    InitWdt();
	
    /* wait WDT interrupt */
    while (u32WdtInt!=TRUE);

    /* disable wdt and its interrupt */	
    SYS_UnlockReg();
    WDT->CTL = 0; 
    SYS_LockReg();
    NVIC_DisableIRQ(WDT_IRQn);

    /* disclose timer0 and its interrupt */		
    TIMER_Close(TIMER0);
    NVIC_DisableIRQ(TMR0_IRQn);
	
    /* check timer0 count */
	/* WDT_duration = 1024/wdt_freq, Timer0_freq = wdt_freq/100 
	   u32Timer0Cnt = WDT_duration*Timer0_freq = 1024/100 */
    if((u32Timer0Cnt >= 1024/100-1) && (u32Timer0Cnt <= 1024/100+2))
		*WDTTestPass = PASS;
    else {
        printf("cnt=%d\n", u32Timer0Cnt);
		*WDTTestPass = FAIL;		
    }
	return 0;
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
