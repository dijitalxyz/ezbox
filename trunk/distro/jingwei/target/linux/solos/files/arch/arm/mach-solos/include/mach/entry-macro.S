/*
 * arch/arm/mach-solos/include/mach/entry-macro.S
 *
 * Author: Guido Barzini
 * Copyright (C) 2005 Conexant inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <mach/hardware.h>
		.macro	disable_fiq
		.endm

		.macro  arch_ret_to_user, tmp1, tmp2
		.endm
		
		.macro  get_irqnr_preamble, base, tmp
		mov     \base, #CHM_BASE_VIRT
		.endm

		.macro	get_irqnr_and_base, irqnr, irqstat, base, tmp
		mov	\base, #CHM_BASE_VIRT
		ldr	\irqnr, [\base, #CHM_VIC_IRQ_SRCADV]
		
                cmp \irqnr, #32 @ 32 means no IRQ. 0 is a valid IRQ number.
                
		.endm
	
		.macro  irq_prio_table
		.endm


