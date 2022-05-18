/**
 *  file    Main.c
 *  date    2008/12/14
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   C 언어로 작성된 커널의 엔트리 포인트 파일
 */

#include "Types.h"
#include "Page.h"

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
#define KERNEL64_START	((uint32_t *)(1024 * 1024 * 1))
#define KERNEL64_END	((uint32_t *)(1024 * 1024 * 6))
#define KERNEL64_MAX	((uint32_t *)(1024 * 1024 * 64))
	uint32_t *start = KERNEL64_START;

	while (start < KERNEL64_END)
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
	uint32_t *start = KERNEL64_START;

	while (start < KERNEL64_MAX)
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
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    // X, Y 좌표를 이용해서 문자열을 출력할 어드레스를 계산
    pstScreen += ( iY * 80 ) + iX;
    
    // NULL이 나올 때까지 문자열 출력
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
		//pstScreen[i].bAttribute = 0;
    }
}

#define STR_PASS	"PASS"
#define STR_FAIL	"FAIL"
static void __main(void)
{
    kPrintString(0, 3, "C Language Kernel Started...................[PASS]");
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

stop:
	kernel_stop();
}
