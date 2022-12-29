/******************************************************************************
 * @file     programcounter_test.c
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
#include "../iec60730_test.h"

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

//return a defined value
#ifdef __ICCARM__
#pragma default_function_attributes = @ "pc_test1"
uint32_t pc_test_return1(void)
{
    return PC_RETURNVALUE;
}
#pragma default_function_attributes =

//return myself address
#pragma default_function_attributes = @ "pc_test2"
uint32_t pc_test_return2(void)
{
    return ((uint32_t) &pc_test_return2);
}
#pragma default_function_attributes =
#endif

#ifdef __ARMCC_VERSION
__attribute__((section("pc_test_1")))
uint32_t pc_test_return1(void)
{
    return PC_RETURNVALUE;
}

//return myself address
__attribute__((section("pc_test_2")))
uint32_t pc_test_return2(void)
{
    return ((uint32_t) &pc_test_return2);
}
#endif

uint32_t IEC60730_PC_Test(uint32_t* PCTestPass)
{
    uint32_t u32Addr = (uint32_t)&pc_test_return2;

	*PCTestPass = PASS;
    if(pc_test_return1() != PC_RETURNVALUE)
        *PCTestPass = FAIL;

    if(pc_test_return2() != u32Addr)
        *PCTestPass = FAIL;

    return 0;
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
