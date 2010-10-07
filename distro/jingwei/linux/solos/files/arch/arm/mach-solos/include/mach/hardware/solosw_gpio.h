/* ###########################################################################
# (c) Copyright Conexant Systems, Inc. 2004
#
# Conexant Systems, Inc. Confidential and Proprietary
#
# The following software source code ("Software") is strictly confidential and
# is proprietary to Conexant Systems, Inc. and its subsidiaries (collectively,
# "Conexant"). It may only be read, used, copied, adapted, modified or 
# otherwise dealt with by you if you have entered into a license agreement 
# with Conexant and then subject to the terms of that agreement and any other 
# applicable confidentiality agreement between you and Conexant.
#
# If you are in any doubt as to whether you are entitled to access, read,
# use, copy, adapt, modify or otherwise deal with the Software or whether you
# are entitled to disclose the Software to any other person you should
# contact Conexant.  If you have not entered into a license agreement
# with Conexant granting access to this Software you should forthwith return
# all media, copies and printed listings containing the Software to Conexant.
#
# Conexant reserves the right to take legal action against you should you
# breach the above provisions.
#
# If you are unsure, or to report violations, please contact
# licensee.support@conexant.com
# ##########################################################################*/

/****************************************************************************
 *  solosw_gpio.h                        Last Modified   -       28/08/06  *
 *****************************************************************************/

/****************************************************************************/
/*                                                                                  */
/* solosw_gpio.h                                                                   */
/*                                                                                  */
/* SOLOS WLC GPIO Hardware Definitions                              */
/*                                                                                  */
/****************************************************************************/
/*******************************************************************************
 *
 *                              I N C L U D E   F I L E S
 *
 *******************************************************************************/
#include <stddef.h>
#include <linux/autoconf.h>
#include <mach/hardware.h>

/*******************************************************************************
 *
 *                            L O C A L   C O N S T A N T S
 *
 *******************************************************************************/
#ifndef SOLOSW_GPIO_H
#define SOLOSW_GPIO_H

#ifndef SOLOSW_GPIO_NUMS
#ifdef CONFIG_ARCH_SOLOSW_GALLUS
#define SOLOSW_GPIO_NUMS (23)              /* Number of GPIO pins in Gallus WLC */
#else
#define SOLOSW_GPIO_NUMS (48)              /* Number of GPIO pins in Gallus */
#endif
#endif

#ifdef CONFIG_ARCH_SOLOSW_GALLUS

/* The GPIO assignment for GallusW */

#define SOLOSW_GPIO_3             (3)   /* USB-LED */
#define SOLOSW_GPIO_7             (7)   /* USB_EXT_PHY_RESET */
#define SOLOSW_GPIO_8             (8)   /* EECK / EPHY_Mode*/
#define SOLOSW_GPIO_13            (13)  /* SPI cs[0]  */
#define SOLOSW_GPIO_14            (14)
#ifdef CONFIG_SOLOS_GPIO_FOUR
#define SOLOSW_GPIO_PHY_RESET     (4)    /* PHY_RESET_GPIO */
#else
#define SOLOSW_GPIO_PHY_RESET     (12)   /* PHY_RESET_GPIO */
#endif
#define SOLOSW_STUN_GPIO_4        (22)  /* Stun GPIO 4 using for Wireless LED */

/* Default mode of GPIO */
#define GPIO_0_MODE                    SOLOSW_GPIO_CONTROL_OUTPUT      /* Internet LED G   */
#define GPIO_1_MODE                    SOLOSW_GPIO_CONTROL_OUTPUT      /* Internet LED R   */
#define GPIO_2_MODE                    SOLOSW_GPIO_CONTROL_OUTPUT      /* DSL LED G        */
#define GPIO_3_MODE                    SOLOSW_GPIO_CONTROL_OUTPUT      /* USB LED G        */
#define GPIO_4_MODE                    SOLOSW_GPIO_CONTROL_OUTPUT      /* EPHY Reset       */
#define GPIO_5_MODE                    SOLOSW_GPIO_CONTROL_INPUT    /* Default_restore  */
#define GPIO_6_MODE                    SOLOSW_GPIO_CONTROL_INPUT       /* External CHIP_ID */
#define GPIO_7_MODE                    SOLOSW_GPIO_CONTROL_OUTPUT      /* EECS / USB_Mode  */
#define GPIO_8_MODE                    SOLOSW_GPIO_CONTROL_FUNCTION0   /* EECK / EPHY_Mode */
#define GPIO_9_MODE                    SOLOSW_GPIO_CONTROL_OUTPUT      /* Voice Relay En.  */
#define GPIO_10_MODE                   SOLOSW_GPIO_CONTROL_OUTPUT      /* Voice LED 0      */
#define GPIO_11_MODE                   SOLOSW_GPIO_CONTROL_OUTPUT      /* Voice LED 1      */
#define GPIO_12_MODE                   SOLOSW_GPIO_CONTROL_OUTPUT      /* Voice Reset      */
#define GPIO_13_MODE                   SOLOSW_GPIO_CONTROL_FUNCTION0   /* SPI cs[0]        */
#define GPIO_14_MODE                   SOLOSW_GPIO_CONTROL_FUNCTION0   /* SPI cs[1]        */
#define GPIO_15_MODE                   SOLOSW_GPIO_CONTROL_INPUT      /* SPI_Vo_INT       */
#define GPIO_16_MODE                   SOLOSW_GPIO_CONTROL_OUTPUT      /* EEDO             */
#define GPIO_17_MODE                   SOLOSW_GPIO_CONTROL_INPUT       /* EEDI             */



#define SOLOSW_GPIO_TRUE_GPIOS (17)
#define SOLOSW_GPIO_STUN_GPIOS (5)
#define SOLOSW_GPIO_STUN_GPIO_INPUT_BIT  (0)
#define SOLOSW_GPIO_STUN_GPIO_MODE_BITS  (1)
#define SOLOSW_GPIO_STUN_GPIO_OUTPUT_BIT (3)
#define SOLOSW_GPIO_STUN_INPUT (0)
#define SOLOSW_GPIO_STUN_BI (1)
#define SOLOSW_GPIO_STUN_OUTPUT (2)
#define SOLOSW_GPIO_STUN_TEST (3)


#endif


#define SOLOSW_GPIO_BASE_ADDR      (SOLOS_LAHB_VIRT + 0x100) /* Base Address of GPIO block in SOLOS WLC */

/* debug stuff */

//#define SOLOSW_GPIO_DEBUG 1

#ifdef SOLOSW_GPIO_DEBUG
#define PRINTK printk
#else    
#define PRINTK(...)
#endif

#define SOLOSW_GPIO_CONTROL_BITS_PER_PIN      (4)
#define SOLOSW_GPIO_CONTROL_PINS_PER_REG    (32 / SOLOSW_GPIO_CONTROL_BITS_PER_PIN) 
#define SOLOSW_GPIO_CONTROL_MASK                    (0xF)

/* CONTROL register configuration values : 
   Note That FUNCTION0 to FUNCTION6 are not supported 
   for pin no 16 & 17 
 */
#define SOLOSW_GPIO_CONTROL_INPUT          (0x1) /* Normal input (default setting)   */
#define SOLOSW_GPIO_CONTROL_INVINPUT       (0x2) /* Inverted input               */
#define SOLOSW_GPIO_CONTROL_FUNCTION0      (0x3) /* Special function 0           */
#define SOLOSW_GPIO_CONTROL_FUNCTION1      (0x4) /* Special function 1           */
#define SOLOSW_GPIO_CONTROL_FUNCTION2      (0x5) /* Special function 2           */
#define SOLOSW_GPIO_CONTROL_FUNCTION3      (0x6) /* Special function 3          */
#define SOLOSW_GPIO_CONTROL_FUNCTION4      (0x7) /* Special function 4           */
#define SOLOSW_GPIO_CONTROL_FUNCTION5      (0x8) /* Special function 5          */
#define SOLOSW_GPIO_CONTROL_FUNCTION6      (0x9) /* Special function 6           */
#define SOLOSW_GPIO_CONTROL_RESERVED1      (0xa) /* Reserved              */
#define SOLOSW_GPIO_CONTROL_RESERVED2      (0xb) /* Reserved               */
#define SOLOSW_GPIO_CONTROL_OUTPUT         (0xc) /* Normal output                */
#define SOLOSW_GPIO_CONTROL_INVOUTPUT      (0xd) /* Inverted output              */
#define SOLOSW_GPIO_CONTROL_OPENDRAIN      (0xe) /* Open drain output            */
#define SOLOSW_GPIO_CONTROL_OPENSOURCE     (0xf) /* Open source output           */
#define SOLOSW_GPIO_NUM_MODES        SOLOSW_GPIO_CONTROL_OPENSOURCE    

/* Convenient types */
typedef enum {
    INPUT          = SOLOSW_GPIO_CONTROL_INPUT,       
    INVINPUT       = SOLOSW_GPIO_CONTROL_INVINPUT,   
    FUNCTION0      = SOLOSW_GPIO_CONTROL_FUNCTION0, 
    FUNCTION1      = SOLOSW_GPIO_CONTROL_FUNCTION1,
    FUNCTION2      = SOLOSW_GPIO_CONTROL_FUNCTION2, 
    FUNCTION3      = SOLOSW_GPIO_CONTROL_FUNCTION3,
    FUNCTION4      = SOLOSW_GPIO_CONTROL_FUNCTION4,
    FUNCTION5   = SOLOSW_GPIO_CONTROL_FUNCTION5,
    FUNCTION6   = SOLOSW_GPIO_CONTROL_FUNCTION6,
    OUTPUT         = SOLOSW_GPIO_CONTROL_OUTPUT,   
    INVOUTPUT      = SOLOSW_GPIO_CONTROL_INVOUTPUT, 
    OPENDRAIN      = SOLOSW_GPIO_CONTROL_OPENDRAIN,
    OPENSOURCE     = SOLOSW_GPIO_CONTROL_OPENSOURCE
} solosw_gpio_mode_t;

/* GPIO CONTROL1 Register bit definitions  */
#define SOLOSW_GPIO_CONTROL1_GPIO0      (0xF << 0)     /* GPIO line 0 control val*/
#define SOLOSW_GPIO_CONTROL1_GPIO1      (0xF << 4)     /* GPIO line 1 control val*/
#define SOLOSW_GPIO_CONTROL1_GPIO2      (0xF << 8)     /* GPIO line 2 control val*/
#define SOLOSW_GPIO_CONTROL1_GPIO3      (0xF << 12)     /* GPIO line 3 control val*/
#define SOLOSW_GPIO_CONTROL1_GPIO4      (0xF << 16)     /* GPIO line 4 control val*/
#define SOLOSW_GPIO_CONTROL1_GPIO5      (0xF << 20)     /* GPIO line 5 control val*/
#define SOLOSW_GPIO_CONTROL1_GPIO6      (0xF << 24)    /* GPIO line 6 control val*/
#define SOLOSW_GPIO_CONTROL1_GPIO7      (0xF << 28)    /* GPIO line 7 control val*/

/* GPIO CONTROL2 Register bit definitions  */
#define SOLOSW_GPIO_CONTROL2_GPIO8      (0xF << 0)     /* GPIO line 8 control val*/
#define SOLOSW_GPIO_CONTROL2_GPIO9      (0xF << 4)     /* GPIO line 9 control val*/
#define SOLOSW_GPIO_CONTROL2_GPIO10  (0xF << 8)    /* GPIO line 10 control val*/
#define SOLOSW_GPIO_CONTROL2_GPIO11  (0xF << 12)     /* GPIO line 11 control val*/
#define SOLOSW_GPIO_CONTROL2_GPIO12  (0xF << 16)     /* GPIO line 12 control val*/
#define SOLOSW_GPIO_CONTROL2_GPIO13  (0xF << 20)     /* GPIO line 13 control val*/
#define SOLOSW_GPIO_CONTROL2_GPIO14  (0xF << 24)     /* GPIO line 14 control val*/
#define SOLOSW_GPIO_CONTROL2_GPIO15  (0xF << 28)     /* GPIO line 15 control val*/

/* GPIO CONTROL3 Register bit definitions : only 0-7 bits are used  */
#define SOLOSW_GPIO_CONTROL3_GPIO16  (0xF << 0)     /* GPIO line 16 control val*/
#define SOLOSW_GPIO_CONTROL3_GPIO17  (0xF << 4)     /* GPIO line 17 control val*/

#define SOLOSW_GPIO_OUTPUT_BITS_PER_PIN        (1)
#define SOLOSW_GPIO_OUTPUT_PINS_PER_REG        (32 / SOLOSW_GPIO_OUTPUT_BITS_PER_PIN)
#define SOLOSW_GPIO_OUTPUT_MASK                    (1)
/* GPIO OUTPUT1 Register bit definitions : only 0-17 bits are used  */
#define SOLOSW_GPIO_OUTPUT1_GPIO0      (0x1 << 0)     /* GPIO line 0 output val*/
#define SOLOSW_GPIO_OUTPUT1_GPIO1      (0x1 << 1)     /* GPIO line 1 output val*/
#define SOLOSW_GPIO_OUTPUT1_GPIO2      (0x1 << 2)     /* GPIO line 2 output val*/
#define SOLOSW_GPIO_OUTPUT1_GPIO3      (0x1 << 3)     /* GPIO line 3 output val*/
#define SOLOSW_GPIO_OUTPUT1_GPIO4      (0x1 << 4)     /* GPIO line 4 output val*/
#define SOLOSW_GPIO_OUTPUT1_GPIO5      (0x1 << 5)     /* GPIO line 5 output val*/
#define SOLOSW_GPIO_OUTPUT1_GPIO6      (0x1 << 6)    /* GPIO line 6 output val*/
#define SOLOSW_GPIO_OUTPUT1_GPIO7      (0x1 << 7)    /* GPIO line 7 output val*/
#define SOLOSW_GPIO_OUTPUT1_GPIO8      (0x1 << 8)     /* GPIO line 8 output val*/
#define SOLOSW_GPIO_OUTPUT1_GPIO9      (0x1 << 8)     /* GPIO line 9 output val*/
#define SOLOSW_GPIO_OUTPUT1_GPIO10      (0x1 << 10)    /* GPIO line 10 output val*/
#define SOLOSW_GPIO_OUTPUT1_GPIO11      (0x1 << 11)     /* GPIO line 11 output val*/
#define SOLOSW_GPIO_OUTPUT1_GPIO12      (0x1 << 12)     /* GPIO line 12 output val*/
#define SOLOSW_GPIO_OUTPUT1_GPIO13      (0x1 << 13)     /* GPIO line 13 output val*/
#define SOLOSW_GPIO_OUTPUT1_GPIO14      (0x1 << 14)     /* GPIO line 14 output val*/
#define SOLOSW_GPIO_OUTPUT1_GPIO15      (0x1 << 15)     /* GPIO line 15 output val*/
#define SOLOSW_GPIO_OUTPUT1_GPIO16      (0x1 << 16)     /* GPIO line 16 output val*/
#define SOLOSW_GPIO_OUTPUT1_GPIO17      (0x1 << 17)     /* GPIO line 17 output val*/

#define SOLOSW_GPIO_INPUT_BITS_PER_PIN      (1)
#define SOLOSW_GPIO_INPUT_PINS_PER_REG    (32 / SOLOSW_GPIO_INPUT_BITS_PER_PIN)
#define SOLOSW_GPIO_INPUT_MASK              (1)
/* GPIO INPUT1 Register bit definitions : only 0-17 bits are used  */
#define SOLOSW_GPIO_INPUT1_GPIO0      (0x1 << 0)     /* GPIO line 0 input val*/
#define SOLOSW_GPIO_INPUT1_GPIO1      (0x1 << 1)     /* GPIO line 1 input val*/
#define SOLOSW_GPIO_INPUT1_GPIO2      (0x1 << 2)     /* GPIO line 2 input val*/
#define SOLOSW_GPIO_INPUT1_GPIO3      (0x1 << 3)     /* GPIO line 3 input val*/
#define SOLOSW_GPIO_INPUT1_GPIO4      (0x1 << 4)     /* GPIO line 4 input val*/
#define SOLOSW_GPIO_INPUT1_GPIO5      (0x1 << 5)     /* GPIO line 5 input val*/
#define SOLOSW_GPIO_INPUT1_GPIO6      (0x1 << 6)    /* GPIO line 6 input val*/
#define SOLOSW_GPIO_INPUT1_GPIO7      (0x1 << 7)    /* GPIO line 7 input val*/
#define SOLOSW_GPIO_INPUT1_GPIO8      (0x1 << 8)     /* GPIO line 8 input val*/
#define SOLOSW_GPIO_INPUT1_GPIO9      (0x1 << 8)     /* GPIO line 9 input val*/
#define SOLOSW_GPIO_INPUT1_GPIO10      (0x1 << 10)    /* GPIO line 10 input val*/
#define SOLOSW_GPIO_INPUT1_GPIO11      (0x1 << 11)     /* GPIO line 11 input val*/
#define SOLOSW_GPIO_INPUT1_GPIO12      (0x1 << 12)     /* GPIO line 12 input val*/
#define SOLOSW_GPIO_INPUT1_GPIO13      (0x1 << 13)     /* GPIO line 13 input val*/
#define SOLOSW_GPIO_INPUT1_GPIO14      (0x1 << 14)     /* GPIO line 14 input val*/
#define SOLOSW_GPIO_INPUT1_GPIO15      (0x1 << 15)     /* GPIO line 15 input val*/
#define SOLOSW_GPIO_INPUT1_GPIO16      (0x1 << 16)     /* GPIO line 16 input val*/
#define SOLOSW_GPIO_INPUT1_GPIO17      (0x1 << 17)     /* GPIO line 17 input val*/

#define SOLOSW_GPIO_SET_BITS_PER_PIN        (1)
#define SOLOSW_GPIO_SET_PINS_PER_REG       (32 / SOLOSW_GPIO_SET_BITS_PER_PIN)
#define SOLOSW_GPIO_SET_MASK                     (1)
/* GPIO SET1 Register bit definitions : only 0-17 bits are used  */
#define SOLOSW_GPIO_SET1_GPIO0      (0x1 << 0)     /* GPIO line 0 set val*/
#define SOLOSW_GPIO_SET1_GPIO1      (0x1 << 1)     /* GPIO line 1 set val*/
#define SOLOSW_GPIO_SET1_GPIO2      (0x1 << 2)     /* GPIO line 2 set val*/
#define SOLOSW_GPIO_SET1_GPIO3      (0x1 << 3)     /* GPIO line 3 set val*/
#define SOLOSW_GPIO_SET1_GPIO4      (0x1 << 4)     /* GPIO line 4 set val*/
#define SOLOSW_GPIO_SET1_GPIO5      (0x1 << 5)     /* GPIO line 5 set val*/
#define SOLOSW_GPIO_SET1_GPIO6      (0x1 << 6)    /* GPIO line 6 set val*/
#define SOLOSW_GPIO_SET1_GPIO7      (0x1 << 7)    /* GPIO line 7 set val*/
#define SOLOSW_GPIO_SET1_GPIO8      (0x1 << 8)     /* GPIO line 8 set val*/
#define SOLOSW_GPIO_SET1_GPIO9      (0x1 << 8)     /* GPIO line 9 set val*/
#define SOLOSW_GPIO_SET1_GPIO10      (0x1 << 10)    /* GPIO line 10 set val*/
#define SOLOSW_GPIO_SET1_GPIO11      (0x1 << 11)     /* GPIO line 11 set val*/
#define SOLOSW_GPIO_SET1_GPIO12      (0x1 << 12)     /* GPIO line 12 set val*/
#define SOLOSW_GPIO_SET1_GPIO13      (0x1 << 13)     /* GPIO line 13 set val*/
#define SOLOSW_GPIO_SET1_GPIO14      (0x1 << 14)     /* GPIO line 14 set val*/
#define SOLOSW_GPIO_SET1_GPIO15      (0x1 << 15)     /* GPIO line 15 set val*/
#define SOLOSW_GPIO_SET1_GPIO16      (0x1 << 16)     /* GPIO line 16 set val*/
#define SOLOSW_GPIO_SET1_GPIO17      (0x1 << 17)     /* GPIO line 17 set val*/

#define SOLOSW_GPIO_CLEAR_BITS_PER_PIN    (1)
#define SOLOSW_GPIO_CLEAR_PINS_PER_REG   (32 / SOLOSW_GPIO_CLEAR_BITS_PER_PIN)
#define SOLOSW_GPIO_CLEAR_MASK                 (1)
/* GPIO CLEAR1 Register bit definitions : only 0-17 bits are used  */
#define SOLOSW_GPIO_CLEAR1_GPIO0      (0x1 << 0)     /* GPIO line 0 clear val*/
#define SOLOSW_GPIO_CLEAR1_GPIO1      (0x1 << 1)     /* GPIO line 1 clear val*/
#define SOLOSW_GPIO_CLEAR1_GPIO2      (0x1 << 2)     /* GPIO line 2 clear val*/
#define SOLOSW_GPIO_CLEAR1_GPIO3      (0x1 << 3)     /* GPIO line 3 clear val*/
#define SOLOSW_GPIO_CLEAR1_GPIO4      (0x1 << 4)     /* GPIO line 4 clear val*/
#define SOLOSW_GPIO_CLEAR1_GPIO5      (0x1 << 5)     /* GPIO line 5 clear val*/
#define SOLOSW_GPIO_CLEAR1_GPIO6      (0x1 << 6)    /* GPIO line 6 clear val*/
#define SOLOSW_GPIO_CLEAR1_GPIO7      (0x1 << 7)    /* GPIO line 7 clear val*/
#define SOLOSW_GPIO_CLEAR1_GPIO8      (0x1 << 8)     /* GPIO line 8 clear val*/
#define SOLOSW_GPIO_CLEAR1_GPIO9      (0x1 << 8)     /* GPIO line 9 clear val*/
#define SOLOSW_GPIO_CLEAR1_GPIO10      (0x1 << 10)    /* GPIO line 10 clear val*/
#define SOLOSW_GPIO_CLEAR1_GPIO11      (0x1 << 11)     /* GPIO line 11 clear val*/
#define SOLOSW_GPIO_CLEAR1_GPIO12      (0x1 << 12)     /* GPIO line 12 clear val*/
#define SOLOSW_GPIO_CLEAR1_GPIO13      (0x1 << 13)     /* GPIO line 13 clear val*/
#define SOLOSW_GPIO_CLEAR1_GPIO14      (0x1 << 14)     /* GPIO line 14 clear val*/
#define SOLOSW_GPIO_CLEAR1_GPIO15      (0x1 << 15)     /* GPIO line 15 clear val*/
#define SOLOSW_GPIO_CLEAR1_GPIO16      (0x1 << 16)     /* GPIO line 16 clear val*/
#define SOLOSW_GPIO_CLEAR1_GPIO17      (0x1 << 17)     /* GPIO line 17 clear val*/

#define SOLOSW_GPIO_OFFSET(reg,gpioNum)        ((gpioNum) / SOLOSW_GPIO_##reg##_PINS_PER_REG)        /* Gets the offset of the REG no */
#define SOLOSW_GPIO_REG_NUM(reg,gpioNum)    (((gpioNum) / SOLOSW_GPIO_##reg##_PINS_PER_REG)+1)
#define SOLOSW_GPIO_SHIFT(reg,gpioNum)        (((gpioNum) % SOLOSW_GPIO_##reg##_PINS_PER_REG) * SOLOSW_GPIO_##reg##_BITS_PER_PIN)
#define SOLOSW_GPIO_MASK(reg,gpioNum)        (SOLOSW_GPIO_##reg##_MASK << SOLOSW_GPIO_SHIFT(reg,gpioNum))

/* Read write register for GPIO pin */
#define REGIO_GPIO(reg, gpioNum)    *(volatile U32*)(SOLOSW_GPIO_BASE_ADDR  + offsetof(SOLOSW_GPIO_REGS, reg##1) + (sizeof(U32)*SOLOSW_GPIO_OFFSET(reg, gpioNum)))

/* BF_IOCONF_REGS Register */
typedef volatile struct {
    unsigned CONFIG0; /* @0 */
    unsigned CONFIG1; /* @4 */
    unsigned CONFIG2; /* @8 */
    unsigned CONFIG3; /* @12 */
    unsigned CONFIG4; /* @16 */
    unsigned CONFIG5; /* @20 */
    unsigned CONFIG6; /* @24 */
    unsigned CONFIG7; /* @28 */
    unsigned CONFIG8; /* @32 */
    unsigned CONFIG9; /* @36 */
    unsigned CONFIG10; /* @40 */
    unsigned CONFIG11; /* @44 */
    unsigned CONFIG12; /* @48 */
    unsigned CONFIG13; /* @52 */
    unsigned CONFIG14; /* @56 */
    unsigned CONFIG15; /* @60 */
    unsigned CONFIG16; /* @64 */
    unsigned CONFIG17; /* @68 */
    unsigned fill0;
    unsigned CONFIG19; /* @76 */
    unsigned CONFIG20; /* @80 */
    unsigned CONFIG21; /* @84 */
    unsigned CONFIG22; /* @88 */
    unsigned CONFIG23; /* @92 */
    unsigned fill1;
    unsigned fill2;
    unsigned fill3;
    unsigned fill4;
    unsigned fill5;
    unsigned fill6;
    unsigned fill7;
    unsigned fill8;
    unsigned PPU_EN; /* @128 */
    unsigned PPD_EN; /* @132 */
    unsigned DRIVE; /* @136 */
} BF_IOCONF_REGS;
#define TRUE (!0)
#define FALSE (0)


#define BF_IOCONF  SOLOS_LAHB_VIRT_CONF(0x40000600)  /* IO config                             */
#define REGIO(_dev, _reg) ( ((BF_##_dev##_REGS *)(BF_##_dev))->_reg )


/* BF_IOCONF_REGS Register */
#define REGIOADDR_GPIO(reg, gpioNum)        (volatile U32*)(SOLOSW_GPIO_BASE_ADDR  + offsetof(SOLOSW_GPIO_REGS, reg##1) + (sizeof(U32)*SOLOSW_GPIO_OFFSET(reg, gpioNum)))


/* Function return codes */ 
#define SOLOSW_GPIO_SUCCESS         0 
#define SOLOSW_GPIO_PIN_FREE         1
#define SOLOSW_GPIO_INVGPIO          2
#define SOLOSW_GPIO_INVMODE         3
#define SOLOSW_GPIO_INVARG           4
#define SOLOSW_GPIO_INVLEVEL         5
#define SOLOSW_GPIO_PIN_RESERVED     6
#define SOLOSW_GPIO_NOT_SREQ         7
#define SOLOSW_GPIO_NOT_INTR         8
#define SOLOSW_GPIO_NOT_IMPL         9 /* Not implemented */

#define INIT_SOLOSW_GPIO_LOCKING              sema_init(&solosw_gpio_sem, 1)
#define LOCK_SOLOSW_GPIO_FOR_WRITING     down_interruptible(&solosw_gpio_sem)
#define UNLOCK_SOLOSW_GPIO_FOR_WRITING     up(&solosw_gpio_sem)
#endif /* CONSERVATIVE_STACKER_LOCKING */

/*******************************************************************************
 *
 *                               L O C A L   T Y P E S
 *
 *******************************************************************************/
#if 1
/* data type definitions */
#define U32 unsigned int
#define S32 signed int
#define I32 long
#define U16 unsigned short
#define S16 signed short
#define I16 short
#define U8 unsigned char
#define S8 signed char
#define I8 char
#endif

/* C structure mapping onto the GPIO interface registers */
typedef volatile struct {
    unsigned CONTROL1;     /* 0x00 GPIO control register 1         */
    unsigned CONTROL2;     /* 0x04 GPIO control register 2         */
    unsigned CONTROL3;     /* 0x08 GPIO control register 3         */
    unsigned CONTROL4;     /* 0x0c GPIO control register 4         */
    unsigned CONTROL5;     /* 0x10 GPIO control register 5         */
    unsigned CONTROL6;     /* 0x14 GPIO control register 6         */
    unsigned CONTROL7;     /* 0x18 GPIO control register 7         */
    unsigned CONTROL8;     /* 0x1c GPIO control register 8         */

    unsigned _reserved_20; /* 0x20 reserved */
    unsigned _reserved_24; /* 0x24 reserved */
    unsigned _reserved_28; /* 0x28 reserved */
    unsigned _reserved_2c; /* 0x2c reserved */
    unsigned _reserved_30; /* 0x30 reserved */
    unsigned _reserved_34; /* 0x34 reserved */
    unsigned _reserved_38; /* 0x38 reserved */
    unsigned _reserved_3c; /* 0x3c reserved */

    unsigned OUTPUT1;      /* 0x40 GPIO output register 1          */
    unsigned OUTPUT2;      /* 0x44 GPIO output register 2          */

    unsigned INPUT1;       /* 0x48 GPIO input register 1           */
    unsigned INPUT2;       /* 0x4c GPIO input register 2           */

    unsigned SET1;         /* 0x50 GPIO set register 1             */
    unsigned SET2;         /* 0x54 GPIO set register 2             */

    unsigned CLEAR1;       /* 0x58 GPIO clear register 1           */
    unsigned CLEAR2;       /* 0x5c GPIO clear register 2           */

    unsigned _reserved_60; /* 0x60 reserved */    /*FIXME*/
    unsigned _reserved_64; /* 0x60 reserved */
    unsigned _reserved_68; /* 0x60 reserved */
    unsigned _reserved_6c; /* 0x60 reserved */
    unsigned _reserved_70; /* 0x60 reserved */
    unsigned _reserved_74; /* 0x60 reserved */
    unsigned _reserved_78; /* 0x60 reserved */
    unsigned _reserved_7c; /* 0x60 reserved */
    unsigned _reserved_80; /* 0x60 reserved */
    unsigned _reserved_84; /* 0x60 reserved */
    unsigned _reserved_88; /* 0x60 reserved */
    unsigned _reserved_8c; /* 0x60 reserved */
    unsigned _reserved_90; /* 0x60 reserved */
    unsigned _reserved_94; /* 0x60 reserved */
    unsigned _reserved_98; /* 0x60 reserved */
    unsigned _reserved_9c; /* 0x60 reserved */
    unsigned _reserved_a0; /* 0x60 reserved */
    unsigned _reserved_a4; /* 0x60 reserved */
    unsigned _reserved_a8; /* 0x60 reserved */
    unsigned _reserved_ac; /* 0x60 reserved */
    unsigned _reserved_b0; /* 0x60 reserved */
    unsigned _reserved_b4; /* 0x60 reserved */
    unsigned _reserved_b8; /* 0x60 reserved */
    unsigned _reserved_bc; /* 0x60 reserved */
    unsigned _reserved_c0; /* 0x60 reserved */
    unsigned _reserved_c4; /* 0x60 reserved */
    unsigned _reserved_c8; /* 0x60 reserved */
    unsigned _reserved_cc; /* 0x60 reserved */
    unsigned _reserved_d0; /* 0x60 reserved */
    unsigned _reserved_d4; /* 0x60 reserved */
    unsigned _reserved_d8; /* 0x60 reserved */
    unsigned _reserved_dc; /* 0x60 reserved */
    unsigned _reserved_e0; /* 0x60 reserved */
    unsigned _reserved_e4; /* 0x60 reserved */
    unsigned _reserved_e8; /* 0x60 reserved */
    unsigned _reserved_ec; /* 0x60 reserved */
    unsigned _reserved_f0; /* 0x60 reserved */
    unsigned _reserved_f4; /* 0x60 reserved */
    unsigned _reserved_f8; /* 0x60 reserved */
    unsigned _reserved_fc; /* 0x60 reserved */
} SOLOSW_GPIO_REGS;

extern struct semaphore    solosw_gpio_sem;
/*******************************************************************************
 *
 *                              L O C A L   M E T H O D S
 *
 *******************************************************************************/
U32 solosw_gpio_set_mode(U32 gpioNum, solosw_gpio_mode_t mode);
U32 solosw_gpio_get_mode(U32 gpioNum, U32* mode);
void solosw_gpio_assert_output(U32 gpioNum);
void solosw_gpio_deassert_output(U32 gpioNum);
void solosw_gpio_assert_output_slow(U32 gpioNum);
void solosw_gpio_deassert_output_slow(U32 gpioNum);
U32 solosw_gpio_check_input(U32 gpioNum);


