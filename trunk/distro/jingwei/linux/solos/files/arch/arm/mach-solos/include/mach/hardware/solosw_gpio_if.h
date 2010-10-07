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

/* GPIO PINS definition */
#define SOLOSW_GPIO0	0
#define SOLOSW_GPIO1	1
#define SOLOSW_GPIO2	2
#define SOLOSW_GPIO3	3
#define SOLOSW_GPIO4	4
#define SOLOSW_GPIO5	5
#define SOLOSW_GPIO6	6
#define SOLOSW_GPIO7	7
#define SOLOSW_GPIO8	8
#define SOLOSW_GPIO9	9
#define SOLOSW_GPIO10	10
#define SOLOSW_GPIO11	11
#define SOLOSW_GPIO12	12
#define SOLOSW_GPIO13	13
#define SOLOSW_GPIO14	14
#define SOLOSW_GPIO15	15
#define SOLOSW_GPIO16	16
#define SOLOSW_GPIO17	17

#ifdef CONFIG_SOLOS_GPIO_FOUR
    #define SOLOSW_PHY_RESET_GPIO 4
#else
    #define SOLOSW_PHY_RESET_GPIO 12
#endif


/* Assignment of GPIOs to Diffrent Devices */
#ifndef SOLOSW_GPIO0_MODE
	#define SOLOSW_GPIO0_MODE	OUTPUT
#endif
#ifndef SOLOSW_GPIO1_MODE
	#define SOLOSW_GPIO1_MODE	OUTPUT
#endif
#ifndef SOLOSW_GPIO2_MODE
	#define SOLOSW_GPIO2_MODE	OUTPUT
#endif
#ifndef SOLOSW_GPIO3_MODE
	#define SOLOSW_GPIO3_MODE	INVOUTPUT
#endif
#ifndef SOLOSW_GPIO4_MODE
	#define SOLOSW_GPIO4_MODE	OUTPUT
#endif
#ifndef SOLOSW_GPIO5_MODE
	#define SOLOSW_GPIO5_MODE	INVINPUT
#endif
#ifndef SOLOSW_GPIO6_MODE
	#define SOLOSW_GPIO6_MODE	INPUT
#endif
#ifndef SOLOSW_GPIO7_MODE
	#define SOLOSW_GPIO7_MODE	OUTPUT
#endif
#ifndef SOLOSW_GPIO8_MODE
	#define SOLOSW_GPIO8_MODE	FUNCTION0
#endif
#ifndef SOLOSW_GPIO9_MODE
	#define SOLOSW_GPIO9_MODE	OUTPUT
#endif
#ifndef SOLOSW_GPIO10_MODE
	#define SOLOSW_GPIO10_MODE	OUTPUT
#endif
#ifndef SOLOSW_GPIO11_MODE
	#define SOLOSW_GPIO11_MODE	OUTPUT
#endif
#ifndef SOLOSW_GPIO12_MODE
	#define SOLOSW_GPIO12_MODE	OUTPUT
#endif
#ifndef SOLOSW_GPIO13_MODE
	#define SOLOSW_GPIO13_MODE	OUTPUT
#endif
#ifndef SOLOSW_GPIO14_MODE
	#define SOLOSW_GPIO14_MODE	OUTPUT
#endif
#ifndef SOLOSW_GPIO15_MODE
	#define SOLOSW_GPIO15_MODE	OUTPUT
#endif
#ifndef SOLOSW_GPIO16_MODE
	#define SOLOSW_GPIO16_MODE	OUTPUT
#endif
#ifndef SOLOSW_GPIO17_MODE
	#define SOLOSW_GPIO17_MODE	OUTPUT
#endif	


/*******************************************************************************
*
*                              L O C A L   M E T H O D S
*
*******************************************************************************/
U32 solosw_gpio_check_reserved(U32 gpioNum);
U32 solosw_gpio_reserve_pin(U32 gpioNum);
U32 solosw_gpio_release_pin(U32 gpioNum);
void solosw_gpio_set_default(void);

