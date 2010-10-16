/*
 * linux/include/asm-arm/arch-solos/irqs.h
 *
 * Copyright (C) 2004 Conexant
 */

#ifndef __ASM_ARCH_IRQS_H
#define __ASM_ARCH_IRQS_H

#ifdef CONFIG_ARCH_SOLOS_GALLUS

#elif defined CONFIG_ARCH_SOLOSW_GALLUS

#elif defined CONFIG_ARCH_SOLOS_MARIUS

#else
#error Unsupported solos variant
#endif

#define IRQ_GPIO0        (0)    /* GPIO */
#define IRQ_GPIO1        (1)    /* GPIO */
#define IRQ_GPIO2        (2)    /* GPIO */
#define IRQ_SPI0         (3)    /* SPI channel 0 */
#define IRQ_SPI1         (4)    /* SPI channel 1 */
#define IRQ_CRYPT        (5)    /* crypto unit */

#define IRQ_ETH0         (7)	/* On-board MAC 0 */
#define IRQ_ETH1         (8)	/* On-board MAC 1 */
#define IRQ_USB0	 (9)    /* 1st USB HC */
#define IRQ_USB1	 (10)   /* 2nd ... */

#define IRQ_SELF         (24)	/* Self IRQ                       */
#define IRQ_WDOG         (25)	/* Watchdog expiry                */
#define IRQ_TIMER2       (26)	/* Timer 2 expiry                 */
#define IRQ_TIMER3       (27)	/* Timer 3 expiry                 */
#define IRQ_TIMER4       (28)	/* Timer 4 expiry                 */
#define IRQ_UART         (29)	/* UART TX empty / RX full        */
#define IRQ_COMMRX       (30)	/* ARM ICE character received     */
#define IRQ_COMMTX       (31)	/* ARM ICE character sent         */

#define NR_IRQS           (32)

#endif
