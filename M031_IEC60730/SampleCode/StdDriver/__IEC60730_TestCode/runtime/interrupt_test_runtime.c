/******************************************************************************
 * @file     interrupt_test_runtime.c
 * @version  V0.10
 * $Revision: 3 $
 * $Date: 18/08/28 5:04p $
 * @brief    IEC60730 Interrupt Test
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "iec60730_test_runtime.h"

extern volatile uint32_t u32SysTickCnt;
extern volatile uint32_t u32Timer0Cnt;
extern uint32_t u32TimerRatio;
extern uint32_t u32TimeDevation;
extern uint32_t volatile u32RunTimeInit;

void IEC60730_Interrupt_Test_RunTime_Init(void)
{
	if (TIMER0_TIMEBASE >= SYSTICK_TIMEBASE)
		u32TimerRatio = TIMER0_TIMEBASE/SYSTICK_TIMEBASE;
	else
		u32TimerRatio = SYSTICK_TIMEBASE/TIMER0_TIMEBASE;

	u32TimeDevation = u32TimerRatio/(100/TIMER_DEVATION);	
	return;
}

uint32_t IEC60730_Interrupt_Test_RunTime(uint32_t* IntTestPass)
{

	if (!(u32RunTimeInit & RUNTIME_INT_INTIT)) {
		u32RunTimeInit |= RUNTIME_INT_INTIT;
		IEC60730_Interrupt_Test_RunTime_Init();
		*IntTestPass = PASS;
	}

	*IntTestPass = PASS;

	/* check whether timer count to be correct or not */		
	if (TIMER0_TIMEBASE >= SYSTICK_TIMEBASE) {
		/* ignore 1st u32Timer0Cnt */
		if (u32Timer0Cnt >=2) {
			if((u32SysTickCnt >= u32Timer0Cnt*(u32TimerRatio-u32TimeDevation))
				&& (u32SysTickCnt <= u32Timer0Cnt*(u32TimerRatio+u32TimeDevation))) {
				*IntTestPass = PASS;
			}
			else {
				rt_printf("u32SysTickCnt=%d\n", u32SysTickCnt);
				rt_printf("u32Timer0Cnt=%d\n", u32Timer0Cnt);
				*IntTestPass = FAIL;			
			}
		}
	} 
	else {
		/* ignore 1st u32SysTickCnt */		
		if (u32SysTickCnt >=2) {
			if((u32Timer0Cnt >= u32SysTickCnt*(u32TimerRatio-u32TimeDevation))
				&& (u32Timer0Cnt <= u32SysTickCnt*(u32TimerRatio+u32TimeDevation))) {
				*IntTestPass = PASS;
			}
			else {
				rt_printf("u32SysTickCnt=%d\n", u32SysTickCnt);
				rt_printf("u32Timer0Cnt=%d\n", u32Timer0Cnt);
				*IntTestPass = FAIL;			
			}
		}
	}
	return 0;
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
