/*
 * linux/include/asm-arm/arch-solos/hardware/charm.h
 * Copyright (C) 2004 Conexant
 */

#ifndef __ASM_ARCH_CHARM
#define __ASM_ARCH_CHARM

#define CHM_BASE_PHYS (0xFF000000)
/* Why move it? */
#define CHM_BASE_VIRT (0xFF000000)

#define CHM_MAP_SIZE (0x1000)

#define CHMIO_VIRT( _x ) (*(volatile unsigned long*)(CHM_BASE_VIRT + CHM_ ##_x ))

#define CHM_VERSION_OFFSET (0x00)
#define CHM_DEVICE_OFFSET  (0x04)
#define CHM_SPICTRL_OFFSET (0x34)


#define CHM_VERSION_IDPINSSHIFT (16)

/* Defines for the Vectored Interrupt Controller */
#define VIC_BASE (CHM_BASE + 0x400)
#define CHM_VIC_IRQ_ENA    (0x410)
#define CHM_VIC_IRQ_DIS    (0x414)
#define CHM_VIC_IRQ_SRC    (0x420)	/* Next IRQ to service. Read does not advance round-robin */
#define CHM_VIC_IRQ_ACTIVE (0x424)	/* Write to here does advance round-robin */
#define CHM_VIC_IRQ_RR     (0x440)	/* Round robin info */
#define CHM_VIC_IRQ_SRCADV (0x444)	/* Next IRQ to service. Read does advance round-robin */

#define CHM_VIC_RR_AUTOADV       (0x20000000)	/* Auto Advance RR's         */
/* Defines for the watchdog */
#define CHM_WDOG_CTRL	(0x200)
#define CHM_WDOG_RESET	(0x20c)	/* Reset register */
#define CHM_WDOG_CTRL_SWRESET (0xf8)	/* Value to write */

/* Sleep register */
#define CHM_SLEEP (0x508)

/* Timer stuff */
#define CHARM_TIMER_FREQUENCY (8000000)

#define CHM_TIMER2_VALUE  (0x110)
#define CHM_TIMER2_CTRL   (0x114)
#define CHM_TIMER2_RELOAD (0x118)
#define CHM_TIMER2_CLEAR  (0x11c)

#define CHM_TIMER_CTRL_VALIDBITS (0x0f)	/* Mask of valid bits in c/s reg   */
#define CHM_TIMER_CTRL_OVERRUN   (0x08)	/* Timer overrun has occurred      */
#define CHM_TIMER_CTRL_SIGEXT    (0x04)	/* Clock off external 8kHz signal  */
#define CHM_TIMER_CTRL_SIGINT    (0x00)	/* Clock off internal 8MHz signal  */
#define CHM_TIMER_CTRL_INTENB    (0x02)	/* Interrupt Enable                */
#define CHM_TIMER_CTRL_INTDIS    (0x00)	/* Interrupts disabled             */
#define CHM_TIMER_CTRL_RELOAD    (0x01)	/* Reload mode                     */
#define CHM_TIMER_CTRL_ONESHOT   (0x00)	/* One-shot mode                   */

/* Uart stuff */
#define CHM_UART_RX     (0x300)
#define CHM_UART_TX     (0x300)
#define CHM_UART_STATUS (0x304)
#define CHM_UART_CTRL   (0x304)

#define CHARM_UART_TXIDLE (0x0000001)	/* ro Tx empty and transmit done  */

#define CHARM_UART_TX_INTEN (0x0000004)
#define CHARM_UART_RX_INTEN (0x0000010)
#define CHARM_UART_BR_INTEN (0x0010000)
#define CHARM_UART_GENBREAK (0x0020000)
#define CHARM_UART_RXORUN_CLR (0x0000020)
#define CHARM_UART_BREAK_CLR (0x0008000)
#define CHARM_UART_UARTEN    (0x0000200)
#define CHARM_UART_BREAK (0x0008000)
#define CHARM_UART_RXORUN (0x0000020)
#define CHARM_UART_TXFULL     (0x0000002)
#define CHARM_UART_RXNOTEMPTY (0x0000008)

#define CHARM_UART_BAUD_MASK  (0x0003800)
#define CHARM_UART_BAUD_9600  (0x0000000)	/* rw  9600 baud                  */
#define CHARM_UART_BAUD_19200 (0x0000800)	/* rw 19200 baud                  */
#define CHARM_UART_BAUD_31250 (0x0001000)	/* rw 31250 baud                  */
#define CHARM_UART_BAUD_38400 (0x0001800)	/* rw 38400 baud                  */

#endif
