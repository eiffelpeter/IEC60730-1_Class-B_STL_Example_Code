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

extern uint32_t volatile ADCTestPass;

volatile uint32_t g_u32AdcIntFlag;
volatile uint32_t g_u32BandGapConvValue;

/**
  * @brief      Read Built-in Band-Gap conversion value
  * @param[in]  None
  * @return     Built-in Band-Gap conversion value
  * @details    This function is used to read Band-Gap conversion value.
  */
__STATIC_INLINE uint32_t FMC_ReadBandGap(void)
{
    FMC->ISPCMD = FMC_ISPCMD_READ_UID;            /* Set ISP Command Code */
    FMC->ISPADDR = 0x70u;                         /* Must keep 0x70 when read Band-Gap */
    FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;           /* Trigger to start ISP procedure */
#if ISBEN
    __ISB();
#endif                                            /* To make sure ISP/CPU be Synchronized */
    while(FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk) {}  /* Waiting for ISP Done */

    return FMC->ISPDAT;
}


void ADC_FunctionTest()
{
    int32_t  i32ConversionData, i32ConversionData_ldo;
    int32_t  i32BuiltInData;
		int32_t  AVdd;
		uint32_t u32ConvertResVldo;
//		int i32Err;

		/*
    printf("+----------------------------------------------------------------------------+\n");
    printf("|     ADC for calculate battery voltage( AVdd ) by using band-gap test       |\n");
    printf("+----------------------------------------------------------------------------+\n\n");

    printf("+----------------------------------------------------------------------+\n");
    printf("|   ADC clock source -> PCLK1  = 48 MHz                                |\n");
    printf("|   ADC clock divider          = 2                                     |\n");
    printf("|   ADC clock                  = 48 MHz / 2 = 24 MHz                   |\n");
    printf("|   ADC extended sampling time = 71                                    |\n");
    printf("|   ADC conversion time = 17 + ADC extended sampling time = 88         |\n");
    printf("|   ADC conversion rate = 24 MHz / 88 = 272.7 ksps                     |\n");
    printf("+----------------------------------------------------------------------+\n");
		*/

    /* Enable ADC converter */
    ADC_POWER_ON(ADC);

    /* Set input mode as single-end, Single mode, and select channel 29 (band-gap voltage) */
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE, BIT29);

    /* To sample band-gap precisely, the ADC capacitor must be charged at least 3 us for charging the ADC capacitor ( Cin )*/
    /* Sampling time = extended sampling time + 1 */
    /* 1/24000000 * (Sampling time) = 3 us */
    ADC_SetExtendSampleTime(ADC, 0, 71);

    /* Clear the A/D interrupt flag for safe */
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

    /* Enable the sample module interrupt.  */
    ADC_ENABLE_INT(ADC, ADC_ADF_INT);   // Enable sample module A/D interrupt.
    NVIC_EnableIRQ(ADC_IRQn);


    /* Reset the ADC interrupt indicator and trigger sample module to start A/D conversion */
    g_u32AdcIntFlag = 0;
    ADC_START_CONV(ADC);

    /* Wait ADC conversion done */
    while(g_u32AdcIntFlag == 0);

    /* Disable the A/D interrupt */
    ADC_DISABLE_INT(ADC, ADC_ADF_INT);

    /* Get the conversion result of the channel 29 */
    i32ConversionData = ADC_GET_CONVERSION_DATA(ADC, 29);


    /* Enable FMC ISP function to read built-in band-gap A/D conversion result*/
    SYS_UnlockReg();
    FMC_Open();
    i32BuiltInData = FMC_ReadBandGap();
		i32BuiltInData= (i32BuiltInData&0x00000FFF);
		
		/* Set input mode as single-end, Single mode, and select channel 0 (band-gap voltage) */
		ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE_CYCLE, BIT0);

    /* Clear the A/D interrupt flag for safe */
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

    /* Enable the sample module interrupt */
    ADC_ENABLE_INT(ADC, ADC_ADF_INT);  /* Enable sample module A/D interrupt. */
    NVIC_EnableIRQ(ADC_IRQn);

    /* Reset the ADC interrupt indicator and trigger sample module 0 to start A/D conversion */
    g_u32AdcIntFlag = 0;
    ADC_START_CONV(ADC);

    /* Wait ADC interrupt (g_u32AdcIntFlag will be set at IRQ_Handler function) */
    while(g_u32AdcIntFlag == 0);

    /* Disable the sample module interrupt */
    ADC_DISABLE_INT(ADC, ADC_ADF_INT);

    /* Get the conversion result of ADC channel 0 */
    i32ConversionData_ldo = ADC_GET_CONVERSION_DATA(ADC, 0);		
		
    /* Use Conversion result of Band-gap to calculating AVdd */
		/*
    printf("      AVdd         i32ConversionData                  \n");
    printf("   ---------- = -------------------------             \n");
    printf("      3072          i32BuiltInData                    \n");
    printf("                                                      \n");
    printf("AVdd =  3072 * i32ConversionData / i32BuiltInData     \n\n");
		*/
		
		AVdd = 3072*i32ConversionData/i32BuiltInData;
		u32ConvertResVldo = AVdd*i32ConversionData_ldo/4096;

		/* Print test result */
    if(u32ConvertResVldo>2100 || u32ConvertResVldo<1620)
    {
        printf("ADC test fail %d (1620~1980)\n", u32ConvertResVldo);
			ADCTestPass = FAIL;
    }
    else
    {
			ADCTestPass = PASS;
    }


}

void ADC_IRQHandler(void)
{
    g_u32AdcIntFlag = 1;
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT); /* Clear the A/D interrupt flag */
}


void IEC60730_AD_Test() {
	
		/* ADC function test */
    ADC_FunctionTest();
	
}



/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
