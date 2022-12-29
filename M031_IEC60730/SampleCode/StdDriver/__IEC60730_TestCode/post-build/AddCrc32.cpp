#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"

typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;

#define MAX_NEW_BIN_SIZE (512 * 1024) //KB
volatile uint8_t NewBinArray[MAX_NEW_BIN_SIZE];

#define CHECKSUM_OFFSET_FROM_TAIL ( 16 * 15 )

//static const unsigned long crcTable[256] = {
//    0x00000000,0x77073096,0xEE0E612C,0x990951BA,0x076DC419,0x706AF48F,0xE963A535,
//    0x9E6495A3,0x0EDB8832,0x79DCB8A4,0xE0D5E91E,0x97D2D988,0x09B64C2B,0x7EB17CBD,
//    0xE7B82D07,0x90BF1D91,0x1DB71064,0x6AB020F2,0xF3B97148,0x84BE41DE,0x1ADAD47D,
//    0x6DDDE4EB,0xF4D4B551,0x83D385C7,0x136C9856,0x646BA8C0,0xFD62F97A,0x8A65C9EC,
//    0x14015C4F,0x63066CD9,0xFA0F3D63,0x8D080DF5,0x3B6E20C8,0x4C69105E,0xD56041E4,
//    0xA2677172,0x3C03E4D1,0x4B04D447,0xD20D85FD,0xA50AB56B,0x35B5A8FA,0x42B2986C,
//    0xDBBBC9D6,0xACBCF940,0x32D86CE3,0x45DF5C75,0xDCD60DCF,0xABD13D59,0x26D930AC,
//    0x51DE003A,0xC8D75180,0xBFD06116,0x21B4F4B5,0x56B3C423,0xCFBA9599,0xB8BDA50F,
//    0x2802B89E,0x5F058808,0xC60CD9B2,0xB10BE924,0x2F6F7C87,0x58684C11,0xC1611DAB,
//    0xB6662D3D,0x76DC4190,0x01DB7106,0x98D220BC,0xEFD5102A,0x71B18589,0x06B6B51F,
//    0x9FBFE4A5,0xE8B8D433,0x7807C9A2,0x0F00F934,0x9609A88E,0xE10E9818,0x7F6A0DBB,
//    0x086D3D2D,0x91646C97,0xE6635C01,0x6B6B51F4,0x1C6C6162,0x856530D8,0xF262004E,
//    0x6C0695ED,0x1B01A57B,0x8208F4C1,0xF50FC457,0x65B0D9C6,0x12B7E950,0x8BBEB8EA,
//    0xFCB9887C,0x62DD1DDF,0x15DA2D49,0x8CD37CF3,0xFBD44C65,0x4DB26158,0x3AB551CE,
//    0xA3BC0074,0xD4BB30E2,0x4ADFA541,0x3DD895D7,0xA4D1C46D,0xD3D6F4FB,0x4369E96A,
//    0x346ED9FC,0xAD678846,0xDA60B8D0,0x44042D73,0x33031DE5,0xAA0A4C5F,0xDD0D7CC9,
//    0x5005713C,0x270241AA,0xBE0B1010,0xC90C2086,0x5768B525,0x206F85B3,0xB966D409,
//    0xCE61E49F,0x5EDEF90E,0x29D9C998,0xB0D09822,0xC7D7A8B4,0x59B33D17,0x2EB40D81,
//    0xB7BD5C3B,0xC0BA6CAD,0xEDB88320,0x9ABFB3B6,0x03B6E20C,0x74B1D29A,0xEAD54739,
//    0x9DD277AF,0x04DB2615,0x73DC1683,0xE3630B12,0x94643B84,0x0D6D6A3E,0x7A6A5AA8,
//    0xE40ECF0B,0x9309FF9D,0x0A00AE27,0x7D079EB1,0xF00F9344,0x8708A3D2,0x1E01F268,
//    0x6906C2FE,0xF762575D,0x806567CB,0x196C3671,0x6E6B06E7,0xFED41B76,0x89D32BE0,
//    0x10DA7A5A,0x67DD4ACC,0xF9B9DF6F,0x8EBEEFF9,0x17B7BE43,0x60B08ED5,0xD6D6A3E8,
//    0xA1D1937E,0x38D8C2C4,0x4FDFF252,0xD1BB67F1,0xA6BC5767,0x3FB506DD,0x48B2364B,
//    0xD80D2BDA,0xAF0A1B4C,0x36034AF6,0x41047A60,0xDF60EFC3,0xA867DF55,0x316E8EEF,
//    0x4669BE79,0xCB61B38C,0xBC66831A,0x256FD2A0,0x5268E236,0xCC0C7795,0xBB0B4703,
//    0x220216B9,0x5505262F,0xC5BA3BBE,0xB2BD0B28,0x2BB45A92,0x5CB36A04,0xC2D7FFA7,
//    0xB5D0CF31,0x2CD99E8B,0x5BDEAE1D,0x9B64C2B0,0xEC63F226,0x756AA39C,0x026D930A,
//    0x9C0906A9,0xEB0E363F,0x72076785,0x05005713,0x95BF4A82,0xE2B87A14,0x7BB12BAE,
//    0x0CB61B38,0x92D28E9B,0xE5D5BE0D,0x7CDCEFB7,0x0BDBDF21,0x86D3D2D4,0xF1D4E242,
//    0x68DDB3F8,0x1FDA836E,0x81BE16CD,0xF6B9265B,0x6FB077E1,0x18B74777,0x88085AE6,
//    0xFF0F6A70,0x66063BCA,0x11010B5C,0x8F659EFF,0xF862AE69,0x616BFFD3,0x166CCF45,
//    0xA00AE278,0xD70DD2EE,0x4E048354,0x3903B3C2,0xA7672661,0xD06016F7,0x4969474D,
//    0x3E6E77DB,0xAED16A4A,0xD9D65ADC,0x40DF0B66,0x37D83BF0,0xA9BCAE53,0xDEBB9EC5,
//    0x47B2CF7F,0x30B5FFE9,0xBDBDF21C,0xCABAC28A,0x53B39330,0x24B4A3A6,0xBAD03605,
//    0xCDD70693,0x54DE5729,0x23D967BF,0xB3667A2E,0xC4614AB8,0x5D681B02,0x2A6F2B94,
//    0xB40BBE37,0xC30C8EA1,0x5A05DF1B,0x2D02EF8D
//};
//
//unsigned long crc32_computeBuf( unsigned long inCrc32, const void *buf, size_t bufLen )
//{
//    unsigned long crc32;
//    unsigned char *byteBuf;
//    size_t i;
//
//    /** accumulate crc32 for buffer **/
//    crc32 = inCrc32 ^ 0xFFFFFFFF;
//    byteBuf = (unsigned char*) buf;
//    for (i=0; i < bufLen; i++) {
//        crc32 = (crc32 >> 8) ^ crcTable[ (crc32 ^ byteBuf[i]) & 0xFF ];
//    }
//    return( crc32 ^ 0xFFFFFFFF );
//}

#define CRC_CTL_CRCMODE_Pos              (30) 

#define CRC_CCITT           (0UL << CRC_CTL_CRCMODE_Pos) /*!<CRC Polynomial Mode - CCITT \hideinitializer */
#define CRC_8               (1UL << CRC_CTL_CRCMODE_Pos) /*!<CRC Polynomial Mode - CRC8 \hideinitializer */
#define CRC_16              (2UL << CRC_CTL_CRCMODE_Pos) /*!<CRC Polynomial Mode - CRC16 \hideinitializer */
#define CRC_32              (3UL << CRC_CTL_CRCMODE_Pos) /*!<CRC Polynomial Mode - CRC32 \hideinitializer */

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
volatile uint32_t u32seed;

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

int main(int argc, char* argv[])
{
    FILE* fptr;
	long binSize, resultSize;
	uint32_t oriCrc32, newCrc32;
	//uint32_t newBinSize;

//  if(argc < 4)
//  {
//      printf("param < 3");
//      exit(1);
//  }

	//newBinSize = atoi(argv[3]);
//  if(newBinSize >  MAX_NEW_BIN_SIZE)
//  {
//      printf("output binary size over maximum");
//      exit(1);
//  }

    if ((fopen_s(&fptr, argv[1], "rb")) != 0) 
	{
        printf("Error! opening file: %s", argv[1]);
        exit(1);
    }

	// obtain file size:
	fseek (fptr , 0 , SEEK_END);
	binSize = ftell (fptr);
	rewind (fptr);
	
//  if(binSize > newBinSize - 4)//last four bytes are reserved for CRC32
//  {
//      printf("Input bin size:%d will over output binary size: %d\n", binSize, newBinSize);
//      fclose (fptr);
//      exit(1);
//  }

	// copy the file into the NewBinArray:
	memset((uint8_t*)NewBinArray, 0xFF, MAX_NEW_BIN_SIZE);
	resultSize = fread ((char*)NewBinArray, sizeof(char), binSize, fptr);
	if (resultSize != binSize) 
	{
		fclose (fptr);
		printf ("Reading error", stderr); 
		exit (3);
	}
	fclose (fptr);
	
	//oriCrc32 = crc32_computeBuf( 0, (const void *)NewBinArray, binSize -  );
	//printf("Input Bin CRC32: %X\n", oriCrc32);
	
	//Compute CRC32 of binary that filled with 0xFF
	//newCrc32 = crc32_computeBuf( 0, (const void *)NewBinArray, newBinSize - 4);
	//printf("New Bin CRC32: %X\n", newCrc32);

    u32TotalByteCnt = binSize - CHECKSUM_OFFSET_FROM_TAIL;
    u32seed = 0xFFFFFFFF;	
    newCrc32 = CRC_SWResult((uint8_t*)NewBinArray,0,0,0,0);
	printf("add Bin CRC32: 0x%08X at address 0x%08X\n", newCrc32, binSize - CHECKSUM_OFFSET_FROM_TAIL);
    

	//Put CRC32 at the end of binary 
	memcpy((char*) NewBinArray + binSize - CHECKSUM_OFFSET_FROM_TAIL, (uint8_t*) &newCrc32, 4);
    if ((fopen_s(&fptr, argv[1], "wb")) != 0) 
	{
        printf("Error! opening file: %s\n", argv[1]);
        exit(1);
    }
	fwrite((char *) NewBinArray, sizeof(char), binSize, fptr);
	fclose (fptr);
	
    return 0;
}