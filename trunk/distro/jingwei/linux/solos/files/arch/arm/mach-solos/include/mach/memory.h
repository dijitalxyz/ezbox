/*
 * linux/include/asm-arm/arch-solos/memory.h
 *
 * Copyright (c) 2004 Conexant
 *
 */
#ifndef __ASM_ARCH_MEMORY_H
#define __ASM_ARCH_MEMORY_H

#define PHYS_OFFSET	UL(0x00000000)

/* The DMA addresses and the base of DRAM coincide, so this is nice and easy */
#define __virt_to_bus(x)	__virt_to_phys(x)
#define __bus_to_virt(x)	__phys_to_virt(x)

#endif
