/*
 * linux/arch/arm/mach-solos/board.h
 *
 * Author: Guido Barzini
 * Copyright (C) 2005 Conexant inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef __ASM_ARCH_BOARD_H
#define __ASM_ARCH_BOARD_H

#include <linux/types.h>
#include <linux/device.h>

/* Hook for board-specific initialisation */
extern void solos_board_init(void);

/* Ethernet (EMAC & MACB) */
struct solos_eth_data {
	u32		phy_mask;
	u8		phy_irq_pin;    /* PHY IRQ */
	u8		is_rmii;        /* using RMII interface? */
};

#endif
