
#include "../iec60730_test.h"

/* debug message */
#define rt_printf	printf
//#define rt_printf(...)
#define RUNTIME_TEST_BASED_TIMER0

#ifndef CPU_IEC60730TEST_RUNTIME_H
#define CPU_IEC60730TEST_RUNTIME_H

/* ------------------------------------------------------------ */
/* application test constant declaration */
/* ------------------------------------------------------------ */


/* each item test cycle */
#define GPIO_TEST_CYCLE			2		// 3*test_cycle
#define SPI_TEST_CYCLE			2		// 5*test_cycle
#define PWM_TEST_CYCLE			2				
#define ADC_TEST_CYCLE			2


/* function declaration for runtime test */
void IEC60730_DoRunApplicatonIntial(void);
void IEC60730_IO_Test (void);
void IEC60730_SPI_Test (void);
void IEC60730_PWM_Test (void);
void IEC60730_AD_Test (void);

#endif
