/******************************************************************************
 * @file     pc_test_runtime.c
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

#ifdef __ICCARM__
#pragma optimize=none
#endif

#ifdef __ARMCC_VERSION
#pragma O0
#endif

extern uint32_t pc_test_return1(void);
extern uint32_t pc_test_return2(void);

uint32_t IEC60730_PC_Test_RunTime(uint32_t* PCTestPass) {

    uint32_t u32Addr = (uint32_t)&pc_test_return2;

	*PCTestPass = PASS;
    if(pc_test_return1() != PC_RETURNVALUE)
        *PCTestPass = FAIL;

    if(pc_test_return2() != u32Addr)
        *PCTestPass = FAIL;

    return 0;
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
