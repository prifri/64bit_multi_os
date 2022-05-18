#ifndef __PAGE_H__
#define __PAGE_H__

#include "Types.h"

#define PAGE_FLAGS_P	0x00000001	// Present
#define PAGE_FLAGS_RW	0x00000002	// Read/Write
#define PAGE_FLAGS_US	0x00000004	// User/Supervisor(flag 설정 시 user level)
#define PAGE_FLAGS_PWT	0x00000008	// Page Level Write-through
#define PAGE_FLAGS_PCD	0x00000010	// Page Level Cache Disable
#define PAGE_FLAGS_A	0x00000020	// Accessed
#define PAGE_FLAGS_D	0x00000040	// Dirty
#define PAGE_FLAGS_PS	0x00000080	// Page Size
#define PAGE_FLAGS_G	0x00000100	// Global
#define PAGE_FLAGS_PAT	0x00001000	// Page Attribute Table Index
#define PAGE_FLAGS_EXB	0x80000000	// Execute Disable bit
#define PAGE_FLAGS_DEFAULT	(PAGE_FLAGS_P | PAGE_FLAGS_RW)
#define PAGE_TABLESIZE	0x1000		//4k
#define PAGE_MAXENTRYCOUNT	512 // 4KB / sizeof(long)

#define MEMORY_SIZE			(64UL * 1024UL * 1024UL * 1024UL)
#define PTE_SIZE			(2 * 1024 * 1024)
#define PDE_SIZE			(PTE_SIZE * PAGE_MAXENTRYCOUNT)
#define PDPT_SIZE			(PDE_SIZE * PAGE_MAXENTRYCOUNT)
#define PML4_SIZE			(PDPT_SIZE * PAGE_MAXENTRYCOUNT)

#if (MEMORY_SIZE / PTE_SIZE) != 0
#define PTE_MAXENTRY_COUNT	(MEMORY_SIZE / PTE_SIZE)
#else
#define PTE_MAXENTRY_COUNT	1
#endif

#if (MEMORY_SIZE / PDE_SIZE) != 0
#define PDE_MAXENTRY_COUNT	(MEMORY_SIZE / PDE_SIZE)
#else
#define PDE_MAXENTRY_COUNT	1
#endif

#if (MEMORY_SIZE / PDPT_SIZE) != 0
#define PDPT_MAXENTRY_COUNT	(MEMORY_SIZE / PDPT_SIZE)
#else
#define PDPT_MAXENTRY_COUNT	1
#endif

#if (MEMORY_SIZE / PML4_SIZE) != 0
#define PML4_MAXENTRY_COUNT	(MEMORY_SIZE / PML4_SIZE)
#else
#define PML4_MAXENTRY_COUNT	1
#endif

#define PAGE_DEFAULTSIZE	PTE_SIZE

#pragma pack(push, 1)

/*
 * PRIFI, 2022.05.17:
 * - PML4(Page Map Level 4 Table)
 *   IA-32e 64bit mode에서 4KB paging은 5단계, 2MB paging은 4단계를 사용한다.
 * - PDPT(Page-Directory-Pointer-table)
 * - PDE(Page Directory Entry)
 */
// page entry struct
typedef struct kPageTableEntryStruct
{
	// PML4T와 PDPTE의 경우
	// 1 bit P, RW, US, PWT, PCD, A, 3 bit resereved, 3bit Avail,
	// 20 bit base address
	// PDE의 경우
	// 1 bit P, RW, US, PWT, PCD, A, D, 1, G, 3 bit Avail, 1 bit PAT,
	// 8 bit Avail, 11 bit Base address
	//
	/*
	 * IAMROOT, 2022.05.17:
	 * - PWL4, PDPTE
	 *   EXB[63]
	 *   Avail[52:62]
	 *   RSV0[40:51]
	 *   PML4 base address[32:39]
	 *   PML4 base address[12:31]
	 *   Avail[9:11]
	 *   RSV0[6:8]
	 *   A[5] : Available. 임의로 사용 가능.
	 *   PCD[4]
	 *   PWT[3]
	 *   US[2]
	 *   RW[1]
	 *   P[0]
	 * - PDE
	 *   EXB[63]
	 *   Avail[52:62]
	 *   RSV0[40:51]
	 *   PML4 base address[32:39]
	 *   PML4 base address[21:31]
	 *		총 18bit = 2^18
	 *   RSV0[13:20]
	 *   PAT[12]
	 *   Avail[9:11]
	 *   G[8]
	 *   PS1[7]
	 *		PS1 | CR4 PAE | 
	 *		0	|   -     | 4kb
	 *		1   |   0     | 4MB
	 *		1   |   1     | 2MB
	 *   D[6]
	 *   A[5]
	 *   PCD[4]
	 *   PWT[3]
	 *   US[2]
	 *   RW[1]
	 *   P[0]
	 */
#define PAGE_ENTRY_LOWER_BASE_ADDR_MASK	(0xFFF00000)
	uint32_t dwAttributeAndLowerBaseAddress;
	// 8 bit Upper BaseAddress, 12 bit Reserved, 11 bit Avail, 1 bit EXB
#define PAGE_ENTRY_UPPER_BASE_ADDR_MASK	(0xFF)
	uint32_t dwUpperBaseAddressAndEXB;
} PML4ENTRY, PDPTENTRY, PDENTRY, PTENTRY;

#pragma pack(pop)

void kInitializePageTables(void);
void kSetPageEntryData(PTENTRY *pstEntry, uint32_t dwUpperBaseaddress,
					   uint32_t dwLowerBaseAddress,
					   uint32_t dwLowerFlags,
					   uint32_t dwUpperFlags);
#endif
