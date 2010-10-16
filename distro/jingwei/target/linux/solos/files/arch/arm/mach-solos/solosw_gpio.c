/*
 *  Copyright 2008 Conexant Systems Inc.
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, see <http://www.gnu.org/licenses/>
 */


/*******************************************************************************
*
*                              I N C L U D E   F I L E S
*
*******************************************************************************/
#include "mach/hardware/solosw_gpio.h"
#include <linux/semaphore.h>
#include <linux/errno.h>
/*******************************************************************************
*
*                             L O C A L   V A R I A B L E S
*
*******************************************************************************/
struct semaphore	solosw_gpio_sem;

/* To support stunG GPIO's */
#ifdef CONFIG_ARCH_SOLOSW_GALLUS
unsigned char stun_gpio_shift[] = {8, 12, 24, 16, 20, 28};
int stun_gpio_inv[SOLOSW_GPIO_STUN_GPIOS];
#endif



/*******************************************************************************
*
*                              L O C A L   M E T H O D S
*
*******************************************************************************/
/*!
\brief Set operation mode for a GPIO
\param gpioNum    32-bit unsigned    INPUT
\param Mode     solosw_gpio_mode_t        INPUT
\return 32-bit unsigned error code:
  \retval SOLOSW_GPIO_SUCCESS: Success
  \retval SOLOSW_GPIO_INVGPIO: Illegal GPIO number
  \retval SOLOSW_GPIO_INVMODE: Illegal mode
\note
*/
U32 solosw_gpio_set_mode(U32 gpioNum, solosw_gpio_mode_t mode)
{
    U32 offset;
    PRINTK(KERN_WARNING "Solosw gpio set mode  %d: GPIO  %d \n", mode,gpioNum);

    if(gpioNum >= SOLOSW_GPIO_NUMS)
        return SOLOSW_GPIO_INVGPIO;
    
    if(mode < INPUT || mode > OPENSOURCE)
        return SOLOSW_GPIO_INVMODE;

#ifdef CONFIG_ARCH_SOLOSW_GALLUS
    if(gpioNum > SOLOSW_GPIO_TRUE_GPIOS)
    {
#if 0
   if ( LOCK_SOLOSW_GPIO_FOR_WRITING )
	return SOLOSW_GPIO_INVMODE;
#endif

        U32 reg = REGIO(IOCONF, CONFIG19) & ~((U32) (3 << (stun_gpio_shift[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1]
                 + SOLOSW_GPIO_STUN_GPIO_MODE_BITS)));

        if ((mode == INPUT) || (mode == INVINPUT))
            reg |= SOLOSW_GPIO_STUN_INPUT << (stun_gpio_shift[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] + SOLOSW_GPIO_STUN_GPIO_MODE_BITS);
        else if ((mode == OUTPUT) || (mode == INVOUTPUT))
            reg |= SOLOSW_GPIO_STUN_OUTPUT << (stun_gpio_shift[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] + SOLOSW_GPIO_STUN_GPIO_MODE_BITS);
        else
        {
  UNLOCK_SOLOSW_GPIO_FOR_WRITING;  
            return SOLOSW_GPIO_INVMODE;
        }
        REGIO (IOCONF, CONFIG19) = reg;
  UNLOCK_SOLOSW_GPIO_FOR_WRITING;  

        if ((mode == INVINPUT) || (mode == INVOUTPUT))
            stun_gpio_inv[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] = TRUE;
        else
            stun_gpio_inv[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] = FALSE;
        return SOLOSW_GPIO_SUCCESS;
    }
#endif

    offset = SOLOSW_GPIO_OFFSET(CONTROL, gpioNum); 
    REGIO_GPIO(CONTROL, gpioNum) = mode << SOLOSW_GPIO_SHIFT(CONTROL, gpioNum);    

    return SOLOSW_GPIO_SUCCESS;
}

/*!
\brief Get operation mode of a GPIO
\param gpioNum    32-bit unsigned               INPUT
\param Mode       Pointer to 32-bit unsigned    OUTPUT 
\return 32-bit unsigned error code:
  \retval SOLOSW_GPIO_SUCCESS : Success
  \retval SOLOSW_GPIO_INVGPIO : Illegal GPIO number
  \retval SOLOSW_GPIO_INVARG  : Illegal "Mode" parameter (NULL pointer)
\note 
-# The caller must provide sizeof(U32) available bytes
              at address pointed to by "Mode". 
-# This function returns the mode field of the control
   register without checking whether it contains a valid mode.
*/
U32 solosw_gpio_get_mode(U32 gpioNum, U32* mode)
{
    U32 offset;
    U32 tmp;

    if(gpioNum >= SOLOSW_GPIO_NUMS)
        return SOLOSW_GPIO_INVGPIO;
    
    if(mode == NULL)
        return SOLOSW_GPIO_INVARG;

#ifdef CONFIG_ARCH_SOLOSW_GALLUS
    if(gpioNum > SOLOSW_GPIO_TRUE_GPIOS)
    {
        tmp = REGIO (IOCONF, CONFIG19) >> (stun_gpio_shift[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] + SOLOSW_GPIO_STUN_GPIO_MODE_BITS);
        if (tmp == SOLOSW_GPIO_STUN_INPUT)
        {
            if (stun_gpio_inv[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1])
                *mode = INVINPUT;
            else
                *mode = INPUT;
        }
        else if (tmp == SOLOSW_GPIO_STUN_OUTPUT)
        {
            if (stun_gpio_inv[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1])
                *mode = INVOUTPUT;
            else
                *mode = OUTPUT;
        }
        else
            return SOLOSW_GPIO_INVGPIO;

        return SOLOSW_GPIO_SUCCESS;
    }
#endif

    
    offset = SOLOSW_GPIO_OFFSET(CONTROL, gpioNum);

    *mode  =  (REGIO_GPIO(CONTROL, gpioNum)  >> SOLOSW_GPIO_SHIFT(CONTROL, gpioNum)) & SOLOSW_GPIO_CONTROL_MASK;

     PRINTK(KERN_WARNING "Solosw gpio get mode  %d: GPIO %d\n",*mode, gpioNum);	
       
     return SOLOSW_GPIO_SUCCESS;
}

/*!
\brief Assert an output pin (fast)
\param gpioNum    32-bit unsigned    INPUT
\return 
\note 
-# Inlined function.
-# Does not check that the pin has actually been programmed 
   as output.
-# Does not check validity of gpioNum.
-# Uses the set registers (0x250 and 0x254) and thus
   does not perform a read cycle before writing to the register.
-# Due to the above point there is no danger of a race condition
   and thus no critical section is needed.
*/
void solosw_gpio_assert_output(U32 gpioNum)
{
   PRINTK(KERN_WARNING "Solosw gpio assert GPIO %d\n", gpioNum);	

    if(gpioNum >= SOLOSW_GPIO_NUMS)
        return;

#ifdef CONFIG_ARCH_SOLOSW_GALLUS
    if (gpioNum > SOLOSW_GPIO_TRUE_GPIOS)
    {
        U32 reg = REGIO (IOCONF, CONFIG19);
   if ( LOCK_SOLOSW_GPIO_FOR_WRITING )
	return;


        if (stun_gpio_inv[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] == FALSE)
            reg |= (1 << (stun_gpio_shift[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] + SOLOSW_GPIO_STUN_GPIO_OUTPUT_BIT));
        else
            reg &= ~((U32) (1 << (stun_gpio_shift[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] + SOLOSW_GPIO_STUN_GPIO_OUTPUT_BIT)));
        REGIO (IOCONF, CONFIG19) = reg;
  UNLOCK_SOLOSW_GPIO_FOR_WRITING;  
    }
    else
#endif 
        REGIO_GPIO(SET, gpioNum) = 1 << SOLOSW_GPIO_SHIFT(SET, gpioNum);

}

/*!
\brief De-assert an output pin  (fast)
\param gpioNum    32-bit unsigned    INPUT
\return 
\note 
-# Inlined function.
-# Does not check that the pin has actually been programmed 
   as output.
-# Does not check validity of gpioNum.
-# Uses the clear registers (0x258 and 0x25c) and thus
   does not perform a read cycle before writing to the register.
-# Due to the above point there is no danger of a race condition
   and thus no critical section is needed.
*/
void solosw_gpio_deassert_output(U32 gpioNum)
{
   PRINTK(KERN_WARNING "Solosw gpio seassert GPIO %d\n", gpioNum);	

   if(gpioNum >= SOLOSW_GPIO_NUMS)
        return;

#ifdef CONFIG_ARCH_SOLOSW_GALLUS
    if (gpioNum > SOLOSW_GPIO_TRUE_GPIOS)
    {
        U32 reg = REGIO (IOCONF, CONFIG19);
   if ( LOCK_SOLOSW_GPIO_FOR_WRITING )
	return;

        if (stun_gpio_inv[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] == FALSE)
            reg &= ~((U32) (1 << (stun_gpio_shift[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] + SOLOSW_GPIO_STUN_GPIO_OUTPUT_BIT)));
        else
            reg |= (1 << (stun_gpio_shift[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] + SOLOSW_GPIO_STUN_GPIO_OUTPUT_BIT));
        REGIO (IOCONF, CONFIG19) = reg;
  UNLOCK_SOLOSW_GPIO_FOR_WRITING;  
    }
    else
#endif
   REGIO_GPIO(CLEAR, gpioNum) = 1 << SOLOSW_GPIO_SHIFT(CLEAR, gpioNum);
}

/*!
\brief Assert an output pin 
\param 32-bit unsigned    gpioNum    INPUT
\return 
\note 
-# The OUTPUT register is accessed atomically by disabling IRQs.
-# Inlined function.
-# Does not check that the pin has actually been programmed 
   as output.
-# Does not check validity of gpioNum.
-# Uses the output registers (0x240 and 0x244) and thus
   performs a read cycle before writing to the register (read,
   or in the value, write back the result).
*/
void solosw_gpio_assert_output_slow(U32 gpioNum)
{
   PRINTK(KERN_WARNING "Solosw gpio assert out slow GPIO %d\n", gpioNum);	

   if(gpioNum >= SOLOSW_GPIO_NUMS)
        return ;

   if ( LOCK_SOLOSW_GPIO_FOR_WRITING )
	return;
#ifdef CONFIG_ARCH_SOLOSW_GALLUS 
    if (gpioNum > SOLOSW_GPIO_TRUE_GPIOS)
    {
        U32 reg = REGIO (IOCONF, CONFIG19);

        if (stun_gpio_inv[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] == FALSE)
            reg |= (1 << (stun_gpio_shift[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] + SOLOSW_GPIO_STUN_GPIO_OUTPUT_BIT));
        else
            reg &= ~((U32) (1 << (stun_gpio_shift[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] + SOLOSW_GPIO_STUN_GPIO_OUTPUT_BIT)));
        REGIO (IOCONF, CONFIG19) = reg;
    }
    else
#endif 
    
  REGIO_GPIO(OUTPUT, gpioNum) |= 1 << SOLOSW_GPIO_SHIFT(OUTPUT, gpioNum);

  UNLOCK_SOLOSW_GPIO_FOR_WRITING;  
}

/*!
\brief De-assert an output pin 
\param gpioNum    32-bit unsigned    INPUT
\return 
\note  
-# The OUTPUT register is accessed atomically by disabling IRQs.
-# Inlined function.
-# Does not check that the pin has actually been programmed 
   as output.
-# Does not check validity of gpioNum.
-# Uses the output registers (0x240 and 0x244) and thus
   performs a read cycle before writing to the register (read,
   reset the bit, write back the result).
*/
void solosw_gpio_deassert_output_slow(U32 gpioNum)
{
   PRINTK(KERN_WARNING "Solosw gpio deassert out slow GPIO %d\n", gpioNum);	

   if(gpioNum >= SOLOSW_GPIO_NUMS)
        return;

   if ( LOCK_SOLOSW_GPIO_FOR_WRITING )
        return;
#ifdef CONFIG_ARCH_SOLOSW_GALLUS 
    if (gpioNum > SOLOSW_GPIO_TRUE_GPIOS)
    {
        U32 reg = REGIO (IOCONF, CONFIG19);

        if (stun_gpio_inv[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] == FALSE)
            reg &= ~((U32) (1 << (stun_gpio_shift[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] + SOLOSW_GPIO_STUN_GPIO_OUTPUT_BIT)));
        else
            reg |= (1 << (stun_gpio_shift[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] + SOLOSW_GPIO_STUN_GPIO_OUTPUT_BIT));
        REGIO (IOCONF, CONFIG19) = reg;
    }
    else
        REGIO_GPIO(OUTPUT, gpioNum) &= ~(1 << SOLOSW_GPIO_SHIFT(OUTPUT, gpioNum));

#endif 
  
   REGIO_GPIO(OUTPUT, gpioNum) &= ~(1 << SOLOSW_GPIO_SHIFT(OUTPUT, gpioNum));

   UNLOCK_SOLOSW_GPIO_FOR_WRITING;
}

/*!
\brief Check the state of an input pin 
\param 32-bit unsigned    gpioNum    INPUT

\note 
-# Inlined function.
-# Does not check that the pin has actually been programmed 
   as input.
-# Does not check validity of gpioNum.
*/
U32 solosw_gpio_check_input(U32 gpioNum)
{
    U32 val;

    if(gpioNum >= SOLOSW_GPIO_NUMS)
        return SOLOSW_GPIO_INVGPIO;
#ifdef CONFIG_ARCH_SOLOSW_GALLUS 

    if (gpioNum > SOLOSW_GPIO_TRUE_GPIOS)
        if (stun_gpio_inv[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] == FALSE)
            val = (REGIO (IOCONF, CONFIG19) & (1 << (stun_gpio_shift[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] + SOLOSW_GPIO_STUN_GPIO_INPUT_BIT)));
        else
        {
            val = ((REGIO (IOCONF, CONFIG19) & (1 << (stun_gpio_shift[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] + SOLOSW_GPIO_STUN_GPIO_INPUT_BIT)))
                   ^ (1 << (stun_gpio_shift[gpioNum - SOLOSW_GPIO_TRUE_GPIOS - 1] + SOLOSW_GPIO_STUN_GPIO_INPUT_BIT)));
        }
    else

#endif 
   val =  (REGIO_GPIO(INPUT, gpioNum) & (1 << SOLOSW_GPIO_SHIFT(INPUT, gpioNum))) >> SOLOSW_GPIO_SHIFT(INPUT, gpioNum);
 
   PRINTK(KERN_WARNING "Solosw gpio read: GPIO %d %d\n", val, gpioNum);

   return val;	
}

