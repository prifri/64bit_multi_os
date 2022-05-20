/**
 *  file    Main.c
 *  date    2008/12/14
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   C 언어로 작성된 커널의 엔트리 포인트 파일
 */

#include "Types.h"
#include "Page.h"
#include "ModeSwitch.h"

static void __main(void);
/**
 *  아래 함수는 C 언어 커널의 시작 부분임
 *      반드시 다른 함수들 보다 가장 앞쪽에 존재해야 함
 */
void Main( void )
{
	__main();
}

static void kernel_stop(void)
{
	while (1);
}

/*
 * PRIFI, 2022.05.16:
 * - IA-32e 모드용 커널 영역(1MB ~ 6MB)을 0으로 초기화
 */
bool kInitializeKernel64Area(void)
{
	uint32_t *start = (uint32_t *)KERNEL64_START;

	while (start < (uint32_t *)KERNEL64_END)
	{
		*start = 0;
		if (*start != 0)
		{
			return false;
		}
		start++;
	}

	return true;
}

/*
 * IAMROOT, 2022.05.18:
 * - 1MB ~ 64 MB 까지 1MB단위로 memory 검사.
 */
static bool kIsMemoryEnough(void)
{
	uint32_t *start = (uint32_t *)KERNEL64_START;

	while (start < (uint32_t *)KERNEL64_MAX)
	{
		*start = 0x12345678;
		if (*start != 0x12345678)
		{
			return false;
		}
		start += (1024 * 1024 / sizeof(*start));
	}

	return true;
}
/**
 *  문자열을 X, Y 위치에 출력
 */
static void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* )VIDEO_ADDR;
    int i;
    
    // X, Y 좌표를 이용해서 문자열을 출력할 어드레스를 계산
    pstScreen += ( iY * VIDEO_Y_OFFSET ) + iX;
    
    // NULL이 나올 때까지 문자열 출력
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
		//pstScreen[i].bAttribute = 0;
    }
}

#define STR_PASS	"PASS"
#define STR_FAIL	"FAIL"

static void cpuid_read(void)
{
	uint32_t dwEAX, dwEBX, dwECX, dwEDX;
#define VENDOR_STR_SIZE	(sizeof(uint32_t) * 3 + 1)
	char vcVendorString[VENDOR_STR_SIZE] = {0};

/*
 * PRIFI, 2022.05.18:
 * - vendor stirng을 가져온다.
 */
#define CPUID_REG	(0)
	// read cpuid
	kReadCPUID(CPUID_REG, &dwEAX, &dwEBX, &dwECX, &dwEDX);
	*((uint32_t *)vcVendorString) = dwEBX;
	*((uint32_t *)vcVendorString + 1) = dwEDX;
	*((uint32_t *)vcVendorString + 2) = dwECX;
    kPrintString( 0, 7, "Processor Vendor String.....................[            ]");
	kPrintString(45, 7, vcVendorString);
	//*((uint32_t *)vcVendorString + 3) = dwEBX;
	
#define CPUID_EXT_REG	(0x80000001)
	kReadCPUID(CPUID_EXT_REG, &dwEAX, &dwEBX, &dwECX, &dwEDX);
    kPrintString( 0, 8, "64bit Mode Support Check....................[    ]");
	if (dwEDX & ( 1 << 29))
	{
		kPrintString(45, 8, STR_PASS);
	}
	else
	{
		kPrintString(45, 8, STR_FAIL);
		kPrintString(0, 9, "This processor does not support 64bit mode~!!");
		kernel_stop();
	}
}

// IA-32e mode kernel을 2MByte address에 copy
static void kCopyKernel64ImageTo2Mbyte(void)
{
	uint16_t wKernel32SectorCount, wTotalKernelSectorCount;
	uint32_t *pdwSourceAddress, *pdwDestinationAddress;
	int i;

	// 0x7C05에 총 kernel sector수, 0x7c07에 보호 모드 커널 섹터수가 들어있음
	wTotalKernelSectorCount = *BOOTLOADER_TOTALSECTORCOUNT_ADDR;
	wKernel32SectorCount = *BOOTLOADER_KERNEL32SECTORCOUNT_ADDR;

	void *ia32e_image_addr = (void *)(PROTECT_MODE_ADDR +
		wKernel32SectorCount * SECTOR_SIZE);
	pdwSourceAddress = ia32e_image_addr;
	pdwDestinationAddress = (uint32_t *)IA32e_MODE_ADDR;

	for (i = 0; i < SECTOR_SIZE *
		 (wTotalKernelSectorCount - wKernel32SectorCount) /
		 (int)sizeof(uint32_t); i++)
	{
		*pdwDestinationAddress = *pdwSourceAddress;
		pdwDestinationAddress++;
		pdwSourceAddress++;
	}
}
static void __main(void)
{
    kPrintString(0, 3, "Proctected C Language Kernel Started........[PASS]");
	kPrintString(0, 4, "Minimum Memory Size Check...................[    ]");

	if (!kIsMemoryEnough())
	{
		kPrintString(45, 4, STR_FAIL);
		kPrintString(0, 5, "Not Enough Memory. MINT64 OS Require Over 64MB");
		goto stop;
	}
	else
	{
		kPrintString(45, 4, STR_PASS);
	}

    kPrintString( 0, 5, "IA-32e Kernel Area Init.....................[    ]");
	if (!kInitializeKernel64Area())
	{
		kPrintString(45, 5, STR_FAIL);
		kPrintString(0, 6, "Kernel Area Init fail");
		goto stop;
	} else
	{
		kPrintString(45, 5, STR_PASS);
	}

	// IA-32e 64bit mode kernel을 위한 페이지 테이블 생성.
    kPrintString( 0, 6, "IA-32e 64bit Page Tables Initialize.........[    ]");
	kInitializePageTables();
	kPrintString(45, 6, STR_PASS);

	cpuid_read();

    kPrintString( 0, 9, "Copy IA-32e Kernel To 2M Address............[    ]");
	kCopyKernel64ImageTo2Mbyte();
	kPrintString(45, 9, STR_PASS);
	// IA-32e mode로 전환
	kPrintString(0, 10, "Switch To IA-32e Mode");
	kSwitchAndExecute64bitKernel();
stop:
	kernel_stop();
}
