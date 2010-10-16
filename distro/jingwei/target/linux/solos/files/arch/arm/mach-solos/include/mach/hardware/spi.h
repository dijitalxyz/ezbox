/*
 * linux/include/asm-arm/arch-solos/hardware/spi.h
 * Copyright (C) 2005 Conexant
 */
/* Definitions for the on-chip SPI controllers*/

#ifndef _ASM_ARCH_SPI
#define _ASM_ARCH_SPI

#define SOLOS_SPI0_BASE	(0x400)
#define SOLOS_SPI1_BASE	(0x500)

#define SOLOS_SPI_CTRLSTAT	(0x00)
#define SOLOS_SPI_AUXCTRL	(0x04)
#define SOLOS_SPI_TXFIFO	(0x40)
#define SOLOS_SPI_RXFIFO	(0x80)

/* CTRL/STAT definitions */
#define SOLOS_SPI_CS_TXSPACE (1 << 31)
#define SOLOS_SPI_CS_RXDATA  (1 << 30)
#define SOLOS_SPI_CS_TXIDLE  (1 << 29) 
#define SOLOS_SPI_CS_READMINUS (1 << 25)
//#define SOLOS_SPI_CS_TXLEN_MASK ((1 << 11) - 1)
#define SOLOS_SPI_CS_TXLEN_MASK ((1 << 12) - 1)
#define SOLOS_SPI_CS_TXFIFO_EMPTY (1 << 27)
#define SOLOS_SPI_CS_RXON   (1 << 13)
#define SOLOS_SPI_CS_TXAUTO (1 << 12)
#define SOLOS_SPI_CS_STARTBIT_7  (0x0000C000)
#define SOLOS_SPI_CS_STARTBIT_15 (0x00008000)
#define SOLOS_SPI_CS_STARTBIT_23 (0x00004000)
#define SOLOS_SPI_CS_STARTBIT_31 (0x00000000)


#define SOLOS_SPI_CS_TXWIDTH_32 (0x00000000)
#define SOLOS_SPI_CS_TXWIDTH_8 (1 << 24 )

#define SOLOS_SPI_CS_FRAMEDELAY_1 (0x00100000)
#define SOLOS_SPI_CS_CSEL_SHIFT (16)
#define SOLOS_SPI_CS_CSEL_MASK  (0xf)
#define SOLOS_SPI_CS_CHIPSEL(_n)    ((SOLOS_SPI_CS_CSEL_MASK & ~(1 << _n) ) << SOLOS_SPI_CS_CSEL_SHIFT)

/* AUXCTRL definitions */
#define SOLOS_SPI_AUXCTRL_TX_IRQEN	(1 << 24)
#define SOLOS_SPI_AUXCTRL_RX_IRQEN	(1 << 23)
#define SOLOS_SPI_AUXCTRL_IDLE_IRQEN	(1 << 22)
#define SOLO_SPI_AUXCTRL_ENAB		(1 << 21)
#define SOLO_SPI_AUXCTRL_CLEAR		(1 << 20)
#define SOLOS_SPI_AUXCTRL_OUTEDGE_HI	(1<< 18)
#define SOLOS_SPI_AUXCTRL_OUTEDGE_LO	(0)
#define SOLOS_SPI_AUXCTRL_INEDGE_HI	(1<< 17)
#define SOLOS_SPI_AUXCTRL_INEDGE_LO	(0)
#define SOLO_SPI_AUXCTRL_CLKHI		(1 << 16) /* Set if the clock idles high */
#define SOLO_SPI_AUXCTRL_CLKLO		(0)

#define SOLOS_SPI_FIFO_DEPTH (32)

extern int spi_max_xfer;

#define SPI_CLCKHI   (1)
#define SPI_ALTPHASE (2)
extern int spi_transact(int bus, int cs, const u8 *outdata, int txlen, u8 *indata, int rxlen, u32 flags);

#endif
