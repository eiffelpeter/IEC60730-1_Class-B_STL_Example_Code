
#include "../iec60730_test.h"

/* debug message */
#define rt_printf	printf
//#define rt_printf(...)


#ifndef CPU_IEC60730TEST_RUNTIME_H
#define CPU_IEC60730TEST_RUNTIME_H

/* ------------------------------------------------------------ */
/* runtime test constant declaration */
/* ------------------------------------------------------------ */
#define RUNTIME_CPUREG_INTIT		BIT0
#define RUNTIME_PC_INTIT			BIT1
#define RUNTIME_STACK_INTIT			BIT2
#define RUNTIME_INT_INTIT			BIT3
#define RUNTIME_RAM_INTIT			BIT4
#define RUNTIME_FLASH_INTIT			BIT5
/* test cycle based on systick or timer0 */
#define RUNTIME_TEST_BASED_TIMER0
#ifndef RUNTIME_TEST_BASED_TIMER0
#define RUNTIME_TEST_BASED_SYSTICK
#endif

/* each item test cycle */
#define CPUREG_TEST_CYCLE			3		// 3*test_cycle
#define PC_TEST_CYCLE				5		// 5*test_cycle
#define STACK_TEST_CYCLE			10				
#define RAM_TEST_CYCLE				5
#define ROM_TEST_CYCLE				1
#define INT_TEST_CYCLE				30
#define REST_WDT_CYCLE				3


/* interrupt test */
/* maximum wait time, for exa
	*/
#define TIMER_MAXI_WAIT_TIME		0x5		

/* RAM & ROM tested length each time */
#define RAM_RUNTIME_TEST_LENGTH		0x20	// based on Artisan SRAM achitecture
#define ROM_RUNTIME_TEST_LENGTH		0x20

/* RAM runtime test methodology */
#define RAM_RUNTIME_MARCHC_WOM
#ifndef RAM_RUNTIME_MARCHC_WOM
#define RAM_RUNTIME_MARCHX_WOM
#endif

/* function declaration for runtime test */
void IEC60730_DoRunTimeIntial(void);
void IEC60730_CPU_Reg_Test_RunTime(void);
uint32_t IEC60730_PC_Test_RunTime(uint32_t*);
uint32_t IEC60730_RAM_MarchC_Test_RunTime(uint32_t*);
uint32_t IEC60730_RAM_MarchX_Test_RunTime(uint32_t*);
uint32_t IEC60730_Flash_Test_RunTime(uint32_t*);
uint32_t IEC60730_Interrupt_Test_RunTime(uint32_t*);
uint32_t IEC60730_Stack_Test_RunTime(uint32_t*);
uint32_t IEC60730_PartRamMarchC_WOM(void);
uint32_t IEC60730_PartRamMarchX_WOM(void);
#endif
