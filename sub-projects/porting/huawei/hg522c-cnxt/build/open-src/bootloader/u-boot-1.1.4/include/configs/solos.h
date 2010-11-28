/*
 * Rick Bronson <rick@efn.org>
 *
 * Configuation settings for the AT91RM9200DK board.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_ARM1026EJS	1	/* This is an ARM1026EJS Core	*/

/*
 * This define decides the definition of functions in
 * cpu.c
 * Disabling MMU support for the time being.
 #define USE_1026EJS_MMU		1
 */

#define CONFIG_CMDLINE_TAG	1	/* enable passing of ATAGs	*/
#define CONFIG_SETUP_MEMORY_TAGS 1
#define CONFIG_INITRD_TAG	1

#define CONFIG_SKIP_RELOCATE_UBOOT     1
#define CONFIG_SKIP_LOWLEVEL_INIT      1

/*
 * Size of malloc() pool
 */
#define CFG_MALLOC_LEN	(CFG_ENV_SIZE + 128*1024)
#define CFG_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */

#define CONFIG_BAUDRATE 38400

/*
 * Hardware drivers
 */


#define CONFIG_BOOTDELAY      3

#define CONFIG_COMMANDS ( CONFIG_CMD_DFL & ~CFG_CMD_NET )

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

/* 
 * SDRAM specific defines
 */
#define CONFIG_NR_DRAM_BANKS 1
#define PHYS_SDRAM 0x00000000
#define PHYS_SDRAM_SIZE 0x2000000  /* 32 megs */

#define CFG_RAMBOOT             1
#define CFG_MEMTEST_START		PHYS_SDRAM
#define CFG_MEMTEST_END			CFG_MEMTEST_START + PHYS_SDRAM_SIZE - 262144

/* TODO ACHIN
 * Include support for an ethernet driver. This is being commented out to begin with,
 * since we know little about porting this driver in u-boot. 
 
 #define CONFIG_DRIVER_ETHER
 
 */
#define CONFIG_NET_RETRY_COUNT		20

/*
 * Parallel Flash specific defines
 */
#define PHYS_FLASH_1			0x38000000
#define PHYS_FLASH_SIZE			0x1000000  /* 16 megs main flash */
#define CFG_FLASH_BASE			PHYS_FLASH_1
#define CFG_MAX_FLASH_BANKS		1
#define CFG_MAX_FLASH_SECT		128
#define CFG_FLASH_ERASE_TOUT		(2*CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT		(2*CFG_HZ) /* Timeout for Flash Write */
#define CFG_FLASH_UNLOCK_TOUT           (2*CFG_HZ) /* Timeout for Flash Unlock */
#define CFG_FLASH_CFI                   1
#define CFG_FLASH_CFI_DRIVER            1

/*
 * Configuration information details
 * We will keep the configuration information in the 
 * second sector of our flash chip.
 */
#define CFG_ENV_IS_IN_FLASH		1
#ifdef CONFIG_SKIP_LOWLEVEL_INIT
#define CFG_ENV_ADDR			(PHYS_FLASH_1 + 0x20000)  /* after u-boot.bin */
#define CFG_ENV_SIZE			0x20000 /* sectors are 64K here */
#else
#define CFG_ENV_ADDR			(PHYS_FLASH_1 + 0xe000)  /* between boot.bin and u-boot.bin.gz */
#define CFG_ENV_SIZE			0x2000  /* 0x8000 */
#endif	/* CONFIG_SKIP_LOWLEVEL_INIT */

#define CFG_MONITOR_BASE        TEXT_BASE
#define CFG_MONITOR_LEN         0x20000


#define CFG_LOAD_ADDR		0x21000000  /* default load address */

#define CFG_BAUDRATE_TABLE	{115200 , 19200, 38400, 57600, 9600 }

#define CFG_PROMPT		"U-Boot> "	/* Monitor Command Prompt */
#define CFG_CBSIZE		256		/* Console I/O Buffer Size */
#define CFG_MAXARGS		16		/* max number of command args */
#define CFG_PBSIZE		(CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */

#define CFG_HZ 1000
#define CONFIG_STACKSIZE	(32*1024)	/* regular stack */


/*
 * Solos specific definitions
 */

#define HFINFO_BLOCK_PTR        0x80 /* Offset in DRAM which contains the address to the location 
				      * containing the params to be passed to any OS. */


#define CHM_MAP_PERIPH                          (0xFF000000) /* Start of CHARM peripherals space    */
#define CHARMIO(_reg) ( *((volatile unsigned int *) (((unsigned int) CHM_MAP_PERIPH) + ((unsigned int) (CHARM_##_reg)))))

/*
 * UART driver support defs
 */
#define CR      0x0D
#define LF      0x0A

#define CHARM_UART_CTRL                 (0x304) /* UART control (write)           */
#define CHARM_UART_TX                   (0x300) /* UART Tx (write)                */
#define CHARM_UART_RX                   (0x300) /* UART Rx (read)                 */
#define CHARM_UART_STAT                 (0x304) /* UART status (read)             */
#define CHARM_UART_AUX                  (0x308) /* UART auxiliary (read/write)    */

#define CHARM_UART_CTRL_UARTEN          (0x0000200) /* rw Enable UART (0 for reset)  */
#define CHARM_UART_CTRL_BAUD_38400      (0x0001800) /* rw 38400 baud                 */
#define CHARM_UART_STAT_RXORUN          (0x0000020) /* rc Receiver overrun (status)  */
#define CHARM_UART_RX_VALID             (0x00100)   /* Set if data is valid          */
#define CHARM_UART_STAT_RXNOTEMPTY      (0x0000008) /* ro Rx buffer contains data    */
#define CHARM_UART_STAT_TXFULL          (0x0000002) /* ro Tx buffer is full          */
#define CHARM_UART_STAT_TXIDLE          (0x0000001) /* ro Tx empty and transmit done */

#define CHARM_UART_READ_MASK            (0xff) /* read mask for recevied data */


#define CHARM_UART_CTRL_REG             (CHM_MAP_PERIPH + CHARM_UART_CTRL)
#define CHARM_UART_RX_REG               (CHM_MAP_PERIPH + CHARM_UART_RX)
#define CHARM_UART_STAT_REG             (CHM_MAP_PERIPH + CHARM_UART_STAT)
#define CHARM_UART_TX_REG               (CHM_MAP_PERIPH + CHARM_UART_TX)

/*
 * Watchdog support definitions to perform a software reset
 */
#define CHARM_WDOG_CTRL            (0x200)      /* Watchdog control (write)  */
#define CHARM_WDOG_CTRL_SWRESET    (0x000000f8) /* Write to cause s/w reset  */

/*
 * Timer support definitions.
 */
#define CHARM_TIMER1_VALUE         (0x108) /* Timer 1 current value          */

#define write_reg(_reg,_val)            (*((volatile unsigned int *)(_reg)) = _val)
#define read_reg(_reg)                  (*((unsigned int *) (_reg)))

#endif
