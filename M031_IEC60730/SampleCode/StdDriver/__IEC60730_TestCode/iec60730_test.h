
#ifndef CPU_IEC60730TEST_H
#define CPU_IEC60730TEST_H

#define FAIL    0
#define PASS    1

#ifdef __ARMCC_VERSION
extern uint32_t Image$$ER_IROM1$$Base;
#endif

#ifdef __ICCARM__
extern uint32_t ER_IROM1$$Base;
#endif

extern const uint32_t __Check_Sum;

//#define OPT_ACCESS_TIME_CHECK		

/* PLL settings */
#define PLLCTL_SETTING  CLK_PLLCTL_72MHz_HXT
#define PLL_CLOCK       72000000

/* HCLK clock source selection */
//#define HCLK_FROM_HXT
#define HCLK_FROM_HIRC
//#define HCLK_FROM_PLL

/* Timer0 clock source selection */
//#define TIMER0_FROM_LIRC
#ifndef TIMER0_FROM_LIRC
#define TIMER0_FROM_HIRC
#endif

/* WDT clock source selection */
//#define WDT_FROM_LXT
//#define WDT_FROM_HCLK_DIV2048
#define WDT_FROM_LIRC

/* WWDT clock source selection */
//#define WWDT_FROM_HCLK_DIV2048
#define WWDT_FROM_LIRC

/* for PC test */
#define PC_RETURNVALUE 			10

/* for Interrupt test */
#define SYSTICK_TIMEBASE		100				// 100 uS
#define TIMER0_TIMEBASE			10000			// 10,000 uS
#define TIMER_DEVATION			20				// 20%

/* for Stack test */
#define STACK_OVERRUN_PTRN0		0x12345432
#define STACK_OVERRUN_PTRN1		0x56789876
#define STACK_OVERRUN_PTRN2		0xfedc9876
#define STACK_OVERRUN_PTRN3		0xabcdef12

/* for RAM test */
#define RAM_LENGTH				0x00001000
//#define RAM_STARTUP_MARCHC_WOM
#ifndef RAM_STARTUP_MARCHC_WOM
#define RAM_STARTUP_MARCHX_WOM
#endif

/* for ROM test */
/* Constants necessary for Flash CRC calculation, byte-aligned addresses */
#define ROM_START 				((uint32_t *)&Image$$ER_IROM1$$Base)	
#define ROM_END   				((uint32_t *)&__Check_Sum)
#define ROM_LENGTH  			((uint32_t)ROM_END - (uint32_t)ROM_START)

/* CRC checsum compare */
//#define CRC_COMAPRE_WITH_HW
#ifndef CRC_COMAPRE_WITH_HW
#define CRC_COMAPRE_WITH_SW
#endif

/* function declaration for Startup test */
void IEC60730_Clk_Fail_Detect(void);
void IEC60730_CPU_Reg_Test(void);
uint32_t IEC60730_PC_Test(uint32_t* PCTestPass);
uint32_t IEC60730_Interrupt_Test(uint32_t* INTTestPass);
uint32_t IEC60730_WDT_WWDT_Reset_Test(uint32_t* WDTTestPass);
uint32_t IEC60730_WDT_Test(uint32_t* WDTTestPass);
void IEC60730_RamMarchC(void);
void IEC60730_RamMarchX(void);
uint32_t IEC60730_FullRamMarchC_WOM(void);
uint32_t IEC60730_FullRamMarchX_WOM(void);
void IEC60730_RamMarchX_WOM(void);
uint32_t IEC60730_Flash_Test(uint32_t* ROMTestPass);

/* function declaration for Application test */
void IEC60730_DoRunApplication(void);
void IEC60730_DoRunApplicationIntial(void);

/* function declaration for Runtime test */
void IEC60730_DoRunTimeChecks(void);
void IEC60730_DoRunTimeIntial(void);
#endif
