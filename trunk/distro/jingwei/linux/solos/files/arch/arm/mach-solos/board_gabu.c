/*
 * linux/arch/arm/mach-solos/board_gabu.c
 *
 * Author: Guido Barzini
 * Copyright (C) 2005 Conexant inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <linux/autoconf.h>
#include <mach/hardware/solosw_gpio.h>
#include <mach/hardware/solosw_gpio_if.h>
#include <mach/hardware.h>

#include <mach/board.h>
#include <linux/delay.h> //for mdelay

/* Board-specific initialisation of the EPB and GPIOs should go here
 * to avoid littering it across the relevant subsystems such as 
 * pcmcia, mtd ... */

/* Init code for the prototyping board */


void solos_board_init()
{

/* Nitin 
   Reset the PHY GPIO */
//int i;
	
#ifdef CONFIG_ARCH_SOLOSW_GALLUS
             /* Set Ethernet Reset GPIO to output mode */
              solosw_gpio_set_mode(SOLOSW_PHY_RESET_GPIO, SOLOSW_GPIO_CONTROL_OUTPUT);
            /*Hold the chip in reset */
              solosw_gpio_deassert_output(SOLOSW_PHY_RESET_GPIO);   /* Take it low */
              mdelay(10); //Wait for 10msec
              solosw_gpio_assert_output(SOLOSW_PHY_RESET_GPIO);   /* Take it high */
              mdelay(10);//Wait for 10msec
#endif

    /* Nothing yet. */
    /* Nothing yet. */
}
