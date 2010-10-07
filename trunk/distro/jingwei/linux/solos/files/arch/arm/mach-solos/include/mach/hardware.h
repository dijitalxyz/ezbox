/*
 * linux/include/asm-arm/arch-solos/hardware.h
 * (C) Conexant 2004
 */

#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

/* Include the core CHARM hardware definitions */
#include <mach/hardware/charm.h>

/* Location and address of ROM. This required for voice dsp */
#define SOLOS_ROM_VIR (0xE0000000)
#define SOLOS_ROM_PHYS (0xFFFF0000)
#define SOLOS_ROM_SIZE (0x8000)
/* Location and address range of uncached SDRAM mapping */
#define SOLOS_SDRAMU_VIRT (0xD0000000)
#define SOLOS_SDRAMU_PHYS (0x20000000)
#define SOLOS_SDRAMU_MAP_SIZE (0x2000000)

/* 
 * Location and address range of EPB memory map
 * This map will support a flash device of 128MB
 */
#define SOLOS_EPBMEM_VIRT (0xD8000000)
#define SOLOS_EPBMEM_PHYS (0x38000000)
#define SOLOS_EPBMEM_MAP_SIZE (0x08000000)

/* Location and address range of the Upper AHB */
#define SOLOS_UAHB_VIRT (0xFF800000)
#define SOLOS_UAHB_PHYS (0x30000000)
/* Way too big, but it's over 1 page so it's less TLB pain to ask for 1M */
#define SOLOS_UAHB_MAP_SIZE (0x00100000)

/* Location and address range of the Lower AHB */
#define SOLOS_LAHB_VIRT (0xFF900000)
#define SOLOS_LAHB_VIRT_CONF(a)  (a | SOLOS_LAHB_VIRT)

#define SOLOS_LAHB_PHYS (0x40000000)
/* Way too big, but it's over 1 page so it's less TLB pain to ask for 1M */
#define SOLOS_LAHB_MAP_SIZE (0x00100000)


#define SOLOS_SRAM_VIRT (0xFFA00000)
#define SOLOS_SRAM_PHYS (0x70000000)
#define SOLOS_SRAM_SIZE (0x00004000)	/* Actual size */
#define SOLOS_SRAM_MAP_SIZE (0x00100000)	/* Size to map: 1 section of 1M */

#ifndef __ASSEMBLY__
/* Chip-specific management of on-chip SRAM */
extern void *solos_sram_alloc(int size);
#endif				/*  __ASSEMBLY__ */

#endif
/* __ASM_ARCH_HARDWARE_H */
