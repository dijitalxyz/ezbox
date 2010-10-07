/*
 * linux/include/asm-arm/arch-solos/hardware/charm.h
 * Copyright (C) 2004 Conexant
 */
/* Definitions for the on-chips 10/100 MACs of Solos family chips */

#ifndef _ASM_ARCH_ETHERNET
#define _ASM_ARCH_ETHERNET

#define SOLOS_ETHER_ID_VALUE             (0x0007fd02)	/* ID register contents    */

#ifdef  CONFIG_ARCH_SOLOS_228PIN
#define SOLOS_ETHER_MAX_PORTS (1)
#else
#define SOLOS_ETHER_MAX_PORTS (2)
#endif

/* Offsets from the AHB base */
#define SOLOS_MAC0_BASE (0x400)
#define SOLOS_MAC1_BASE (0x500)

/* Register offsets in the Ethernet block */
#define SOLOS_ETHER_ID                   (0x000)
#define SOLOS_ETHER_STAT                 (0x004)
#define SOLOS_ETHER_ENABLE               (0x008)
#define SOLOS_ETHER_CONTROL              (0x00c)
#define SOLOS_ETHER_POLLRATE             (0x010)
#define SOLOS_ETHER_RXERR                (0x014)
#define SOLOS_ETHER_MISS                 (0x018)
#define SOLOS_ETHER_TXRINGPTR            (0x01c)
#define SOLOS_ETHER_RXRINGPTR            (0x020)
#define SOLOS_ETHER_ADDRL                (0x024)
#define SOLOS_ETHER_ADDRH                (0x028)
#define SOLOS_ETHER_LAFL                 (0x02c)
#define SOLOS_ETHER_LAFH                 (0x030)
#define SOLOS_ETHER_MDIO                 (0x034)
#define SOLOS_ETHER_TXPTRREAD            (0x038)
#define SOLOS_ETHER_RXPTRREAD            (0x03c)
#define SOLOS_ETHER_XTRACTRL             (0x040)
#define SOLOS_ETHER_XTRASTAT             (0x044)

/* Interrupt Status Register Bit Masks */
#define SOLOS_ETHER_STAT_TXINT           (0x00000001)	/* Tx interrupt pending    */
#define SOLOS_ETHER_STAT_RXINT           (0x00000002)	/* Rx interrupt pending    */
#define SOLOS_ETHER_STAT_ERR             (0x00000004)	/* Error interrupt pending */
#define SOLOS_ETHER_STAT_BUFFERR         (0x00000008)	/* Error in Tx buffer      */
#define SOLOS_ETHER_STAT_TXCH            (0x00000008)	/* Error in Tx buffer      */
#define SOLOS_ETHER_STAT_MISSERR         (0x00000010)	/* Missed pkt count roll   */
#define SOLOS_ETHER_STAT_SQE             (0x00000020)	/* SQE error               */
#define SOLOS_ETHER_STAT_RXCRC           (0x00000100)	/* RXCRC count roll over   */
#define SOLOS_ETHER_STAT_RXFRAME         (0x00000200)	/* RX frame count roll ovr */
#define SOLOS_ETHER_STAT_RXOFLOW         (0x00000400)	/* RX overflow count roll  */
#define SOLOS_ETHER_STAT_MDIO            (0x00001000)	/* MDIO complete           */
#define SOLOS_ETHER_STAT_TXPOLL          (0x80000000)	/* Poll Tx descriptors     */

/* Interrupt Enable Register Masks */
/* Same masks as ISR above */

/* Control Register Bit Masks */
#define SOLOS_ETHER_CONTROL_ENABLE       (0x00000001)	/* Disable/Enable ethernet */
#define SOLOS_ETHER_CONTROL_TXRUN        (0x00000008)	/* Disable/Enable Tx       */
#define SOLOS_ETHER_CONTROL_RXRUN        (0x00000010)	/* Disable/Enable Rx       */
#define SOLOS_ETHER_CONTROL_RXINT2TX     (0x00000020)	/* Rx int activates Tx int */
#define SOLOS_ETHER_CONTROL_DISBDCST     (0x00000100)	/* Disable/Enb Rx b'casts  */
#define SOLOS_ETHER_CONTROL_DISMCAST     (0x00000200)	/* Disable/Enb Rx m'casts  */
#define SOLOS_ETHER_CONTROL_ENBFULL      (0x00000400)	/* Disable/Enb full duplex */
#define SOLOS_ETHER_CONTROL_PROM         (0x00000800)	/* Enable promiscuous mode */
#define SOLOS_ETHER_CONTROL_ENB2PART     (0x00001000)	/* Disable 2 part deferral */
#define SOLOS_ETHER_CONTROL_TEST         (0x00002000)	/* Silicon testing         */
#define SOLOS_ETHER_CONTROL_DISRETRY     (0x00004000)	/* Disable/Enb Tx retries  */
#define SOLOS_ETHER_CONTROL_DISADDFCS    (0x00008000)	/* Disable/Enb FCS append  */
#define SOLOS_ETHER_CONTROL_TXBDTLEN     (0x00FF0000)	/* No. BDTs Tx ring 1-255  */
#define SOLOS_ETHER_CONTROL_TXBDTLEN_SHIFT       (16)	/* bit shift  for TXBDTLEN */
#define SOLOS_ETHER_CONTROL_RXBDTLEN     (0xFF000000)	/* No. BDTs Rx ring 1-255  */
#define SOLOS_ETHER_CONTROL_RXBDTLEN_SHIFT       (24)	/* bit shift for RXBDTLEN  */

/* Poll Rate Register Bit Masks */
#define SOLOS_ETHER_POLLRATE_MASK        (0x0000FFFF)	/* Units = 1024 cycles     */

/* Rx Error Register Bit Masks */
#define SOLOS_ETHER_RXERR_RXCRC          (0x000000FF)	/* CRC errors count        */
#define SOLOS_ETHER_RXERR_RXFRAM         (0x0000FF00)	/* Frame errors count      */
#define SOLOS_ETHER_RXERR_RXOFLOW        (0x00FF0000)	/* Overflow errors count   */

/* Missed Packet Register Bit Mask */
#define SOLOS_ETHER_MISS_MASK            (0x000000FF)	/* Missed packet counter   */

/* Tx Ring Pointer Address Register Bit Mask */
#define SOLOS_ETHER_TXRINGPTR_MASK       (0xFFFFFFFF)	/* Address of Tx BDT ring  */

/* Rx Ring Pointer Address Register Bit Mask */
#define SOLOS_ETHER_RXRINGPTR_MASK       (0xFFFFFFFF)	/* Address of Rx BDT ring  */

/* Ethernet MAC Address Register Bit Masks */
#define SOLOS_ETHER_ADDRL_MASK           (0xFFFFFFFF)	/* Lower bits for 48-bit
							   ethernet MAC address */
#define SOLOS_ETHER_ADDRH_MASK           (0x0000FFFF)	/* Higher bits for 48-bit
							   ethernet MAC address */

/* Logical Address Filter Register Bit Masks */
#define SOLOS_ETHER_LAFL_MASK            (0xFFFFFFFF)	/* Lower bits of
							   Logical Address */
#define SOLOS_ETHER_LAFH_MASK            (0xFFFFFFFF)	/* Higher bits of
							   Logical Address */

/* MDIO_DATA register bits */
#define SOLOS_ETHER_MDIO_SFD             (0x40000000)
#define SOLOS_ETHER_MDIO_OP_WRITE        (0x10000000)
#define SOLOS_ETHER_MDIO_OP_READ         (0x20000000)
#define SOLOS_ETHER_MDIO_PHY_MASK        (0x0F800000)
#define SOLOS_ETHER_MDIO_PHY_SHIFT	     (23)
#define SOLOS_ETHER_MDIO_PHY_MASK_NOT    (~SOLOS_ETHER_MDIO_PHY_MASK)
#define SOLOS_ETHER_MDIO_REG_MASK        (0x007C0000)
#define SOLOS_ETHER_MDIO_REG_SHIFT       (18)
#define SOLOS_ETHER_MDIO_REG_MASK_NOT    (~SOLOS_ETHER_MDIO_REG_MASK)
#define SOLOS_ETHER_MDIO_TA              (0x00020000)
#define SOLOS_ETHER_MDIO_DATA_MASK       (0x0000FFFF)
#define SOLOS_ETHER_MDIO_DATA_MASK_NOT   (~SOLOS_ETHER_MDIO_DATA_MASK)

/* XTRACTRL register bits - CNXT modifications to VMAC */
#define SOLOS_ETHER_XTRACTRL_BVCIFIX     (0x00080000)	/* 1=Enb BVCI HBUSREQ fix  */
#define SOLOS_ETHER_XTRACTRL_TXAUTOPAD   (0x00040000)	/* 1=Auto Pad TX to 64-byte */
#define SOLOS_ETHER_XTRACTRL_RXBURST8    (0x00020000)	/* 1=8-word DMA writes     */
#define SOLOS_ETHER_XTRACTRL_TXBURST8    (0x00010000)	/* 1=8-word DMA reads      */
#define SOLOS_ETHER_XTRACTRL_TXDEPTH     (0x0000FC00)	/* log2(TX FIFO depth)     */
#define SOLOS_ETHER_XTRACTRL_TXDEPTH_SHIFT       (10)	/* bit shift  for TXDEPTH  */
#define SOLOS_ETHER_XTRACTRL_TXTHRESH    (0x000003FF)	/* TX FIFO start threshold */
#define SOLOS_ETHER_XTRACTRL_TXTHRESH_SHIFT       (0)	/* bit shift for TXTHRESH  */
#define SOLOS_ETHER_XTRACTRL_RESETMASK   (0xFFC00000)   /* MS bits to reset on startup */

/************************************************************************
 ** The #define's below relate to the descriptor tables, not hardware. **
 ************************************************************************/

/* Tx Control Information Written By the CPU - Bit Masks */

#define SOLOS_ETHER_TXINFO_CPU_TX_LEN (0x000007FF)	/* Tx Length in this buffer to be Xmitted */
#define SOLOS_ETHER_TXINFO_CPU_FIRST  (0x00010000)	/* First Tx buffer in the packet          */
#define SOLOS_ETHER_TXINFO_CPU_LAST   (0x00020000)	/* Last Tx buffer in the packet           */
#define SOLOS_ETHER_TXINFO_CPU_ADDCRC (0x00040000)	/* Add the CRC tp the pkt                 */
#define SOLOS_ETHER_TXINFO_CPU_OWN    (0x80000000)	/* CPU/VMAC Ownership of buffer           */

/* Tx Control Information Written By the VMAC - Bit Masks */

#define SOLOS_ETHER_TXINFO_TX_LEN     (0x000007FF)	/* Tx Length in this buffer to be Xmitted */
#define SOLOS_ETHER_TXINFO_FIRST      (0x00010000)	/* First Tx buffer in the packet          */
#define SOLOS_ETHER_TXINFO_LAST       (0x00020000)	/* Last Tx buffer in the packet           */
#define SOLOS_ETHER_TXINFO_ADDCRC     (0x00040000)	/* Add the CRC to the pkt that is Xmitted */
#define SOLOS_ETHER_TXINFO_CARR_LOSS  (0x00200000)	/* Carrier Lost during xmission           */
#define SOLOS_ETHER_TXINFO_DEFER      (0x00400000)	/* xmission deferred due to traffic       */
#define SOLOS_ETHER_TXINFO_DROPPED    (0x00800000)	/* pkt dropped after 16 retries           */
#define SOLOS_ETHER_TXINFO_RETRY      (0x0F000000)	/* Retry count for Tx                     */
#define SOLOS_ETHER_TXINFO_LATE_COLL  (0x10000000)	/* Late Collision                         */
#define SOLOS_ETHER_TXINFO_UFLO       (0x20000000)	/* Data not available on time             */
#define SOLOS_ETHER_TXINFO_BUFF       (0x40000000)	/* Buffer error - bad FIRST and LAST      */
#define SOLOS_ETHER_TXINFO_OWN        (0x80000000)	/* CPU/VMAC Ownership of buffer           */

/* Pointer To Tx Buffer Bit Mask */

#define SOLOS_ETHER_TX_BUFFER_PTR     (0xFFFFFFFF)	/* Address of start of the buffer of data */

/* Rx Control Information Written By the VMAC - Bit Masks */

#define SOLOS_ETHER_RXINFO_RX_LEN     (0x000007FF)	/* Rx Length in this buffer to be Xmitted */
#define SOLOS_ETHER_RXINFO_FIRST      (0x00010000)	/* First Rx buffer in the packet          */
#define SOLOS_ETHER_RXINFO_LAST       (0x00020000)	/* Last Rx buffer in the packet           */
#define SOLOS_ETHER_RXINFO_BUFF       (0x40000000)	/* Buffer chaining error                  */
#define SOLOS_ETHER_RXINFO_OWN        (0x80000000)	/* CPU/VMAC Ownership of buffer           */

/* Rx Control Information Written By the CPU - Bit Masks */

#define SOLOS_ETHER_RXINFO_CPU_RX_LEN (0x000007FF)	/* Rx Length in this buffer to be Xmitted */
#define SOLOS_ETHER_RXINFO_CPU_FIRST  (0x00010000)	/* First Rx buffer in the packet          */
#define SOLOS_ETHER_RXINFO_CPU_LAST   (0x00020000)	/* Last Rx buffer in the packet           */
#define SOLOS_ETHER_RXINFO_CPU_OWN    (0x80000000)	/* CPU/VMAC Ownership of buffer           */

/* Pointer To Rx Buffer Bit Mask */

#define SOLOS_ETHER_RX_BUFFER_PTR     (0xFFFFFFFF)	/* Address of start of the buffer of data */

/* Bits for Extra Ctrl register */
#define SOLOS_ETHER_XTRACTRL_CLRLOCKOUT  (0x04000000)  /* Reset rxbf_unload_lockout if enable=0*/
#define SOLOS_ETHER_XTRACTRL_DRAINENB    (0x02000000)  /* Enable Drain fix (broken?)    */
#define SOLOS_ETHER_XTRACTRL_MISSBUFDIS  (0x01000000)  /* Stop MISSED affecting RX FIFO */
#define SOLOS_ETHER_XTRACTRL_LOCKFIX     (0x00800000)  /* Auto lockup get-out-of-jail   */
#define SOLOS_ETHER_XTRACTRL_XTRAENB     (0x00400000)  /* Enable XTRA_STAT & CTRL bits  */
#define SOLOS_ETHER_XTRACTRL_PAUSEDIS    (0x00200000)  /* 1=Disable PAUSE RX      */
#define SOLOS_ETHER_XTRACTRL_PADDRDIS    (0x00100000)  /* 1=Disable PADDR decode  */
#define SOLOS_ETHER_XTRACTRL_BVCIFIX     (0x00080000)  /* 1=Enb BVCI HBUSREQ fix  */
#define SOLOS_ETHER_XTRACTRL_TXAUTOPAD   (0x00040000)  /* 1=Auto Pad TX to 64-byte*/
#define SOLOS_ETHER_XTRACTRL_RXBURST8    (0x00020000)  /* 1=8-word DMA writes     */
#define SOLOS_ETHER_XTRACTRL_TXBURST8    (0x00010000)  /* 1=8-word DMA reads      */
#define SOLOS_ETHER_XTRACTRL_TXDEPTH     (0x0000FC00)  /* log2(TX FIFO depth)     */
#define SOLOS_ETHER_XTRACTRL_TXDEPTH_SHIFT       (10)  /* bit shift  for TXDEPTH  */
#define SOLOS_ETHER_XTRACTRL_TXTHRESH    (0x000003FF)  /* TX FIFO start threshold */
#define SOLOS_ETHER_XTRACTRL_TXTHRESH_SHIFT       (0)  /* bit shift for TXTHRESH  */


#endif
