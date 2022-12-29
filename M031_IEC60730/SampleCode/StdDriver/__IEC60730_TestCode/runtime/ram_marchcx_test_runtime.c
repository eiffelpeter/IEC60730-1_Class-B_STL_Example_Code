/******************************************************************************
 * @file     ram_marchcx_test_runtime.c
 * @version  V0.10
 * $Revision: 4 $
 * $Date: 18/08/28 5:04p $
 * @brief    IEC60730 MarchC method for testing RAM
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <string.h>
#include "NuMicro.h"
#include "iec60730_test_runtime.h"

#ifdef __ARMCC_VERSION
extern uint32_t Image$$RW_STACK$$Base;
extern uint32_t Image$$RW_STACK$$ZI$$Length;

extern uint32_t Image$$RW_IRAM1$$Base;
extern uint32_t Image$$STACK_PTRN$$Base;
#endif

#ifdef __ICCARM__
extern uint32_t CSTACK$$Base;
extern uint32_t CSTACK$$Limit;

extern uint32_t RW_IRAM1$$Base;
extern uint32_t STACK_PTRN$$Base;
#endif

extern uint32_t __initial_sp;
static uint32_t s_u32RamCurrentAddr, s_u32RamEndAddr;

extern uint32_t volatile RAMStartAddr;
extern uint32_t volatile RAMEndAddr;
extern uint32_t volatile u32RunTimeInit;

__asm static void Disable_AllException(void)
{
		movs r0, #1
        msr PRIMASK, r0
		bx      lr		
}

__asm static void Enable_AllException(void)
{
		push	{r0-r7}
        movs    r0, #0
        msr     PRIMASK, r0
	;restore registers
		pop		{r0-r7}
		
	;Branch back
		bx      lr
}


static void IEC60730_RAM_MarchCX_Test_RunTime_Init(void)
{
#ifdef __ICCARM__
	s_u32RamCurrentAddr = (uint32_t) &RW_IRAM1$$Base;	
	s_u32RamEndAddr = (uint32_t) &STACK_PTRN$$Base;
#endif

#ifdef __ARMCC_VERSION
	s_u32RamCurrentAddr = (uint32_t) &Image$$RW_IRAM1$$Base;
	s_u32RamEndAddr = (uint32_t) &Image$$STACK_PTRN$$Base;
#endif
}

uint32_t IEC60730_RAM_MarchC_Test_RunTime(uint32_t* RAMTestPass)
{
	uint8_t * pu8CopyToSafeArea;
    uint8_t *pu8AddrPtr;
    uint32_t u32StartAddr, u32EndAddr;
	uint32_t u32Length = RAM_RUNTIME_TEST_LENGTH;
	
	if (!(u32RunTimeInit & RUNTIME_RAM_INTIT)) {
		u32RunTimeInit |= RUNTIME_RAM_INTIT;
		IEC60730_RAM_MarchCX_Test_RunTime_Init();
		*RAMTestPass = PASS;		
	}
	
	u32StartAddr = s_u32RamCurrentAddr;
	u32EndAddr = s_u32RamEndAddr;
	pu8AddrPtr = (uint8_t *) u32StartAddr;	
	
    /* disable interrupt for safe */
	Disable_AllException();

    /* if we need to backup ram data, safe ram reserved area is 
	   [0x200008000-0x10*2  0x200007FFF] */
	if (u32StartAddr == u32EndAddr - u32Length*2)
		pu8CopyToSafeArea = (uint8_t*) (u32EndAddr - u32Length);
	else
		pu8CopyToSafeArea = (uint8_t*) (u32EndAddr - u32Length*2);	
		
	memcpy(pu8CopyToSafeArea, pu8AddrPtr, u32Length);

    /* start ram test */
	RAMStartAddr = u32StartAddr;
	RAMEndAddr =  RAMStartAddr + u32Length;
	RAMEndAddr -= 1;
    *RAMTestPass = IEC60730_PartRamMarchC_WOM();

    /* restore the data */
    if(pu8CopyToSafeArea)
        memcpy(pu8AddrPtr, pu8CopyToSafeArea, u32Length);
	
    /* check if all ram have been tested (excluding safe reserved area) */
	u32StartAddr += u32Length;
	if (u32StartAddr >= u32EndAddr)
		u32RunTimeInit &= ~RUNTIME_RAM_INTIT;		

    /* based on Artisan's SRAM architecture, for overlap test */
	u32StartAddr -= u32Length/2;
	s_u32RamCurrentAddr = u32StartAddr;
	
    /* enable interrupt */
	Enable_AllException();
    return 0;
}

uint32_t IEC60730_RAM_MarchX_Test_RunTime(uint32_t* RAMTestPass)
{
	uint8_t * pu8CopyToSafeArea;
    uint8_t *pu8AddrPtr;
    uint32_t u32StartAddr, u32EndAddr;
	uint32_t u32Length = RAM_RUNTIME_TEST_LENGTH;

	if (!(u32RunTimeInit & RUNTIME_RAM_INTIT)) {
		u32RunTimeInit |= RUNTIME_RAM_INTIT;
		IEC60730_RAM_MarchCX_Test_RunTime_Init();
		*RAMTestPass = PASS;
	}

#ifdef OPT_ACCESS_TIME	
    PA4 = 1;
//    PA7 = 0;
#endif	
	
	u32StartAddr = s_u32RamCurrentAddr;
	u32EndAddr = s_u32RamEndAddr;
	pu8AddrPtr = (uint8_t *) u32StartAddr;	
	
    /* disable interrupt for safe */
	Disable_AllException();

    /* if we need to backup ram data, safe ram reserved area is 
	   [0x200008000-0x10*2  0x200007FFF] */
	if (u32StartAddr == u32EndAddr - u32Length*2)
		pu8CopyToSafeArea= (uint8_t*) (u32EndAddr - u32Length);
	else
		pu8CopyToSafeArea= (uint8_t*) (u32EndAddr - u32Length*2);	
		
	memcpy(pu8CopyToSafeArea, pu8AddrPtr, u32Length);

    /* start ram test */
	RAMStartAddr = u32StartAddr;
	RAMEndAddr =  RAMStartAddr + u32Length;
	RAMEndAddr -= 1;
	
    *RAMTestPass = IEC60730_PartRamMarchX_WOM();

    /* restore the data */
    if(pu8CopyToSafeArea)
        memcpy(pu8AddrPtr, pu8CopyToSafeArea, u32Length);
	
    /* check if all ram have been tested (excluding safe reserved area) */
	u32StartAddr += u32Length;
	if (u32StartAddr >= u32EndAddr)
		u32RunTimeInit &= ~RUNTIME_RAM_INTIT;		

    /* based on Artisan's SRAM architecture, for overlap test */
	u32StartAddr -= u32Length/2;
	s_u32RamCurrentAddr = u32StartAddr;

    /* enable interrupt */
	Enable_AllException();
    return 0;
}

