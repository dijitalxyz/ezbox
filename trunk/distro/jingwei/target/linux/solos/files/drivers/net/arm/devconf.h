#ifdef  CONFIG_ARCH_SOLOSW_GALLUS

	typedef volatile struct {
    unsigned CONFIG_REGISTER;       /* 0x00 */
    unsigned CONFIG_BOOT_CONTROL;   /* 0x04 */
    unsigned CONFIG_RESET_CONTROL;  /* 0x08 */
    unsigned CONFIG_PLL_REG_SEL;    /* 0x0C */
    unsigned CONFIG_PLL_WRITE_DATA; /* 0x10 */
    unsigned CONFIG_PLL_READ_DATA;  /* 0x14 */
    unsigned CONFIG_PLL_CONTROL;    /* 0x18 */
    unsigned F_PLL0_STATIC_CONTROL; /* 0x1C */
    unsigned F_PLL0_DIVIDER_CONTROL;/* 0x20 */
    unsigned F_PLL0_DELTA_CONTROL1; /* 0x24 */
    unsigned F_PLL0_DELTA_CONTROL2; /* 0x28 */
    unsigned F_PLL0_SPECTRUM_CONTROL;/* 0x2C */
    unsigned F_PLL1_STATIC_CONTROL; /* 0x30 */
    unsigned F_PLL1_DIVIDER_CONTROL;/* 0x34 */
    unsigned F_PLL1_DELTA_CONTROL1; /* 0x38 */
    unsigned F_PLL1_DELTA_CONTROL2; /* 0x3C */
    unsigned F_PLL1_SPECTRUM_CONTROL;/* 0x40 */
    unsigned CONFIG_COMMON;         /* 0x44 */
    unsigned CONFIG_RRM0;           /* 0x48 */
    unsigned CONFIG_RRM1;           /* 0x4C */
    unsigned CONFIG_BBPCK;          /* 0x50 */
    unsigned CONFIG_ARMCK;          /* 0x54 */
    unsigned CONFIG_USBCK;          /* 0x58 */
    unsigned CONFIG_MACCK;          /* 0x5C */
    unsigned CONFIG_HSSDCK;         /* 0x60 */
    unsigned CONFIG_DNOVACK;        /* 0x64 */
    unsigned CONFIG_REFCK;          /* 0x68 */
    unsigned CONFIG_STLPCK;         /* 0x6C */
    unsigned LOCK_STATUS;           /* 0x70 */
    unsigned CONFIG_DIAGNOSTIC;     /* 0x74 */
    unsigned CONFIG_SDRAM_CLK_DEL;  /* 0x78 */

} BF_DEVCONF_REGS;

#define BF_DEVCONF         SOLOS_LAHB_VIRT
#define REGIO(_dev, _reg) ( ((BF_##_dev##_REGS *)(BF_##_dev))->_reg )

// CONFIG_REGISTER
#define BF_DEVCONF_CONFIG_STUN_ULPI_MUX_EN  (0x40000000)
#define BF_DEVCONF_CONFIG_STUNG_DBG_ENABLE  (0x20000000)
#define BF_DEVCONF_CONFIG_OTDM_IF_ENABLE    (0x10000000)
#define BF_DEVCONF_CONFIG_OTDM_CK_ENABLE    (0x08000000)
#define BF_DEVCONF_CONFIG_ULPI_CK_ENABLE    (0x04000000)
#define BF_DEVCONF_CONFIG_EPHY_MODE         (0x02000000)
#define BF_DEVCONF_CONFIG_VPN_ENABLE        (0x01000000)
#define BF_DEVCONF_CONFIG_SERIAL_MODE       (0x00800000)
#define BF_DEVCONF_CONFIG_PARALLEL_MODE     (0x00400000)
#define BF_DEVCONF_CONFIG_EXT_USB_PHY_EN    (0x00200000)
#define BF_DEVCONF_CONFIG_SUBS_VER_MASK     (0x001E0000)
#define BF_DEVCONF_CONFIG_CHIP_ID_MASK      (0x00010000)
#define BF_DEVCONF_CONFIG_VER_NUM_MASK      (0x0000FFFF)

#endif 
