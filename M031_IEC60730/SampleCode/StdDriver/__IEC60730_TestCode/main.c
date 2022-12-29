/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * $Revision: 3 $
 * $Date: 18/08/28 5:04p $
 * @brief    Show the usage of clock fail detector and clock frequency monitor function.
 * @note
 * Copyright (C) 2013~2015 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include "stdio.h"
#include "NuMicro.h"
#include "iec60730_test.h"
#include "iec60730_test_application.h"


#ifdef __ARMCC_VERSION
extern uint32_t Image$$RW_IRAM1$$Base;
#endif

#ifdef __ICCARM__
extern uint32_t RW_IRAM1$$Base;
#endif


uint32_t volatile CPUTestPass=FAIL;
uint32_t volatile WDTTestPass;
uint32_t volatile PCTestPass;
uint32_t volatile IntTestPass;
uint32_t volatile StackTestPass;
uint32_t volatile WDTTestPass;
uint32_t volatile RAMTestPass=FAIL;
uint32_t volatile ROMTestPass;
uint32_t volatile GPIOTestPass;
uint32_t volatile SPITestPass;
uint32_t volatile PWMTestPass;
uint32_t volatile ADCTestPass;

uint32_t volatile RAMStartAddr;
uint32_t volatile RAMEndAddr;

extern uint32_t volatile STACK_TEST_PTRN[];

uint8_t RAM_backup[0x300] __attribute__((at(0x20001000)));

static uint32_t volatile systick = 1;
static uint32_t volatile ledtick = 0;

void TMR1_IRQHandler(void)
{
    systick++;
    printf("TMR1_IRQHandler\n");
    /* clear timer interrupt flag */
    TIMER_ClearIntFlag(TIMER1);
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable HIRC, HXT and LXT clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
    CLK_EnableXtalRC(CLK_PWRCTL_LXTEN_Msk);
    CLK_EnableXtalRC(CLK_PWRCTL_LIRCEN_Msk);	

    /* Wait for HIRC, HXT and LXT clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);	

    /* Select HCLK clock source as HIRC and and HCLK clock divider as 1 */
#ifdef HCLK_FROM_HXT
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT, CLK_CLKDIV0_HCLK(1));	
#endif 
#ifdef HCLK_FROM_HIRC
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));	
#endif 
    /* Set core clock as PLL_CLOCK from PLL */
#ifdef HCLK_FROM_PLL	
    CLK_SetCoreClock(PLL_CLOCK);
#endif

    /* Enable perpherial module clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(CRC_MODULE);
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(WDT_MODULE);
    CLK_EnableModuleClock(WWDT_MODULE);
    CLK_EnableModuleClock(SPI0_MODULE);
		CLK_EnableModuleClock(ADC_MODULE);
    CLK_EnableModuleClock(PWM0_MODULE);

	/* select WDT clock source */
#ifdef WDT_FROM_LXT	
    CLK_SetModuleClock(WDT_MODULE, CLK_CLKSEL1_WDTSEL_LXT, 0);
#endif
#ifdef WDT_FROM_HCLK_DIV2048	
    CLK_SetModuleClock(WDT_MODULE, CLK_CLKSEL1_WDTSEL_HCLK_DIV2048, 0);
#endif
#ifdef WDT_FROM_LIRC	
    CLK_SetModuleClock(WDT_MODULE, CLK_CLKSEL1_WDTSEL_LIRC, 0);
#endif

	/* select WWDT clock source */
#ifdef WWDT_FROM_PCLK0_DIV2048	
    CLK_SetModuleClock(WWDT_MODULE, CLK_CLKSEL1_WWDTSEL_HCLK_DIV2048, 0);
#endif
#ifdef WWDT_FROM_LIRC	
    CLK_SetModuleClock(WWDT_MODULE, CLK_CLKSEL1_WWDTSEL_LIRC, 0);
#endif

    /* Select UART module clock source as HIRC and UART module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT, CLK_CLKDIV0_UART0(1));
	
    /* Select Timer0 module clock source */	
#ifdef TIMER0_FROM_HIRC
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, 0);
#endif
#ifdef TIMER0_FROM_LIRC
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_LIRC, 0);
#endif

    //CLK_EnableModuleClock(TMR1_MODULE); // peter
    //CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HIRC, 0);
	
    SystemCoreClockUpdate();
	
		/* Select PCLK1 as the clock source of SPI0 */
    CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL2_SPI0SEL_PCLK1, MODULE_NoMsk);
		/* Switch ADC clock source to HIRC, set divider to 2, ADC clock is 48/2 MHz */
    CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL2_ADCSEL_PCLK1, CLK_CLKDIV0_ADC(2));
		CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL2_PWM0SEL_PCLK0, 0);
	
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD=PB.12 and TXD=PB.13 */
	SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
	SYS->GPB_MFPH |= SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD;
	
		/* Setup SPI0 multi-function pins */
    /* PA.3 is SPI0_SS,   PA.2 is SPI0_CLK,
       PA.1 is SPI0_MISO, PA.0 is SPI0_MOSI*/
  SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA3MFP_Msk |
                                       SYS_GPA_MFPL_PA2MFP_Msk |
                                       SYS_GPA_MFPL_PA1MFP_Msk |
                                       SYS_GPA_MFPL_PA0MFP_Msk)) |
                  (SYS_GPA_MFPL_PA3MFP_SPI0_SS |
                   SYS_GPA_MFPL_PA2MFP_SPI0_CLK |
                   SYS_GPA_MFPL_PA1MFP_SPI0_MISO |
                   SYS_GPA_MFPL_PA0MFP_SPI0_MOSI);
									 
	/* Set PB multi-function pins for PWM0 Channel 0 and 2 */
  SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB5MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk)) |
                  (SYS_GPB_MFPL_PB5MFP_PWM0_CH0 | SYS_GPB_MFPL_PB3MFP_PWM0_CH2);
									
	/* Set PB.0 to input mode */
  GPIO_SetMode(PB, BIT0, GPIO_MODE_INPUT);
  /* Configure the PB.0 ADC analog input pins. */
  SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB0MFP_Msk)) |
                  (SYS_GPB_MFPL_PB0MFP_ADC_CH0 );
									
  GPIO_SetMode(PB, BIT14, GPIO_MODE_OUTPUT);
																
}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART module */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    static uint32_t delay = 5000000;
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

	/* enable clock fail detector */
    //IEC60730_Clk_Fail_Detect();

#ifdef OPT_ACCESS_TIME_CHECK
    SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA0MFP_Msk | SYS_GPA_MFPL_PA3MFP_Msk);	
		PA->MODE &= ~GPIO_MODE_MODE0_Msk;	
		PA->MODE &= ~GPIO_MODE_MODE3_Msk;		
		PA->MODE |= (1<<GPIO_MODE_MODE0_Pos);	// output mode
		PA->MODE |= (1<<GPIO_MODE_MODE3_Pos);	// output mode	
    PA0 = 0;
    PA3 = 0;	
#endif
	
	/* enable Brown Out Detector and LVR reset */
    SYS_UnlockReg();
		SYS->BODCTL |= (SYS_BODCTL_BODRSTEN_Msk | SYS_BODCTL_LVREN_Msk);
    SYS_LockReg();	

    /* WDT & WWDT Reset Test */
#if 0
    printf("WDT & WWDT Reset Test ...");	
    printf("ICE will hang up while runing this test program.");		
		IEC60730_WDT_WWDT_Reset_Test((uint32_t*)&WDTTestPass);
    if(WDTTestPass)
		printf("Pass !!\n");
    else {
		printf("Fail !!\n");
		while(1);
	}
#endif

   
    // set LED on 
    PB14 = 0;



//    // begin peter
//    /* Set timer frequency to 1HZ */
//    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1);
//
//    /* Enable timer interrupt */
//    TIMER_EnableInt(TIMER1);
//    NVIC_EnableIRQ(TMR1_IRQn);
//
//    /* Start Timer 1 */
//    TIMER_Start(TIMER1);
//    // end peter


    /* CPU Registers Test */
    CPUTestPass = 0;
    printf("CPU Register Test (BIST)...");
    IEC60730_CPU_Reg_Test();
    if(CPUTestPass) printf("Pass !!\n");
    else printf("Fail !!\n");
    
    /* Program Counter Test */
    printf("Program Counter Test ...");
    IEC60730_PC_Test((uint32_t*)&PCTestPass);
    if(PCTestPass) printf("Pass !!\n");
    else printf("Fail !!\n");

    /* WDT Test */
    printf("WDT Test ...");
    IEC60730_WDT_Test((uint32_t*)&WDTTestPass);
    if(WDTTestPass)
		printf("Pass !!\n");
    else {
		printf("Fail !!\n");
		while(1);
	}

    /* Interrupt Test */
    printf("Interrupt Test ...");
    IEC60730_Interrupt_Test((uint32_t*)&IntTestPass);
    if(IntTestPass)
		printf("Pass !!\n");
    else {
		printf("Fail !!\n");
		while(1);
	}

    /* RAM Test (MarchC) */
#ifdef RAM_STARTUP_MARCHC_WOM	
	RAMStartAddr = (uint32_t)&Image$$RW_IRAM1$$Base;
	RAMEndAddr =  RAMStartAddr + RAM_LENGTH;
	RAMEndAddr -= 1;
    if(IEC60730_FullRamMarchC_WOM())	
		printf("Pass !!\n");
    else {
		printf("Fail !!\n");
		while(1);
	}
#endif

    /* RAM Test (MarchX) */
#ifdef RAM_STARTUP_MARCHX_WOM
	RAMStartAddr = (uint32_t)&Image$$RW_IRAM1$$Base;    // 0x20000000
	RAMEndAddr =  RAMStartAddr + RAM_LENGTH;     // peter 0x20001000
	RAMEndAddr -= 1;
    if(IEC60730_FullRamMarchX_WOM())	
		printf("RAM Test (MarchX) Pass !!\n");
    else {
		printf("RAM Test (MarchX) Fail !!\n");
		while(1);
	}
#endif

    /* Flash Test */
    /* After building test code, CRC32 checksum value must be given to 
	"__Check_Sum" symbol which is defined in startup_M031Series.s, and then 
	rebuild code again */
//#ifndef OPT_ACCESS_TIME_CHECK		
//    printf("Flash Test ...");
//    IEC60730_Flash_Test((uint32_t*)&ROMTestPass);
//    if(ROMTestPass)
//		printf("Pass !!\n");
//    else {
//		printf("Fail !!\n");
//		while(1);
//	}
//#endif

  /* Run Time Test initial */
	IEC60730_DoRunTimeIntial();
	IEC60730_DoRunApplicationIntial();

	while (1)
	{

//      if(systick != ledtick)
//      {
//          printf("diff:%d\n", systick - ledtick);
//      }

        
        /* Run Application Test mainloop */		
        IEC60730_DoRunApplication();
		
        /* Run Time Test mainloop */
        IEC60730_DoRunTimeChecks();
        

        for ( delay = 10000000 ; delay > 0; delay--);
        printf("\n");
        
        //printf("PB14:%d\n",PB14);
//      if ((systick - ledtick) >= 0x1) {
//            GPIO_SetMode(PB, BIT14, GPIO_MODE_OUTPUT);
            PB14 = !PB14;
//          ledtick = systick;
//          //printf("test\n");
//      }
	}

}
