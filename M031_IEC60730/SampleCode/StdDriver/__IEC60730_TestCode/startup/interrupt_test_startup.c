/******************************************************************************
 * @file     intrrupt_test.c
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

volatile uint32_t u32SysTickCnt;
volatile uint32_t u32Timer0Cnt;
uint32_t u32TimerRatio;
uint32_t u32TimeDevation;

volatile uint32_t u32TimerIntFlag0;

static uint8_t volatile u8WDTINT = FALSE;
static uint8_t volatile u8TestResult = FAIL;


/*-------------------------------------------*/
/*                systick isr                */
/*-------------------------------------------*/
void SysTick_Handler(void)
{
    u32SysTickCnt++;
}

/*-------------------------------------------*/
/*                timer0 isr                 */
/*-------------------------------------------*/
void TMR0_IRQHandler(void)
{
    TIMER_ClearIntFlag(TIMER0);
	u32Timer0Cnt++;
    u32TimerIntFlag0 = 1;
}

/*-------------------------------------------*/
/*                init systick               */
/*-------------------------------------------*/
static void InitSysTick(void)
{
    u32SysTickCnt = 0;
	
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable SysTick module */
#ifdef HCLK_FROM_HXT
	CLK_EnableSysTick(CLK_CLKSEL0_STCLKSEL_HCLK, __HXT/(SYSTICK_TIMEBASE*100));
#endif
#ifdef HCLK_FROM_HIRC
	CLK_EnableSysTick(CLK_CLKSEL0_STCLKSEL_HCLK, __HIRC/(SYSTICK_TIMEBASE*100));
#endif
#ifdef HCLK_FROM_PLL
	CLK_EnableSysTick(CLK_CLKSEL0_STCLKSEL_HCLK, PLL_CLOCK/(SYSTICK_TIMEBASE*100));
#endif
	
	/* Lock protected registers */
    SYS_LockReg();
}

/*-------------------------------------------*/
/*                init timer0                */
/*-------------------------------------------*/
static void InitTimer0(void)
{
    /* set timer0 in periodic mode and timebase */
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000000/TIMER0_TIMEBASE);

    /* enable TIMER0 Intettupt */
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);

	/* clear flag */
	u32TimerIntFlag0 = 0;
	u32Timer0Cnt = 0;
}

uint32_t IEC60730_Interrupt_Test(uint32_t* IntTestPass)
{
    /* init timer0 */
    InitTimer0();
    InitSysTick();
	
	/* check ratio between timer0 and systick */
	if (TIMER0_TIMEBASE >= SYSTICK_TIMEBASE)
		u32TimerRatio = TIMER0_TIMEBASE/SYSTICK_TIMEBASE;
	else
		u32TimerRatio = SYSTICK_TIMEBASE/TIMER0_TIMEBASE;

	/* given default time devation */
	u32TimeDevation = u32TimerRatio/(100/TIMER_DEVATION);
	
	/* start timer0 */	
    TIMER_Start(TIMER0);

    /* wait for timer0 interrupt */
	while (1) {
		if (TIMER0_TIMEBASE >= SYSTICK_TIMEBASE) {
			if (u32Timer0Cnt==1) {
				TIMER_Close(TIMER0);
				NVIC_DisableIRQ(TMR0_IRQn);
				CLK_DisableSysTick();
				
				if((u32SysTickCnt >= u32TimerRatio-u32TimeDevation) 
					&& (u32SysTickCnt <= u32TimerRatio+u32TimeDevation)) {
				#if 0
					/* modified devation */
					if (u32SysTickCnt > u32TimerRatio)
						u32TimeDevation = (u32SysTickCnt - u32TimerRatio)+5;
					else
						u32TimeDevation = (u32TimerRatio - u32SysTickCnt)+5;
				#endif
					*IntTestPass = PASS;
				}
				else {
					printf("cnt=%d\n", u32SysTickCnt);
					*IntTestPass = FAIL;
				}
				break;
			}
		} 
		else {
			if (u32SysTickCnt==1) {
				TIMER_Close(TIMER0);
				NVIC_DisableIRQ(TMR0_IRQn);
				CLK_DisableSysTick();
				
				if((u32Timer0Cnt >= u32TimerRatio-u32TimeDevation) 
					&& (u32Timer0Cnt <= u32TimerRatio+u32TimeDevation)) {
				#if 0
					/* modified devation */
					if (u32Timer0Cnt > u32TimerRatio)
						u32TimeDevation = (u32Timer0Cnt - u32TimerRatio)+5;
					else
						u32TimeDevation = (u32TimerRatio - u32Timer0Cnt)+5;
				#endif
					*IntTestPass = PASS;
				}
				else {
					printf("cnt=%d\n", u32Timer0Cnt);
					*IntTestPass = FAIL;
				}
				break;				
			}
		}
	}
	return 0;
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
