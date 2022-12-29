/******************************************************************************
 * @file     application_test.c
 * @version  V0.10
 * $Revision: 3 $
 * $Date: 19/01/09 5:00p $
 * @brief    IEC60730 Application Test
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "iec60730_test_application.h"

/* variables declaration */
extern uint32_t volatile GPIOTestPass;
extern uint32_t volatile SPITestPass;
extern uint32_t volatile PWMTestPass;
extern uint32_t volatile ADCTestPass;


extern uint32_t u32SysTickCnt;
extern uint32_t u32Timer0Cnt;
extern uint32_t u32TimerRatio;
uint32_t u32GPIOTestOnce;
uint32_t u32SPITestOnce;
uint32_t u32PWMTestOnce;
uint32_t u32ADCTestOnce;

void IEC60730_DoRunApplicationIntial(void)
{
	/* init timer0  */
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000000/TIMER0_TIMEBASE);
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);
	u32Timer0Cnt = 0;
    TIMER_Start(TIMER0);

	/* init systick */	
    SYS_UnlockReg();
#ifdef HCLK_FROM_HXT
	CLK_EnableSysTick(CLK_CLKSEL0_STCLKSEL_HCLK, __HXT/(SYSTICK_TIMEBASE*100));
#endif
#ifdef HCLK_FROM_HIRC
	CLK_EnableSysTick(CLK_CLKSEL0_STCLKSEL_HCLK, __HIRC/(SYSTICK_TIMEBASE*100));
#endif
#ifdef HCLK_FROM_PLL
	CLK_EnableSysTick(CLK_CLKSEL0_STCLKSEL_HCLK, PLL_CLOCK/(SYSTICK_TIMEBASE*100));
#endif
	
    SYS_LockReg();
    u32SysTickCnt = 0;
	
	
	/* init related variables */	
	u32GPIOTestOnce=0;
	u32SPITestOnce=0;
	u32PWMTestOnce=0;
	u32ADCTestOnce=0;

}


void IEC60730_DoRunApplication(void)
{
	uint32_t* ptimercnt;
	uint32_t  timercnt;

#ifdef RUNTIME_TEST_BASED_SYSTICK
	ptimercnt = (uint32_t*)&u32SysTickCnt;
#endif
#ifdef RUNTIME_TEST_BASED_TIMER0
	ptimercnt = (uint32_t*)&u32Timer0Cnt;
#endif
	
	/* get timer count */		
	timercnt = *ptimercnt;
	/* GPIO Test */
	
	if (!(timercnt%GPIO_TEST_CYCLE)) {
		if (!u32GPIOTestOnce) {
			u32GPIOTestOnce = 1;
			GPIOTestPass = 0;
			IEC60730_IO_Test();
			if(GPIOTestPass)
				printf("GPIO test: Pass !!\n");
			else {
				printf("GPIO test: Fail !!\n");
				while(1);
			}
		}
	}

	/* SPI Test */
	if (!(timercnt%SPI_TEST_CYCLE)) {
		if (!u32SPITestOnce) {
			u32SPITestOnce = 1;
			IEC60730_SPI_Test();
			if(SPITestPass)			
				printf("SPI test: Pass !!\n");
			else {
				printf("SPI test: Fail !!\n");
				while(1);
			}
		}
	}

	/* PWM Test */
	if (!(timercnt%PWM_TEST_CYCLE)) {
		if (!u32PWMTestOnce) {
			u32PWMTestOnce = 1;
			IEC60730_PWM_Test();
			if(PWMTestPass)
				printf("PWM test: Pass !!\n");
			else {
				printf("PWM test: Fail !!\n");
				while(1);
			}
		}
	}

	/* ADC Test */
	if (!(timercnt%ADC_TEST_CYCLE)) {		
		if (!u32ADCTestOnce) {
			u32ADCTestOnce = 1;
			IEC60730_AD_Test();
			if(ADCTestPass)
				printf("AD test: Pass !!\n");
			else {
				printf("AD test: Fail !!\n");
				while(1);
			}
		}
	}
	
	/* setup maximum counter value */
	if (timercnt >= 0x000FFFFF) {
		u32SysTickCnt = 0;
		u32Timer0Cnt = 0;
	}
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
