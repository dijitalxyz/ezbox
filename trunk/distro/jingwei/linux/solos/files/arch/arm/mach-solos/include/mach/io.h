/*
 * linux/include/asm-arm/arch-solos/io.h
 * (C) Conexant 2004
 */
#ifndef __ASM_ARM_ARCH_IO_H
#define __ASM_ARM_ARCH_IO_H

#define IO_SPACE_LIMIT 0xffffffff

#define __io(_x) (_x)

#define __mem_pci(_x) (_x)	/* For now. This is wrong. */

#endif
