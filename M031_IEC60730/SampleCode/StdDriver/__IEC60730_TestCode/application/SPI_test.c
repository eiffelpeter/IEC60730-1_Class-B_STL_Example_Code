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


#define TEST_CYCLE      0x1000
#define DATA_COUNT      64
#define SPI_CLK_FREQ    2000000

#ifdef __ICCARM__
#pragma optimize=none
#endif

#ifdef __ARMCC_VERSION
#pragma O0
#endif

extern uint32_t volatile SPITestPass;

uint32_t g_au32SourceData[DATA_COUNT];
uint32_t g_au32DestinationData[DATA_COUNT];

void SPI_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init SPI                                                                                                */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Configure as a master, clock idle low, 32-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    /* Set IP clock divider. SPI clock rate = 2MHz */
    SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 32, SPI_CLK_FREQ);

    /* Enable the automatic hardware slave select function. Select the SS pin and configure as low-active. */
    SPI_EnableAutoSS(SPI0, SPI_SS, SPI_SS_ACTIVE_LOW);
}



void IEC60730_SPI_Test() {
		
		uint32_t u32DataCount, u32TestCycle, u32Err;
		
		SPI_Init();
    u32Err = 0;
    for(u32TestCycle = 0; u32TestCycle < TEST_CYCLE; u32TestCycle++)
    {
        /* set the source data and clear the destination buffer */
        for(u32DataCount = 0; u32DataCount < DATA_COUNT; u32DataCount++)
        {
            g_au32SourceData[u32DataCount] = u32DataCount;
            g_au32DestinationData[u32DataCount] = 0;
        }

        u32DataCount = 0;

        while(1)
        {
            /* Write to TX register */
            SPI_WRITE_TX(SPI0, g_au32SourceData[u32DataCount]);
            /* Check SPI0 busy status */
            while(SPI_IS_BUSY(SPI0));
            /* Read received data */
            g_au32DestinationData[u32DataCount] = SPI_READ_RX(SPI0);
            u32DataCount++;
            if(u32DataCount >= DATA_COUNT)
                break;
        }

        /*  Check the received data */
        for(u32DataCount = 0; u32DataCount < DATA_COUNT; u32DataCount++)
        {
            if(g_au32DestinationData[u32DataCount] != g_au32SourceData[u32DataCount])
                u32Err = 1;
        }

        if(u32Err)
            break;
    }

    if(u32Err)
        SPITestPass = FAIL;
    else
        SPITestPass = PASS;
}


/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
