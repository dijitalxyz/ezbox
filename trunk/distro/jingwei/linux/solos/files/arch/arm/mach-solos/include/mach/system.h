/*
 * linux/include/asm-arm/arch-solos/system.h
 * (C) Copyright Conexant 2004
 */

#ifndef __ASM_ARCH_SYSTEM_H
#define __ASM_ARCH_SYSTEM_H
#include <mach/hardware.h>

static inline void arch_idle(void)
{
	volatile int dummy;
	/* Read the sleep register */
	dummy = CHMIO_VIRT(SLEEP);
}

static inline void arch_reset(char mode, const char *cmd)
{
   volatile unsigned long arm9_reg;

   /* Reset ARM-9 for avoiding FISZ issue while booting. */
 
   printk("ARCH_RESET: Halting ARM-9, before reset.......\n");
   arm9_reg = 0;
 
   arm9_reg = *(volatile unsigned long *)(0xFF802078);
 
   arm9_reg |= 0x40000000;
   arm9_reg &= 0xFF7FFFFF;
 
   *(volatile unsigned long *)(0xFF802078) = arm9_reg;
   *(volatile unsigned long *)(0xFF802060) = 0xFFFFFFFF;

	CHMIO_VIRT(WDOG_CTRL) = CHM_WDOG_CTRL_SWRESET;
}

#endif
