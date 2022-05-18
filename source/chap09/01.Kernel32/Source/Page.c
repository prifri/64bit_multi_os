#include "Page.h"

// page netry에 기준 주소와 속성 flag를 설정
void kSetPageEntryData(PTENTRY *pstEntry,
							  uint32_t dwUpperBaseAddress,
							  uint32_t dwLowerBaseAddress,
							  uint32_t dwLowerFlags,
							  uint32_t dwUpperFlags)
{
	pstEntry->dwAttributeAndLowerBaseAddress =
		(dwLowerBaseAddress & PAGE_ENTRY_LOWER_BASE_ADDR_MASK) | dwLowerFlags;
	pstEntry->dwUpperBaseAddressAndEXB =
		(dwUpperBaseAddress & PAGE_ENTRY_UPPER_BASE_ADDR_MASK) | dwUpperFlags;
}

// IA-32e mode kernel을 위한 page table 생성
void kInitializePageTables(void)
{
	PML4ENTRY *pstPML4TEntry;
	PDPTENTRY *pstPDPTEntry;
	PDENTRY *pstPDEntry;
	uint32_t dwMappingAddress;
	unsigned long long i;

	// PML4 table 생성
	// 첫 번째 entry 외에 나머지는 모두 0으로 초기화
#define PML4_TABLE_ADDR	((uint32_t *)0x100000)
#define PDPT_BASE_ADDR	((uint32_t *)0x101000)
#define PDE_BASE_ADDR	((uint32_t *)0x102000)
	pstPML4TEntry = (PML4ENTRY *)PML4_TABLE_ADDR;

	for (i = 0; i < PML4_MAXENTRY_COUNT; i++)
	{
		kSetPageEntryData(pstPML4TEntry, 0,
						  __pa_symbol(PDPT_BASE_ADDR) + (i * PAGE_TABLESIZE),
						  PAGE_FLAGS_DEFAULT, 0);
	}

	for (i = PML4_MAXENTRY_COUNT; i < PAGE_MAXENTRYCOUNT; i++)
	{
		kSetPageEntryData(pstPML4TEntry, 0, 0, 0, 0);
	}

	// PDT table 생성
	// 하나의 PDPT로 512GB까지 매핑 가능하므로 하나로 충분함.
	// 64개의 entry를 설정하여 64GB까지 매핑함.
	pstPDPTEntry = (PDPTENTRY *)PDPT_BASE_ADDR;

	for (i = 0; i < PDPT_MAXENTRY_COUNT; i++)
	{
		kSetPageEntryData(&pstPDPTEntry[i], 0,
						  __pa_symbol(PDE_BASE_ADDR) + (i * PAGE_TABLESIZE),
						  PAGE_FLAGS_DEFAULT, 0);
	}

	for (i = PDPT_MAXENTRY_COUNT; i < PAGE_MAXENTRYCOUNT; i++)
	{
		kSetPageEntryData(&pstPDPTEntry[i], 0, 0, PAGE_FLAGS_DEFAULT, 0);
	}

	// PDE Table 생성
	// 하나의 페이지 디렉터리가 1GB까지 매핑 가능
	// 여유있게 64개의 페이지 디렉터리를 생성하여 총 64GB까지 지원
	
	pstPDEntry = (PDENTRY *)PDE_BASE_ADDR;
	dwMappingAddress = 0;

	for (i = 0; i < MEMORY_SIZE / PDE_SIZE; i++)
	{
		//32bit로는 상위 어드레스를 표현할 수 없으므로 MB 단위로 계산한 다음
		//최종 결과를 다시 4KB로 나누어 32bit 이상의 어드레스를 계산함.
		kSetPageEntryData(&pstPDEntry[i],
						  (i * (PAGE_DEFAULTSIZE >> 20)) >> 12,
						  dwMappingAddress,
						  PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS, 0);
		dwMappingAddress += PAGE_DEFAULTSIZE;
	}

	for (i = MEMORY_SIZE / PDE_SIZE; i < PAGE_MAXENTRYCOUNT; i++)
	{
		//32bit로는 상위 어드레스를 표현할 수 없으므로 MB 단위로 계산한 다음
		//최종 결과를 다시 4KB로 나누어 32bit 이상의 어드레스를 계산함.
		kSetPageEntryData(&pstPDEntry[i], 0, 0, 0, 0);
	}
}

