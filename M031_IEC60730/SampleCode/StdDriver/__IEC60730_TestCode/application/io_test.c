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
#include "iec60730_test_application.h"

#ifdef __ICCARM__
#pragma optimize=none
#endif

#ifdef __ARMCC_VERSION
#pragma O0
#endif

extern uint32_t volatile GPIOTestPass;

//PB.3(Output) and PC.1(Input)
void IEC60730_IO_Test() {
		
		int32_t i32Err, i32TimeOutCnt;
	
    /* Configure PB.4 as Output mode and PC.1 as Input mode then close it */
    GPIO_SetMode(PB, BIT4, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT1, GPIO_MODE_INPUT);

    i32Err = 0;

    /* Use Pin Data Input/Output Control to pull specified I/O or get I/O pin status */
    /* Set PB.4 output pin value is low */
    PB4 = 0;

    /* Set time out counter */
    i32TimeOutCnt = 10000;

    /* Wait for PC.1 input pin status is low for a while */
    while(PC1 != 0)
    {
        if(i32TimeOutCnt > 0)
        {
            i32TimeOutCnt--;
        }
        else
        {
            i32Err = 1;
            break;
        }
    }

    /* Set PB.4 output pin value is high */
    PB4 = 1;

    /* Set time out counter */
    i32TimeOutCnt = 10000;

    /* Wait for PC.1 input pin status is high for a while */
    while(PC1 != 1)
    {
        if(i32TimeOutCnt > 0)
        {
            i32TimeOutCnt--;
        }
        else
        {
            i32Err = 1;
            break;
        }
    }
    /* Print test result */
    if(i32Err)
    {
			GPIOTestPass = FAIL;
    }
    else
    {
			GPIOTestPass = PASS;
    }

}



/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
