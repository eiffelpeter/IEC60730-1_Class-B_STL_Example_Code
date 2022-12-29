/******************************************************************************
 * @file     stack_overrun_ptrn.c
 * @version  V0.10
 * $Revision: 2 $
 * $Date: 18/06/15 2:23p $
 * @brief    IEC60730 Program Counter Test
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "iec60730_test_runtime.h"

//******************************//
// Need to use scatter file
// iec60730.sct
//******************************//

#ifdef __ICCARM__
#pragma optimize=none
#endif

#ifdef __ARMCC_VERSION
#pragma O0
#endif

#ifdef __ICCARM__
#pragma default_function_attributes = @ "STACK_PTRN"
uint32_t const STACK_TEST_PTRN[] = {STACK_OVERRUN_PTRN0, STACK_OVERRUN_PTRN1,
								STACK_OVERRUN_PTRN2, STACK_OVERRUN_PTRN3};
#pragma default_function_attributes =
#endif

#ifdef __ARMCC_VERSION
__attribute__((section("STACK_PTRN")))
uint32_t const STACK_TEST_PTRN[] = {STACK_OVERRUN_PTRN0, STACK_OVERRUN_PTRN1,
								STACK_OVERRUN_PTRN2, STACK_OVERRUN_PTRN3};
#endif

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
