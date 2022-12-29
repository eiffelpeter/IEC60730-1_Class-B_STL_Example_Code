/******************************************************************************
 * @file     stack_overrun_ptrn.c
 * @version  V0.10
 * $Revision: 3 $
 * $Date: 18/08/28 5:04p $
 * @brief    IEC60730 Program Counter Test
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "iec60730_test_runtime.h"

#ifdef __ICCARM__
#pragma optimize=none
#endif

#ifdef __ARMCC_VERSION
#pragma O0
#endif

extern uint32_t volatile u32RunTimeInit;

extern uint32_t STACK_TEST_PTRN[];


static void IEC60730_Stack_Test_RunTime_Init(void)
{
	/* stack pattern is destoried by RAM test at startup stage */
	STACK_TEST_PTRN[0] = STACK_OVERRUN_PTRN0;
	STACK_TEST_PTRN[1] = STACK_OVERRUN_PTRN1;
	STACK_TEST_PTRN[2] = STACK_OVERRUN_PTRN2;
	STACK_TEST_PTRN[3] = STACK_OVERRUN_PTRN3;
}

uint32_t IEC60730_Stack_Test_RunTime(uint32_t* StackTestPass) {
	
	uint32_t* pstackptrn;
	
	if (!(u32RunTimeInit & RUNTIME_STACK_INTIT)) {
		u32RunTimeInit |= RUNTIME_STACK_INTIT;
		IEC60730_Stack_Test_RunTime_Init();
		*StackTestPass = PASS;		
	}
	
	pstackptrn = (uint32_t*) STACK_TEST_PTRN;
	if ((*pstackptrn++ == STACK_OVERRUN_PTRN0) &&
		(*pstackptrn++ == STACK_OVERRUN_PTRN1) &&
		(*pstackptrn++ == STACK_OVERRUN_PTRN2) &&
		(*pstackptrn   == STACK_OVERRUN_PTRN3) ) {
	
		*StackTestPass = PASS;
	} else {
		*StackTestPass = FAIL;
	}
	return 0;
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
