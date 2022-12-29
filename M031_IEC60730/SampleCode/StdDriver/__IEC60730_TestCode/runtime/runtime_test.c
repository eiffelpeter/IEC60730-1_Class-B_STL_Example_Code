/******************************************************************************
 * @file     runtime_test.c
 * @version  V0.10
 * $Revision: 3 $
 * $Date: 18/08/28 5:04p $
 * @brief    IEC60730 Runtime Test
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "iec60730_test_runtime.h"


#ifdef __ARMCC_VERSION
extern uint32_t Image$$RW_STACK$$Base;
extern uint32_t Image$$RW_STACK$$ZI$$Length;
extern __asm void Disable_AllException(void);
extern __asm void Enable_AllException(void);
#endif

#ifdef __ICCARM__
extern uint32_t CSTACK$$Base;
extern uint32_t CSTACK$$Limit;
#endif


/* variables declaration */
extern uint32_t volatile CPUTestPass;
extern uint32_t volatile PCTestPass;
extern uint32_t volatile IntTestPass;
extern uint32_t volatile StackTestPass;
extern uint32_t volatile RAMTestPass;
extern uint32_t volatile ROMTestPass;

extern uint32_t u32SysTickCnt;
extern uint32_t u32Timer0Cnt;
extern uint32_t u32TimerRatio;
uint32_t u32CPUTestOnce;
uint32_t u32PCTestOnce;
uint32_t u32StackTestOnce;
uint32_t u32RomTestOnce;
uint32_t u32RamTestOnce;
uint32_t u32IntTestOnce;
uint32_t u32RestWdtOnce;
uint32_t u32timercnt_old;
uint32_t volatile u32RunTimeInit;

/* for wdt startup test */
extern uint32_t volatile u32WdtInt;

/*-------------------------------------------*/
/*                wdt isr                    */
/*-------------------------------------------*/
void WDT_IRQHandler()
{
    /* for wdt startup test */
    SYS_UnlockReg();
    WDT_RESET_COUNTER();
    WDT_CLEAR_TIMEOUT_INT_FLAG();
    SYS_LockReg();
    u32WdtInt = TRUE;
}

/*-------------------------------------------*/
/*                init wdt                   */
/*-------------------------------------------*/
static void init_wdt(void)
{
    /* set wdt time out period, enable wdt, and wdt interrupt */
    WDT->CTL = WDT_TIMEOUT_2POW14 | WDT_CTL_WDTEN_Msk | WDT_CTL_INTEN_Msk;
    /* reset wdt counter andd time out flag */
    WDT_RESET_COUNTER();
    WDT_CLEAR_TIMEOUT_INT_FLAG();
}

/*-------------------------------------------*/
/*              reset wdt counter            */
/*-------------------------------------------*/
static void reset_wdt(void)
{
    SYS_UnlockReg();
    /* reset wdt counter andd time out flag */
    WDT_RESET_COUNTER();
    WDT_CLEAR_TIMEOUT_INT_FLAG();
    SYS_LockReg();
}

void IEC60730_DoRunTimeIntial(void)
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
	
	/* init wdt */
    SYS_UnlockReg();
	init_wdt();
    SYS_LockReg();
    NVIC_EnableIRQ(WDT_IRQn);	
	
	/* init related variables */	
	u32CPUTestOnce = 0;
	u32PCTestOnce = 0;
	u32StackTestOnce = 0;
	u32RomTestOnce = 0;
	u32RamTestOnce = 0;
	u32IntTestOnce = 0;
	u32RestWdtOnce = 0;
	u32timercnt_old = 0;
	u32RunTimeInit = 0;
}

void IEC60730_DoRunTimeChecks(void)
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
	/* CPU Registers Test */
	if (!(timercnt%CPUREG_TEST_CYCLE)) {
		if (!u32CPUTestOnce) {
			u32CPUTestOnce = 1;
			CPUTestPass = 0;
			IEC60730_CPU_Reg_Test_RunTime();
			if(CPUTestPass)
				rt_printf("runtime CPU reg test: Pass !!\n");
			else {
				rt_printf("runtime CPU reg test: Fail !!\n");
				while(1);
			}
		}
	}

	/* Program Counter Test */
	if (!(timercnt%PC_TEST_CYCLE)) {
		if (!u32PCTestOnce) {
			u32PCTestOnce = 1;
			IEC60730_PC_Test_RunTime((uint32_t*)&PCTestPass);
			if(PCTestPass)			
				rt_printf("runtime PC test: Pass !!\n");
			else {
				rt_printf("runtime PC test: Fail !!\n");
				while(1);
			}
		}
	}

	/* Stack Overrun Test */
	if (!(timercnt%STACK_TEST_CYCLE)) {
		if (!u32StackTestOnce) {
			u32StackTestOnce = 1;
			IEC60730_Stack_Test_RunTime((uint32_t*)&StackTestPass);
			if(StackTestPass)
				rt_printf("runtime Stack test: Pass !!\n");
			else {
				rt_printf("runtime Stack test: Fail !!\n");
				while(1);
			}
		}
	}

	/* RAM Test */
	if (!(timercnt%RAM_TEST_CYCLE)) {		
		if (!u32RamTestOnce) {
			u32RamTestOnce = 1;
			
	#ifdef OPT_ACCESS_TIME_CHECK	
			PA0 = 1;
	#endif	
	#ifdef RAM_RUNTIME_MARCHC_WOM
			IEC60730_RAM_MarchC_Test_RunTime((uint32_t*)&RAMTestPass);
	#endif
	#ifdef RAM_RUNTIME_MARCHX_WOM
			IEC60730_RAM_MarchX_Test_RunTime((uint32_t*)&RAMTestPass);
	#endif
	#ifdef OPT_ACCESS_TIME_CHECK	
			PA0 = 0;
	#endif	
			if(RAMTestPass)
				rt_printf("runtime RAM test: Pass !!\n");
			else {
				rt_printf("runtime RAM test: Fail !!\n");
				while(1);
			}
		}
	}

	/* ROM Test */
	if (!(timercnt%ROM_TEST_CYCLE)) {
		if (!u32RomTestOnce) {
			u32RomTestOnce = 1;
		#ifdef OPT_ACCESS_TIME_CHECK	
			PA3 = 1;
		#endif	
			IEC60730_Flash_Test_RunTime((uint32_t*)&ROMTestPass);
		#ifdef OPT_ACCESS_TIME_CHECK	
			PA3 = 0;
		#endif	

			if(ROMTestPass)
				rt_printf("runtime ROM test: Pass !!\n");
			else {
				rt_printf("runtime ROM test: Fail !!\n");
				while(1);
			}
		}
	}

	/* Interrupt Test */
	if (!(timercnt%INT_TEST_CYCLE)) {		
		if (!u32IntTestOnce) {
			u32IntTestOnce = 1;
			IEC60730_Interrupt_Test_RunTime((uint32_t*)&IntTestPass);
			if(IntTestPass)
				rt_printf("runtime Interrupt test: Pass !!\n");
			else {
				rt_printf("runtime Interrupt test: Fail !!\n");
				while(1);
			}
		}
	}

	/* reset wdt */
	if (!(timercnt%REST_WDT_CYCLE)) {		
		if (!u32RestWdtOnce) {
			u32RestWdtOnce = 1;
			reset_wdt();
		}
	}
	
	/* check whether counter has been updated or not */		
	if (u32timercnt_old != timercnt) {
		u32timercnt_old = timercnt;
		u32CPUTestOnce = 0;
		u32PCTestOnce = 0;
		u32StackTestOnce = 0;
		u32RomTestOnce = 0;
		u32RamTestOnce = 0;
		u32IntTestOnce = 0;
		u32RestWdtOnce = 0;
	}
	
	/* setup maximum counter value */
	if (timercnt >= 0x000FFFFF) {
		u32SysTickCnt = 0;
		u32Timer0Cnt = 0;
	}
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
