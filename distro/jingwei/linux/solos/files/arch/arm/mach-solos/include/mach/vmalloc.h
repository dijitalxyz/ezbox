/*
 * linux/include/asm-arm/arch-solos/vmalloc.h
 *
 * Copyright (c) 2004 Conexant
 *
 */
#ifndef __ASM_ARCH_VMALLOC_H
#define __ASM_ARCH_VMALLOC_H

/* OK, so if I understand correctly... */

/* This is the gap between the kernel map of RAM and the 
 * start of the vmalloc area. Just there to trap undefined 
 * accessed. Everyone seems happy with 1M */
#define VMALLOC_OFFSET    (8*1024*1024)

/* Aligned start address calculated from above... */
#define VMALLOC_START     (((unsigned long)high_memory + VMALLOC_OFFSET) & ~(VMALLOC_OFFSET-1))

/* Maybe this isn't always an identity, but I can't see why... */
#define VMALLOC_VMADDR(x) ((unsigned long)(x))

/* There is a limit to how much memory we can address */
#define VMALLOC_END       (PAGE_OFFSET + 0x10000000)

#endif
