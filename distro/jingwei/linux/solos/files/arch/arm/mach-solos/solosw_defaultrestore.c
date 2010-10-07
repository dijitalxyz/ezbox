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
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/reboot.h>



/*******************************************************************************
 *
 *                            L O C A L   C O N S T A N T S
 *
 *******************************************************************************/
extern void dying_gasp_intr(void);
extern irqreturn_t  DefaultRestoreIntHandler(void);
#define U8 char
#define U32 int
#define FALSE 0
#define TRUE 1
#define RESTORE_BUTTON_DELAY_IN_SEC             2
#define RESTORE_BUTTON_CONT_PRESS_DELAY (u32TimerResolution/5)          // Time diff in ticks (Once every 200 Ms)
#define SOLOSW_restorefactory_NAME                "SOLOSW_restorefactory"


#define RESTORE_BUTTON_MIN_RESOLUTION           (u32TimerResolution/10)         // RESTORE Interrupt should be
// only 10 times in a sec. (Once every 100 ms)
U8      gU8KeepPressed = 1;
U32     gU32KeepPressedDuration = 1;
U32     gU32NumTimePressToReset = 5;
int g_RstBtnReboot = FALSE;
char ga_reset_command[128];
#define CHARM_VIC_NUM_DEF_RESTORE  ( 0)    /* unused                         */
/* wrapper for request_irq */

typedef int (*lxhandler_t)(int, void *, struct pt_regs *);
typedef int (*lx_kthread_entry)(void *);

U32 sw_restorefactory_nr_devs     = 1;
U32 sw_restorefactory_major         = 0;
U32 sw_restorefactory_minor         = 0;

/*******************************************************************************
 *
 *                              L O C A L   M E T H O D S
 *
 *******************************************************************************/
void install_restore_factory_int_handler(void)
{
    irqreturn_t rc=0;
    printk("%s: Install dying gasp interrupt\n",__FUNCTION__);
    /* Install handler */
    //atmos_sethandler(CHARM_VIC_NUM_DEF_RESTORE,DefaultRestoreIntHandler);
    rc = request_irq( CHARM_VIC_NUM_DEF_RESTORE , (lxhandler_t )DefaultRestoreIntHandler, 0,
            NULL, (void *)NULL);

    /* Changing the priority of Default Restore Interrupt */
    /* Internal Priority Select and Internal Upper Priority Enable Register : DO-418264-TC */
    /* Enable the Interrupt*/
    *(U32 *)0xFF000410 |= 0x01;
    *(U32 *)0xFF000418 |= 0x01;
    *(U32 *)0xFF00041C |= 0x01;
}

void (*restore_funct_hdlr)() = NULL;
EXPORT_SYMBOL(restore_funct_hdlr);

DECLARE_WAIT_QUEUE_HEAD(waitResetQueue);

static void RestoreFunctHandler(void)
{
    g_RstBtnReboot = TRUE;
    wake_up(&waitResetQueue);
}

irqreturn_t  DefaultRestoreIntHandler(void)
{
#if 1
    static     U8    u8ButtonPressCount     = 0;
    U32   u32CurrTime         = 0;
    static     U32   u32LastTime         = 0;
    static     U32   u32BeginTime         = 0;
    U8    u8Reset            = 0;
    U32   u32TimerResolution    = 100;

    if (u32LastTime == 0)
    {  
        /*
         ** First Buttton Press 
         */
        u8ButtonPressCount++ ;
        /*
         ** Get the current time
         */
        u32LastTime = jiffies;
        /*
         ** Log the Button Press begin time
         */
        u32BeginTime = u32LastTime;     
    }
    else 
    { 
        U32 u32TimeDiff = 0;
        u8Reset = 0;    
        /* 
         ** Get the Current Time 
         */
        u32CurrTime = jiffies;        
        u32TimeDiff = u32CurrTime - u32LastTime;
        if(u32TimeDiff <= RESTORE_BUTTON_MIN_RESOLUTION)
        {
            return IRQ_HANDLED;
        }
        /* Check the case when user has kept the Reset Button pressed */
        if (gU8KeepPressed)
        {
            if( u32TimeDiff <= RESTORE_BUTTON_CONT_PRESS_DELAY)
            {
                /*
                 ** Increment the button press count
                 */
                u8ButtonPressCount++ ;
                /* 
                 ** Update the last button press time
                 */
                u32LastTime = u32CurrTime; 
                /* Check if button has been pressed for the required time */
                if((u32CurrTime - u32BeginTime) >= (gU32KeepPressedDuration* u32TimerResolution) )
                {
                    u8Reset = 1;    
                }
            }
            else
            {
                /* Looks like user had released the button, reset the count */
                u32BeginTime = u32CurrTime;
                u8ButtonPressCount = 1;    
                u32LastTime = u32CurrTime;
            }
        }
        /*
         ** Check if the button is not pressed again in a second 
         */
        else 
        {

            if((u32TimeDiff <= (u32TimerResolution / 2))&&(gU32NumTimePressToReset > 1) )    // Enable it only once 500 ms
                return IRQ_HANDLED;
            if (u32TimeDiff <= (u32TimerResolution * RESTORE_BUTTON_DELAY_IN_SEC))
            {
                /*
                 ** Increment the button press count
                 */
                u8ButtonPressCount++ ;
                /* 
                 ** Update the last button press time
                 */
                u32LastTime = u32CurrTime; 
                if(u8ButtonPressCount >= gU32NumTimePressToReset)
                    u8Reset = 1;    
            }
            else
            {
                /* Looks like user didnt complete the count last time, reset the count */
                u32BeginTime = u32CurrTime;
                u8ButtonPressCount = 1;    
                u32LastTime = u32CurrTime;
            }

        }

    }

    if(    u8Reset == 0x01 )
    {
        /* Already pressed but not Serviced, so ignore */ 
        if(g_RstBtnReboot)
        {
            return IRQ_HANDLED;
        }
        printk("Restore Factory Defaults\n");    
        /* This will be checked in the ewmain.c file, to perform the action */
        //g_RstBtnReboot = TRUE;
        /* uncomment desired line to reset the factory with or without reboot */
        //strcpy(ga_reset_command,"system restart factory\r\n"); 
        //strcpy(ga_reset_command,"system config restore factory\r\n"); 
        if(!restore_funct_hdlr)
        {
            printk("Restore factory function not implemented!\n");
            return IRQ_HANDLED;
        }

        restore_funct_hdlr();

        return IRQ_HANDLED;
        /*We can also Disable this interrupt here and Enable the same at the ewmain.c */
        /*
         ** Set the button press count to Zero. Does not matter as we are resetting.
         */
        u8ButtonPressCount = 0;         
        /* Reset Everything */
        u32LastTime = u32CurrTime = u32BeginTime = 0;
    }
    return IRQ_HANDLED;
#endif 
}

static int resdef_open(struct inode *inode, struct file *file)
{
    static int alreadyOpen = 0;

    if (alreadyOpen) return -1;

    alreadyOpen = 1;
    restore_funct_hdlr = RestoreFunctHandler;
    wait_event(waitResetQueue, g_RstBtnReboot == TRUE);
    g_RstBtnReboot = FALSE;
    alreadyOpen = 0;

    return 0;
}
static int resdef_release(struct inode *inode, struct file *file)
{
    printk("Rebooting system\n");
    machine_restart("");

    return 0;
}

static struct file_operations resdef_fops = 
{
    .open = resdef_open,
    .release = resdef_release,
};

int __init solosw_restorefactory_init(void)
{
    dev_t dev;
    U32 result;
    int resdef_major;

    printk(KERN_WARNING "SolosW restore factory init\n");

    /* Allocate the device number */
    if (sw_restorefactory_major) 
    {
        dev = MKDEV(sw_restorefactory_major, sw_restorefactory_minor);
        result = register_chrdev_region(dev, sw_restorefactory_nr_devs, "solos_restorefactory");
    } 
    else 
    {
        result = alloc_chrdev_region(&dev, sw_restorefactory_minor, sw_restorefactory_nr_devs, "solos_restorefactory");
        sw_restorefactory_major = MAJOR(dev);
    }

    if (result < 0) 
    {
        printk(KERN_WARNING "Solosw restorefactory: can't get major %d\n", sw_restorefactory_major);
        return result;
    }


    install_restore_factory_int_handler();
    resdef_major = register_chrdev(157, "resdef", &resdef_fops);
    if (resdef_major < 0) {
        printk(KERN_WARNING "solosw_restorefactory_init: cannot create chr device: %d\n",
            resdef_major);
        result = 1; // Error cannot create char device
    }
    return result;

}

static void __exit solosw_restorefactory_exit(void)
{
    printk(KERN_WARNING "Solosw restorefactory exit\n");

    unregister_chrdev(sw_restorefactory_major, SOLOSW_restorefactory_NAME);
}


MODULE_AUTHOR("Ash Gupta");
MODULE_DESCRIPTION("SOLOSW Reset configuration");
MODULE_LICENSE("GPL");

module_init(solosw_restorefactory_init);
module_exit(solosw_restorefactory_exit);

