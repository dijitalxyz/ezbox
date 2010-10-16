/*
 * linux/include/asm-arm/arch-solos/hardware/msc16.h
 *
 * Copyright (C) 2005 Conexant inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
 
/* Definitions for the MSC16 controllers */

#include "asm/hardware.h"

/* AHB addresses of the MSC16s */
#define SOLOS_DSL_MSC16        (SOLOS_UAHB_VIRT + 0x0600)
#define SOLOS_NXM_MSC16        (SOLOS_UAHB_VIRT + 0x0f00)
#define SOLOS_CRYPT_MSC16      (SOLOS_UAHB_VIRT + 0x0d00)

/* Register offsets within each MSC16 */
#define MSC16_CTRL_REG (0x00)
#define MSC16_ADDR_REG (0x04)
#define MSC16_READ_REG (0x08)
#define MSC16_WRITE_REG (0x0c)
#define MSC16_TEST_REG (0x10)
#define MSC16_BP0_REG (0x14)
#define MSC16_BP1_REG (0x18)
#define MSC16_BP2_REG (0x1c)
#define MSC16_BP3_REG (0x20)

/* Definitions for CTRL */
#define MSC16_CTRL_INTEN        (0x00000001) /* Overall IRQ enable        */
#define MSC16_CTRL_HALTINTEN    (0x00000002) /* Halt IRQ enable           */
#define MSC16_CTRL_BPINTEN      (0x00000004) /* Breakpoint IRQ enable     */
#define MSC16_CTRL_STAT0INTEN   (0x00000008) /* Status 0 IRQ enable       */
#define MSC16_CTRL_STAT1INTEN   (0x00000010) /* Status 1 IRQ enable       */
#define MSC16_CTRL_STAT2INTEN   (0x00000020) /* Status 2 IRQ enable       */
#define MSC16_CTRL_STAT3INTEN   (0x00000040) /* Status 3 IRQ enable       */
#define MSC16_CTRL_PCLD         (0x00000080) /* Load PC from addr reg.    */
#define MSC16_CTRL_START        (0x00000100) /* Start the MSC16           */
#define MSC16_CTRL_STOP         (0x00000200) /* Stop the MSC16            */
#define MSC16_CTRL_STEP         (0x00000400) /* Step the MSC16            */
#define MSC16_CTRL_CONT         (0x00000800) /* Continue the MSC16        */
#define MSC16_CTRL_CLEAR        (0x00001000) /*                           */
#define MSC16_CTRL_ATT0         (0x00002000) /* MSC16 Attention 0 Flag    */
#define MSC16_CTRL_ATT1         (0x00004000) /* MSC16 Attention 1 Flag    */
#define MSC16_CTRL_ATT2         (0x00008000) /* MSC16 Attention 2 Flag    */
#define MSC16_CTRL_ATT3         (0x00010000) /* MSC16 Attention 3 Flag    */
#define MSC16_CTRL_STAT0        (0x00020000) /* MSC16 Status 0 Flag       */
#define MSC16_CTRL_STAT1        (0x00040000) /* MSC16 Status 0 Flag       */
#define MSC16_CTRL_STAT2        (0x00080000) /* MSC16 Status 0 Flag       */
#define MSC16_CTRL_STAT3        (0x00100000) /* MSC16 Status 0 Flag       */
#define MSC16_CTRL_HALT         (0x00200000) /* MSC16 HALT Status         */
#define MSC16_CTRL_WAIT         (0x00400000) /* MSC16 WAIT Status         */
#define MSC16_CTRL_BUSY         (0x00800000) /* MSC16 BUSY Status         */
#define MSC16_CTRL_BP           (0x01000000) /* Breakpoint detect status  */
#define MSC16_CTRL_DATAAV       (0x02000000) /* Data available status     */
#define MSC16_CTRL_SPACEAV      (0x04000000) /* Space available status    */
#define MSC16_CTRL_HALTINT      (0x10000000) /* HALT IRQ raised           */
#define MSC16_CTRL_BPINT        (0x20000000) /* Breakpoint IRQ raised     */
#define MSC16_CTRL_STATINT      (0x40000000) /* Status IRQ raised         */
#define MSC16_CTRL_INT          (0x80000000) /* IRQ is raised             */

/* Definitions for ADDR */
#define MSC16_ADDR_MODE_PC       (0x00000000) /* Target is PC (write only)            */
#define MSC16_ADDR_MODE_REG      (0x00020000) /* Target is register file (read only ) */
#define MSC16_ADDR_MODE_BP       (0x00030000) /* Target is breakpoint regs (R/W) */
#define MSC16_ADDR_MODE_PERIPH   (0x00040000) /* Target is periph. I/O space */
#define MSC16_ADDR_MODE_IMEM     (0x00050000) /* Target is instruction memory */
#define MSC16_ADDR_MODE_DMEM     (0x00060000) /* Target is data memory (16-bit access ) */
#define MSC16_ADDR_MODE_DMEM32   (0x00070000) /* Target is data memory (32-bit access ) */
#define MSC16_ADDR_AUTOREAD     (0x00080000) /* Autoread (used with autoincrement for block reads) */
#define MSC16_ADDR_AUTOINC      (0x00100000) /* Autoincrement address */
#define MSC16_ADDR_WRITE        (0x40000000) /* Perform write access */
#define MSC16_ADDR_READ         (0x80000000) /* Perform read access  */

/* Definitions for Breakpoint registers */
#define MSC16_BP_ADDRMASK       (0x0000ffff)
#define MSC16_BP_INST           (0x00010000) /* Match on instruction access */
#define MSC16_BP_DATA           (0x00020000) /* Match on data access */
#define MSC16_BP_PERIPH         (0x00030000) /* Match on periph I/O access */
#define MSC16_BP_HIT            (0x80000000) /* This breakpoint has been hit */


#define MSC16_ADMA_REG   0x00    /* 0x00-0x06 Base for register addresses  */

#define MSC16_ADMA_CTRL          (MSC16_ADMA_REG+0x00) 
/* Bit definitions for the DMA control register */
#define MSC16_ADMA_CTRL_RESET                 (0x0001)  /* Reset DMA control logic          */
#define MSC16_ADMA_CTRL_READ                  (0x0002)  /* Start DMA Read cycle             */
#define MSC16_ADMA_CTRL_WRITE                 (0x0004)  /* Start DMA Write cycle            */
#define MSC16_ADMA_CTRL_ENABLE                (0x0008)  /* Enable DMA Cycles                */
#define MSC16_ADMA_CTRL_GSENABLE              (0x0010)  /* Enable Global status             */
#define MSC16_ADMA_CTRL_SPACEAV               (0x2000)  /* Space available in write FIFO    */
#define MSC16_ADMA_CTRL_DATAAV                (0x4000)  /* Data available in read FIFO      */
#define MSC16_ADMA_CTRL_READY                 (0x8000)  /* DMA ready to accept new address  */

