/*
 * EHCI HCD (Host Controller Driver) for Solos.
 *
 * (C) Copyright 1999 Roman Weissgaerber <weissg@vienna.at>
 * (C) Copyright 2000-2002 David Brownell <dbrownell@users.sourceforge.net>
 * (C) Copyright 2002 Hewlett-Packard Company
 *
 * Bus Glue for Conexant Solos
 *
 * Written by Christopher Hoover <ch@hpl.hp.com>
 * Based on fragments of previous driver by Rusell King et al.
 *
 * Modified for LH7A404 from ohci-sa1111.c
 *  by Durgesh Pattamatta <pattamattad@sharpsec.com>
 *
 * This file is licenced under the GPL.
 */
#define EHCI_IAA_JIFFIES        (HZ/100)        /* arbitrary; ~10 msec */
#define EHCI_IO_JIFFIES         (HZ/10)         /* io watchdog > irq_thresh */
#define EHCI_ASYNC_JIFFIES      (HZ/20)         /* async idle timeout */
#define EHCI_SHRINK_JIFFIES     (HZ/200)        /* async qh unlink delay */


#include <linux/autoconf.h>
#include <linux/reboot.h>
#include <linux/usb.h>
#include <linux/platform_device.h>
#include <mach/io.h>
#include <asm/mach-types.h>
#include <mach/hardware.h>
#include <mach/hardware/usb.h>
#include <mach/hardware/devconf.h>
#include <mach/hardware/solosw_gpio.h>
#include <mach/hardware/solosw_gpio_if.h>

#define EHCI_ULPI_VIEWPORT  0x30       /* RW to send commands to ULPI */
#define EHCI_PS_PP          0x00001000 /* RW,RO port power */
/* following define will change for hosts other than arc_usb */
#define EHCI_USBCMD         0x00       /* RO, RW, WO Command register */
#define EHCI_CMD_FLS_M      0x0000800c /* RW/RO frame list size */

struct usbblock {
	int irq;
	u32 base_addr;
};

static int ehci_solos_setup(struct usb_hcd *hcd)
{
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);
	int retval = 0;

	/* registers start at offset 0x0 */
	ehci->caps = hcd->regs;
	ehci->regs = hcd->regs +
		HC_LENGTH(ehci_readl(ehci, &ehci->caps->hc_capbase));
	dbg_hcs_params(ehci, "reset");
	dbg_hcc_params(ehci, "reset");

	/* cache this readonly data; minimize chip reads */
	ehci->hcs_params = ehci_readl(ehci, &ehci->caps->hcs_params);

	retval = ehci_halt(ehci);
	if (retval)
		return retval;

	/* data structure init */
	retval = ehci_init(hcd);
	if (retval)
		return retval;

	ehci->sbrn = 0x20;

	ehci_reset(ehci);
	ehci_port_power(ehci, 0);

	return retval;
}


static struct hc_driver ehci_solos_hc_driver = {
	.description		= hcd_name,
	.product_desc		= "Solos EHCI UHP HS",
	.hcd_priv_size		= sizeof(struct ehci_hcd),

	/* generic hardware linkage */
	.irq			= ehci_irq,
	.flags			= HCD_MEMORY | HCD_USB2,

        /* basic lifecycle operations */
	.reset			= ehci_solos_setup,
	.start			= ehci_run,
	.stop			= ehci_stop,
	.shutdown		= ehci_shutdown,

	/* managing i/o requests and associated device resources */
	.urb_enqueue		= ehci_urb_enqueue,
	.urb_dequeue		= ehci_urb_dequeue,
	.endpoint_disable	= ehci_endpoint_disable,

	/* scheduling support */
	.get_frame_number	= ehci_get_frame,

	/* root hub support */
	.hub_status_data	= ehci_hub_status_data,
	.hub_control		= ehci_hub_control,
	.bus_suspend		= ehci_bus_suspend,
	.bus_resume		= ehci_bus_resume,
	.relinquish_port	= ehci_relinquish_port,
	.port_handed_over	= ehci_port_handed_over,
};

extern int usb_disabled(void);

static int ehci_hcd_solos_drv_remove(struct device *dev);

/*-------------------------------------------------------------------------*/
static int __init ehci_hcd_solos_drv_probe(struct platform_device *pdev)
{
	struct hc_driver	*driver = &ehci_solos_hc_driver;
	struct usb_hcd		*hcd;
	struct device		*dev = &pdev->dev;
	struct usbblock		*block = dev->platform_data;
	struct ehci_regs *regs = (struct ehci_regs *)((u32)block->base_addr + 0x140);
	int irq = block->irq;
	char buf[8], *bufp = buf;
	int ret;
	u32 cmd;
	pr_debug ("In ehci_hcd_solos_drv_probe");

	if (usb_disabled())
		return -ENODEV;

	hcd = usb_create_hcd(driver, &pdev->dev, dev_name(&pdev->dev));
	if (hcd == NULL){
		dev_dbg (dev, "hcd alloc fail\n");
		return -ENOMEM;
	}

#ifdef CONFIG_ARCH_SOLOSW_GALLUS 
	{
		int value=0;
		writel(0x80000000, &regs->port_status[0]);
		mdelay(20);

		 /* Initialize Stony Point to fix power sequencing problems */
		value = readl(SOLOS_LAHB_VIRT);
		if ( value & SOLOS_DEVCONF_CONFIG_EXT_USB_PHY_EN )
		{
			writel(0x20040055, VIEWPORT_ADDR);
			writel(0x60040055, VIEWPORT_ADDR);
			writel(0x40040055, VIEWPORT_ADDR);
			writel(0x20040015, VIEWPORT_ADDR);
			writel(0x60040015, VIEWPORT_ADDR);
		}
	}

	/* halt the controller */
	writel(0, &regs->command);

#endif  /* eo CONFIG_ARCH_SOLOSW_GALLUS */ 

	/* Reset the block */
	cmd = readl(&regs->command);
	cmd |= CMD_RESET;
	writel(cmd, &regs->command);
	while(readl(&regs->command) & CMD_RESET);
	/* Put it into host mode */
	writel(FLAG_CF, &regs->configured_flag); 
	writel(0x13, block->base_addr + 0x1A8);
	writel(0x1000000, block->base_addr + 0x19c); 

#ifdef CONFIG_ARCH_SOLOS_GALLUS 
	writel(0, &regs->port_status[1]); 
#endif /* eo CONFIG_ARCH_SOLOS_GALLUS */
	
#ifdef CONFIG_ARCH_SOLOSW_GALLUS 

	if (readl(&regs->port_status[0]) & EHCI_PS_PP)
	{
		writel(0x80000000 | EHCI_PS_PP, &regs->port_status[0]);
	}
	else
	{
		writel(0x80000000, &regs->port_status[0]);
	}
	writel(EHCI_CMD_FLS_M, block->base_addr + 0x140);
#endif /* eo CONFIG_ARCH_SOLOSW_GALLUS */ 

	// hcd zeroed everything
	hcd->regs = (void *)(block->base_addr + SOLOS_USB_CAPOFFSET);
	hcd->driver = driver;
	hcd->self.bus_name = "Solos EHCI";
	hcd->product_desc = "Solos USB 2.0";
	hcd->self.controller = &pdev->dev;

	if ((ret = hcd_buffer_create (hcd)) != 0) {
		usb_put_hcd(hcd);
		dev_dbg(&pdev->dev, "hcd_buffer_create fail\n");
		return -ENOMEM;
	}

	dev_info (hcd->self.controller, "%s\n", hcd->product_desc);

	/* And mark it as an ARC device */
	//hcd_to_ehci(hcd)->is_arc_rh_tt = 1;
	/* till now HC has been in an indeterminate state ... */
	if (driver->reset && (ret = driver->reset (hcd)) < 0) {
		dev_err (hcd->self.controller, "can't reset: %d\n", ret);
		usb_put_hcd(hcd);
		return ret;
	}
	hcd->state = HC_STATE_HALT;

	sprintf (buf, "%d", irq);

	ret = request_irq(irq, usb_hcd_irq, IRQF_SHARED,
				hcd->product_desc, hcd);
	if (ret != 0) {
		dev_err (hcd->self.controller,
				"request interrupt %s failed\n", bufp);
		usb_put_hcd(hcd);
		return -EINVAL;
	}
	hcd->irq = irq;

	dev_info (hcd->self.controller, "irq %s, %s %p\n", bufp,
		(driver->flags & HCD_MEMORY) ? "pci mem" : "io base",
		hcd->regs);

	dev_set_drvdata (dev, hcd);

	if ((ret = driver->start (hcd)) < 0) {
		dev_err (hcd->self.controller, "init error %d\n", ret);
		ehci_hcd_solos_drv_remove(dev);
	}

	return ret;
}

static int __exit ehci_hcd_solos_drv_remove(struct device *dev)
{
	struct usb_hcd *hcd = dev_get_drvdata(dev); 
	if (!hcd)
		return 0;
	dev_info (hcd->self.controller, "remove, state %x\n", hcd->state);

	if (in_interrupt ())
		BUG ();

	if (HC_IS_RUNNING (hcd->state))
		hcd->state = HC_STATE_QUIESCING;

	dev_dbg (hcd->self.controller, "roothub graceful disconnect\n");
	usb_disconnect (&hcd->self.root_hub);

	hcd->driver->stop (hcd);
	hcd_buffer_destroy (hcd);
	hcd->state = HC_STATE_HALT;
	dev_set_drvdata(dev, NULL);

	free_irq (hcd->irq, hcd);

	return 0;
}

static struct platform_driver ehci_hcd_solos_driver = {
	.probe		= ehci_hcd_solos_drv_probe,
	.remove		= __exit_p(ehci_hcd_solos_drv_remove),
	.shutdown	= usb_hcd_platform_shutdown,
#ifdef CONFIG_ARCH_SOLOS_GALLUS 
	.driver.name	= "solos-ehci",
#elif CONFIG_ARCH_SOLOSW_GALLUS
	.driver.name	= "solosW-ehci",
#endif
};
