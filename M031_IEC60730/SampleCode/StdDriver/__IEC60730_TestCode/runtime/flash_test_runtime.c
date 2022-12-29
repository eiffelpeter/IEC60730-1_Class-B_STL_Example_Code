/******************************************************************************
 * @file     flash_test_runtime.c
 * @version  V0.10
 * $Revision: 3 $
 * $Date: 18/08/28 5:04p $
 * @brief    IEC60730 Intrnal Flash Test Using CRC Method
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "stdio.h"
#include "NuMicro.h"
#include "iec60730_test_runtime.h"

static uint32_t s_u32RomCurrentAddr, s_u32RomEndAddr;
static uint32_t volatile s_u32HWCRC = 0, s_u32SWCRC = 0;
static uint8_t volatile s_bIsHWCRCCalculated = FALSE;

extern volatile uint32_t u32seed;
extern volatile uint32_t u32TotalByteCnt;
extern uint32_t volatile u32RunTimeInit;

extern uint32_t CRC_SWResult(uint8_t *string, int8_t IsInput1sCOM, int8_t IsInputRVS, int8_t IsCRC1sCOM, int8_t IsCRCRVS);

void IEC60730_Flash_Test_RunTime_Init(void)
{
#ifdef __ICCARM__
	s_u32RomCurrentAddr = (uint32_t) &ER_IROM1$$Base;	
#endif

#ifdef __ARMCC_VERSION
	s_u32RomCurrentAddr = (uint32_t) &Image$$ER_IROM1$$Base;
#endif

	s_u32RomEndAddr = s_u32RomCurrentAddr + ROM_LENGTH;


    
    //printf("ROM test init s_u32RomCurrentAddr:0x%x s_u32RomEndAddr:0x%x \n", s_u32RomCurrentAddr, s_u32RomEndAddr);
	/* init HW CRC */
#ifdef CRC_COMAPRE_WITH_HW	
	CRC->CTL = 0;
	CRC->CTL = CRC_CTL_CRCEN_Msk;
	CRC->CTL |= CRC_32;
	CRC->CTL |= CRC_CPU_WDATA_32;
	CRC_Open(CRC_32, 0, 0xFFFFFFFF, CRC_CPU_WDATA_32);
#endif
	
	/* init SW CRC32 seed */
#ifdef CRC_COMAPRE_WITH_SW	
	s_u32SWCRC = 0xFFFFFFFF;
#endif
}

uint32_t IEC60730_Flash_Test_RunTime(uint32_t* ROMTestPass)
{
	uint32_t u32Length = ROM_RUNTIME_TEST_LENGTH;
    uint32_t u32StartAddr;
#ifdef CRC_COMAPRE_WITH_HW
    uint32_t ii, u32EndAddr;
#endif

	if (!(u32RunTimeInit & RUNTIME_FLASH_INTIT)) {
		u32RunTimeInit |= RUNTIME_FLASH_INTIT;
		IEC60730_Flash_Test_RunTime_Init();
		*ROMTestPass = PASS;
	}

    //printf("ROM test s_u32RomCurrentAddr:0x%x s_u32RomEndAddr:0x%x \n", s_u32RomCurrentAddr, s_u32RomEndAddr);

	

	u32StartAddr = s_u32RomCurrentAddr;
    
	/* calculate SW CRC checksum */	
#ifdef CRC_COMAPRE_WITH_SW

    // do checksum
//  do
//  {
//      u32TotalByteCnt = u32Length;
//      u32seed = s_u32SWCRC;
//      s_u32SWCRC = CRC_SWResult((uint8_t*)u32StartAddr,0,0,0,0);
//      //printf("ROM test s_u32SWCRC:0x%x u32StartAddr:0x%x \n", s_u32SWCRC, u32StartAddr);
//      u32StartAddr += u32Length;
//
//  } while(u32StartAddr < s_u32RomEndAddr);

        u32TotalByteCnt = u32Length = ROM_LENGTH;
        u32seed = 0xFFFFFFFF;	
        s_u32SWCRC = CRC_SWResult((uint8_t*)u32StartAddr,0,0,0,0);
        //printf("ROM test s_u32SWCRC:0x%x u32StartAddr:0x%x \n", s_u32SWCRC, u32StartAddr);
        u32StartAddr += u32Length;


    /* check if end address */	
	if (u32StartAddr >= s_u32RomEndAddr)
		u32RunTimeInit &= ~RUNTIME_FLASH_INTIT;		

	s_u32RomCurrentAddr = u32StartAddr;
	
    /* compare the CRC value */
	if (!(u32RunTimeInit & RUNTIME_FLASH_INTIT)) {	
		if(s_u32SWCRC != __Check_Sum)
        {
            printf("ROM __Check_Sum at address 0x%x, bin size:%d(0x%x)\n", &__Check_Sum, ROM_LENGTH, ROM_LENGTH );
            printf("ROM test fail1 expect:0x%08x, but __Check_Sum:0x%08x\n\r", s_u32SWCRC, __Check_Sum);
            *ROMTestPass = FAIL;
        }
		else
        {
            //printf("ROM test pass1 0x%x 0x%x\n\r", s_u32SWCRC, __Check_Sum);
            *ROMTestPass = PASS;
        }
	}
#endif

	/* calculate HW CRC checksum */	
#ifdef CRC_COMAPRE_WITH_HW
	u32EndAddr = u32StartAddr + u32Length;	
	for(ii=u32StartAddr; ii < u32EndAddr; ii += 4)
		CRC_WRITE_DATA(inpw(ii));
		
	/* check if end address */	
	u32StartAddr += u32Length;
	if (u32StartAddr >= s_u32RomEndAddr) {
		s_u32HWCRC = CRC_GetChecksum();
		u32RunTimeInit &= ~RUNTIME_FLASH_INTIT;				
	}

	s_u32RomCurrentAddr = u32StartAddr;
	
    /* compare the CRC value */
	if (!(u32RunTimeInit & RUNTIME_FLASH_INTIT)) {		
		if(s_u32HWCRC != __Check_Sum)
        {
            printf("ROM test fail2 0x%x 0x%x\n\r", s_u32SWCRC, __Check_Sum);
                *ROMTestPass = FAIL;
        }
		else
			*ROMTestPass = PASS;
	}
#endif

	return 0;
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
