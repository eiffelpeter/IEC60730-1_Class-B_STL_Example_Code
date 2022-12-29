/******************************************************************************
 * @file     flash_test.c
 * @version  V0.10
 * $Revision: 2 $
 * $Date: 18/06/15 2:23p $
 * @brief    IEC60730 Intrnal Flash Test Using CRC Method
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "stdio.h"
#include "NuMicro.h"
#include "../iec60730_test.h"

typedef struct {
    const char mode[10];
    const uint32_t polynom;
    uint32_t seed;
    uint32_t mask;
    uint32_t input_com;
    uint32_t input_rvs;
    uint32_t output_com;
    uint32_t output_rvs;
    uint32_t transfer_mode;
} CRC_MODE_ATTR_T;

const uint32_t ModeTable[] = {
    CRC_CCITT,
    CRC_8,
    CRC_16,
    CRC_32,
};

typedef enum {
    E_CRCCCITT = 0,
    E_CRC8,
    E_CRC16,
    E_CRC32,
} E_CRC_MODE;

const CRC_MODE_ATTR_T CRCAttr[] = 
{
    {"CRC-CCITT", 0x1021,       0xFFFF      , 0xFFFF,       0, 0, 0, 0, 0},
    {"CRC-8    ", 0x7,          0xFF        , 0xFF,         0, 0, 0, 0, 0},
    {"CRC-16   ", 0x8005,       0xFFFF      , 0xFFFF,       0, 0, 0, 0, 0},
    {"CRC-32   ", 0x04C11DB7,   0xFFFFFFFF  , 0xFFFFFFFF,   0, 0, 0, 0, 0},
};

static int8_t order;
static uint32_t polynom;
static uint32_t crcinit;
static uint32_t crcxor;
static int8_t refin;
static int8_t refout;

static uint32_t crcmask;
static uint32_t crchighbit;
//static uint32_t crcinit_direct;
static uint32_t crcinit_nondirect;

volatile uint32_t u32TotalByteCnt;
volatile uint32_t u32HWCRC32, u32seed;

static uint32_t reflect(uint32_t crc, int bitnum)
{
    // reflects the lower 'bitnum' bits of 'crc'
    uint32_t i, j=1, crcout=0;

    for (i=(uint32_t)1<<(bitnum-1); i; i>>=1) 
    {
        if (crc & i) 
            crcout |= j;
        j <<= 1;
    }

    return (crcout);
}

static uint32_t crcbitbybit(uint8_t *p, uint32_t len, int8_t IsInput1sCOM, int8_t IsCRC1sCOM) 
{
    /* bit by bit algorithm with augmented zero bytes.
     suited for polynom orders between 1...32. */
    uint32_t i, j, k, bit;
    uint32_t crc = crcinit_nondirect;

    for (i=0; i<len; i++) 
    {
        k = (uint32_t)*p++;
        if (IsInput1sCOM)
        {
            k = (~k) & 0xFF;
        }

        if (refin) 
            k = reflect(k, 8);

        for (j=0x80; j; j>>=1) 
        {
            bit = crc & crchighbit;
            crc <<= 1;

            if (k & j) 
                crc |= 1;

            if (bit) 
                crc ^= polynom;
        }
    }	

    for (i=0; i<order; i++) 
    {
        bit = crc & crchighbit;
        crc <<= 1;
        if (bit) 
            crc ^= polynom;
    }

    if (refout) 
        crc = reflect(crc, order);

    crc ^= crcxor;
    crc &= crcmask;

    return (crc);
}

uint32_t CRC_SWResult(uint8_t *string, int8_t IsInput1sCOM, int8_t IsInputRVS, int8_t IsCRC1sCOM, int8_t IsCRCRVS)
{
    uint8_t i;
    uint32_t bit, crc, crc_result;    
	volatile uint8_t uCRCMode;
//	volatile uint32_t u32SWCRC32;

	/* CRC in CRC32 mode */
	order = 32;
	uCRCMode = E_CRC32;
    polynom = CRCAttr[uCRCMode].polynom;
//    crcinit = CRCAttr[uCRCMode].seed;
	crcinit = u32seed;
    
    refin = IsInputRVS;
    refout = IsCRCRVS;

    /* at first, compute constant bit masks for whole CRC and CRC high bit */
    crcmask = ((((uint32_t)1 << (order-1)) - 1) << 1) | 1;
    crchighbit = (uint32_t)1 << (order-1);
    crcxor = (IsCRC1sCOM)? crcmask:0;

    /* check parameters */
    if (polynom != (polynom & crcmask)) 
        return (0);

    if (crcinit != (crcinit & crcmask)) 
        return (0);

    if (crcxor != (crcxor & crcmask)) 
        return (0);
	
    /* compute missing initial CRC value ... direct always is 1 */
    crc = crcinit;
    for (i=0; i<order; i++) 
    {
        bit = crc & 1;
        if (bit) 
            crc ^= polynom;
        crc >>= 1;

        if (bit) 
            crc |= crchighbit;
    }	
    crcinit_nondirect = crc;
    crc_result = crcbitbybit((uint8_t *)string, u32TotalByteCnt, IsInput1sCOM, IsCRC1sCOM);
    return crc_result;
}

#if 0
uint32_t GetFMCChecksum(uint32_t u32Address, uint32_t u32Size)
{
    uint32_t u32CHKS;

    FMC_ENABLE_ISP();
    u32CHKS = FMC_GetCheckSum(u32Address, u32Size);

    return u32CHKS;
}
#endif

uint32_t IEC60730_Flash_Test(uint32_t* ROMTestPass)
{
	uint32_t u32StartAddr, u32Length = ROM_LENGTH;

#ifdef CRC_COMAPRE_WITH_SW
	volatile uint32_t u32SWCRC32;
#endif
#ifdef CRC_COMAPRE_WITH_HW	
	uint32_t ii, u32EndAddr;	
#endif

#ifdef __ICCARM__
	u32StartAddr = (uint32_t) &ER_IROM1$$Base;	
#endif
#ifdef __ARMCC_VERSION
	u32StartAddr = (uint32_t) &Image$$ER_IROM1$$Base;
#endif
	
	u32Length = ROM_LENGTH;
		
    /*  case a: software CRC32 checksum */
#ifdef CRC_COMAPRE_WITH_SW	
	u32TotalByteCnt = u32Length;
    u32SWCRC32 = 0xFFFFFFFF;

	u32seed = u32SWCRC32;
	u32SWCRC32 = CRC_SWResult((uint8_t*)u32StartAddr,0,0,0,0);
	
    /* compare CRC32 value */	
    if(u32SWCRC32 != __Check_Sum)
    {
        printf("ROM __Check_Sum at address 0x%x, bin size:%d(0x%x)\n", &__Check_Sum, ROM_LENGTH, ROM_LENGTH );
        printf("ROM test fail3 expect:0x%08x, but __Check_Sum:0x%08x\n\r", u32SWCRC32, __Check_Sum);        
        *ROMTestPass = FAIL;
    }
    else
        *ROMTestPass = PASS;
#endif

    /*  case b: hardware CRC32 checksum */
#ifdef CRC_COMAPRE_WITH_HW		
    CRC->CTL = 0;
    CRC->CTL = CRC_CTL_CRCEN_Msk;
    CRC->CTL |= CRC_32;
    CRC->CTL |= CRC_CPU_WDATA_32;
    CRC_Open(CRC_32, 0, 0xFFFFFFFF, CRC_CPU_WDATA_32);
	
    /* start to execute CRC-CRC32 operation */
	u32EndAddr = u32StartAddr + u32Length;		
    for(ii=u32StartAddr; ii < u32EndAddr; ii += 4)
        CRC_WRITE_DATA(inpw(ii));

    /* compare CRC32 value */
    u32HWCRC32 = CRC_GetChecksum();
    if(u32HWCRC32 != __Check_Sum)
    {
        printf("ROM test fail4 0x%x 0x%x\n\r", s_u32SWCRC, __Check_Sum);
            *ROMTestPass = FAIL;
    }
    else
        *ROMTestPass = PASS;
#endif
  
	return 0 ;
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
