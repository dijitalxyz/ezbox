/*
 * linux/arch/arm/mach-solos/msc_drv.h
 *
 * Author: Guido Barzini
 * Copyright (C) 2005 Conexant inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

/* Support code for the MSC16 controllers */

enum msc16_id {
	MSC_DSL_ID  = 0,
	MSC_NXM_ID,
	MSC_CRYPT_ID
};

/* Request the firmware for the specified  msc16 and load it */
extern int msc16_load_fw(enum msc16_id id);
/* Set r0 to arg0, r1 to arg1 and start the MSC16 from location 0 */
extern void msc16_run_fw(enum msc16_id id, u16 arg0, u16 arg1 );
/* Start tghe MSC16 at the specified address */
extern void msc16_start(enum msc16_id id, u16 addr);
/* Stop the MSC16 */
extern void msc16_stop(enum msc16_id id);
/* Set ATT0 and wait for STAT0 to come on */
extern void msc16_handshake(enum msc16_id id);

/* Write to MSC16 periph, data instr spaces. */
extern void msc16_write_periph(enum msc16_id id, u16 addr, u16 val);
extern void msc16_write_data(enum msc16_id id, u16 addr, u16 val);
extern void msc16_write_instr(enum msc16_id id, u16 addr, u16 val);

/* Read MSC16 instr and data */
extern u16 msc16_read_instr(enum msc16_id id, u16 addr);
extern u16 msc16_read_data(enum msc16_id id, u16 addr);
