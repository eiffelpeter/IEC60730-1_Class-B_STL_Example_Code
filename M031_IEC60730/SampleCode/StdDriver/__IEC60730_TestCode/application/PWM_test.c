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

//#define PLL_CLOCK           96000000
extern uint32_t volatile PWMTestPass;

void CalPeriodTime(PWM_T *PWM, uint32_t u32Ch)
{
    uint16_t u32Count[4];
    uint32_t u32i = 0;
    uint16_t u16HighPeriod, u16LowPeriod, u16TotalPeriod;
//		uint16_t u16RisingTime, u16FallingTime;

    /* Clear Capture Falling Indicator (Time A) */
    PWM_ClearCaptureIntFlag(PWM, u32Ch, PWM_CAPTURE_INT_FALLING_LATCH);

    /* Wait for Capture Falling Indicator  */
    while((PWM0->CAPIF & PWM_CAPIF_CFLIF2_Msk) == 0)
    {
        //printf("pwm while 1 \n");

        u32i++;

        if(u32i > 10000)
        {
            PWMTestPass = FAIL;
            return;
        }
    }

    /* Clear Capture Falling Indicator (Time B)*/
    PWM_ClearCaptureIntFlag(PWM, u32Ch, PWM_CAPTURE_INT_FALLING_LATCH);

    u32i = 0;

    while(u32i < 4)
    {
        /* Wait for Capture Falling Indicator */
        while(PWM_GetCaptureIntFlag(PWM, u32Ch) < 2)
        {
            //printf("pwm while 2 \n");
        }

        /* Clear Capture Falling and Rising Indicator */
        PWM_ClearCaptureIntFlag(PWM, u32Ch, PWM_CAPTURE_INT_FALLING_LATCH | PWM_CAPTURE_INT_RISING_LATCH);

        /* Get Capture Falling Latch Counter Data */
        u32Count[u32i++] = PWM_GET_CAPTURE_FALLING_DATA(PWM, u32Ch);

        /* Wait for Capture Rising Indicator */
        while(PWM_GetCaptureIntFlag(PWM, u32Ch) < 2)
        {
            //printf("pwm while 3 \n");
        }

        /* Clear Capture Rising Indicator */
        PWM_ClearCaptureIntFlag(PWM, u32Ch, PWM_CAPTURE_INT_RISING_LATCH);

        /* Get Capture Rising Latch Counter Data */
        u32Count[u32i++] = PWM_GET_CAPTURE_RISING_DATA(PWM, u32Ch);
    }

//    u16RisingTime = u32Count[1];
//    u16FallingTime = u32Count[0];
    u16HighPeriod = u32Count[1] - u32Count[2];
    u16LowPeriod = 0x10000 - u32Count[1];
    u16TotalPeriod = 0x10000 - u32Count[2];

    if((u16HighPeriod < 19199) || (u16HighPeriod > 19201)
            || (u16LowPeriod < 44799) || (u16LowPeriod > 44801)
            || (u16TotalPeriod < 63999) || (u16TotalPeriod > 64001))
        PWMTestPass = FAIL;
    else
        PWMTestPass = PASS;
}



void IEC60730_PWM_Test() {
	
		
		/* set PWM0 channel 0 output configuration */
     PWM_ConfigOutputChannel(PWM0, 0, 250, 30);

    /* Enable PWM Output path for PWM0 channel 0 */
     PWM_EnableOutput(PWM0, PWM_CH_0_MASK);

    /* Enable Timer for PWM0 channel 0 */
     PWM_Start(PWM0, PWM_CH_0_MASK);

		/* set PWM0 channel 2 capture configuration */
     PWM_ConfigCaptureChannel(PWM0, 2, 62, 0);

     /* Enable Timer for PWM0 channel 2 */
     PWM_Start(PWM0, PWM_CH_2_MASK);

		/* Enable Capture Function for PWM0 channel 2 */
     PWM_EnableCapture(PWM0, PWM_CH_2_MASK);

     /* Enable falling capture reload */
     PWM0->CAPCTL |= PWM_CAPCTL_FCRLDEN2_Msk;

     /* Wait until PWM0 channel 2 Timer start to count */
     while((PWM0->CNT[2]) == 0);

     /* Capture the Input Waveform Data */
     CalPeriodTime(PWM0, 2);

}



/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
