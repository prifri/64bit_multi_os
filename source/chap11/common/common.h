#ifndef __COMMON_H__
#define __COMMON_H__

//****************************************************************************
// device addr
//****************************************************************************

#define VIDEO_ADDR (0xB8000)
#define VIDEO_Y_OFFSET	80
//****************************************************************************
// memory map
//****************************************************************************

#define BOOTLOADER_ADDR		(0x7C0)
#define PROTECT_MODE_ADDR	(0x10000UL)

#define PML4_TABLE_ADDR		(0x100000)
#define PDPT_BASE_ADDR		(0x101000)
#define PDE_BASE_ADDR		(0x102000)

#define IA32e_MODE_ADDR		(0x200000)

///

#define KERNEL64_START	(1024 * 1024 * 1)
#define KERNEL64_END	(1024 * 1024 * 6) 
#define KERNEL64_MAX	(1024 * 1024 * 64)

//****************************************************************************
// Boot loader
//****************************************************************************

#define BOOTLOADER_JMP_CMD_SIZE	(5)
#define BOOTLOADER_TOTALSECTORCOUNT_OFFSET	(BOOTLOADER_JMP_CMD_SIZE)
#define BOOTLOADER_TOTALSECTORCOUNT_SIZE	(sizeof(unsigned short))
#define BOOTLOADER_TOTALSECTORCOUNT_ADDR	\
	((unsigned short *)((unsigned char *)BOOTLOADER_ADDR + \
				  BOOTLOADER_TOTALSECTORCOUNT_OFFSET))
#define BOOTLOADER_KERNEL32SECTORCOUNT_OFFSET	\
	(BOOTLOADER_TOTALSECTORCOUNT_OFFSET + BOOTLOADER_TOTALSECTORCOUNT_SIZE)
#define BOOTLOADER_KERNEL32SECTORCOUNT_SIZE	(sizeof(unsigned short))
#define BOOTLOADER_KERNEL32SECTORCOUNT_ADDR	\
	((unsigned char *)((unsigned char *)BOOTLOADER_ADDR + \
				BOOTLOADER_KERNEL32SECTORCOUNT_OFFSET))

//****************************************************************************
// Sector
//****************************************************************************

#define SECTOR_SIZE	512
#define BYTESOFSECTOR SECTOR_SIZE
#endif
