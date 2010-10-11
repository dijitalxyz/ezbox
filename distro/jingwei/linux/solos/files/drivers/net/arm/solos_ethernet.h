#include <linux/ethtool.h>
#include <linux/netdevice.h>
#include <linux/dma-mapping.h>
#include <linux/mii.h>
#include <linux/if_vlan.h>
#include "mach/hardware/ethernet.h"
#include "mach/hardware/solosw_gpio.h"
#include "mach/hardware.h"
#include "mach/irqs.h"
#include "mach/memory.h"

#include "devconf.h"

static struct ethtool_cmd vlan_cmd;

/* Enable Buffer reuse to improve performance */
#define BUFFER_QUEUING 1

/* Legacy proc command support */
#define LEGACY_ETHER_PROC 1

/* Disable DMA support */
#define DMA 0

#ifdef BUFFER_QUEUING 
static int refill_queue_len=0;
void *refillQueue;
#endif

//Nitin : need to decide where to put it
#ifdef CONFIG_SOLOS_GPIO_FOUR
	#define PHY_RESET_GPIO 4
#else
	#define PHY_RESET_GPIO 12 
#endif


#define ETHIO(ptr)   *ptr
#define DEFAULT_NUM_TXDESCS (32)
#define DEFAULT_NUM_RXDESCS (32)
#define DEFAULT_TX_QUEUE_DEPTH (256)
#define RX_SIZE (1584)
#define SOLOS_NET_IP_ALIGN 0

/* Incase of special tagging reserve space to insert tag */
#define SOLOS_NET_STAG_RESERVE 8

enum mii_req {
	MII_NONE,
	MII_READ,
	MII_WRITE
};

char *devname="solos-eth";

#define SRAM_PHYS( _x ) (((u32)_x & ~SOLOS_SRAM_VIRT) | SOLOS_SRAM_PHYS)
#define MIN_REFILL_QUEUE_LEN 100 

static inline unsigned int READ_REG(u32 *address)
{
	return ETHIO(address);	
}
static inline void WRITE_REG(u32 *address, u32 data)
{
	ETHIO(address)=data;
}

/* Note: 8 byte alignment required. */
struct solos_eth_desc {
	u32 ctrl;       /* Control/status field. */
	u32 ptr;     /* Data pointer. */
};

int Get_PHY_settings(struct net_device *dev, struct ethtool_cmd *cmd);
int Set_PHY_settings(struct net_device *dev, struct ethtool_cmd *cmd);
u32 Get_PHY_link(struct net_device *dev);
void Get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info);

struct ethtool_ops solos_ethtool_phy_ops = {
	.get_settings = Get_PHY_settings,
	.set_settings = Set_PHY_settings,
	.get_drvinfo =  Get_drvinfo,
	.get_link =     Get_PHY_link,
};


struct solos_eth_ctxt {
	int n_txdescs;
	int n_rxdescs;
	int max_tx_queue_depth;
	int curr_tx_queue_depth;
	/* The descriptor rings */
	struct solos_eth_desc *tx_desc_ring;
	struct solos_eth_desc *rx_desc_ring;
	/* The pointers into the rings */
	struct solos_eth_desc *tx_desc_ptr;
	struct solos_eth_desc *rx_desc_ptr;
	struct solos_eth_desc *tx_desc_done;
	struct solos_eth_desc *refill_desc_ptr;
	struct solos_eth_desc *tx_refil;
	/* Pointers used for tracking the sk_buffs */
	struct sk_buff **tx_buffs;
	struct sk_buff **rx_buffs;
	void * tx_queue;
	enum mii_req mii_pending;

	/*PHY Abstraction Layer Specific */
	int mac_id;
	void * mii_bus;
	void *phy_list[32];
	int no_phys_detected;

	int opened;

	int curr_phy_id;
	int old_index;

	int old_speed[32];
	int old_duplex[32];
	int old_link[32];
};

#define MAX_PHYS_PER_MAC 32 

enum MAC_PHYSICAL_STATUS {
	MAC_NOT_PRESENT = 0,
	MAC_PRESENT = 1,
};


struct MAC
{
	unsigned int MacBaseAddr;
	unsigned int InterruptNo;
	enum MAC_PHYSICAL_STATUS MacStatus; 
	struct solos_eth_ctxt MacContext;
	struct net_device * NdevList[MAX_PHYS_PER_MAC];
	struct semaphore mutex;
};


static struct MAC MacList[]={
	{ SOLOS_UAHB_VIRT + SOLOS_MAC0_BASE, IRQ_ETH0, MAC_PRESENT },
	{ SOLOS_UAHB_VIRT + SOLOS_MAC1_BASE, IRQ_ETH1, MAC_NOT_PRESENT } 
};

struct NetDevPrivate
{
	struct MAC * MacDevPtr;
	struct phy_device *PhyDevice;
	
	struct net_device_stats stats;
	int tagged_port;
	int index;
};

#ifdef FORCE_NAME
	char *portname="ethPort-X"; 
	#define PHY_INDEX 8 
#endif


/* Duplex, half or full. */
#define SOLOS_DUPLEX_HALF     0x00
#define SOLOS_DUPLEX_FULL     0x01

enum {
	SIOCGLINKSTATE = SIOCDEVPRIVATE + 1,
	SIOCREADWRITEREG,
	SIOCSCLEARMIBCNTR,
	SIOCGIFTRANSSTART,
	SIOCMIBINFO,
	SIOCSDUPLEX,        /* 0: auto 1: full 2: half */
	#if 0
	SIOCSSPEED,         /* 0: auto 1: 100mbps 2: 10mbps */
	#endif
	SIOCSETMODE,
	SIOCCIFSTATS,
	SIOCGENABLEVLAN,
	SIOCGDISABLEVLAN,
	SIOCGQUERYNUMVLANPORTS,
	SIOCGSWITCHTYPE,
	SIOCGQUERYNUMPORTS,
	#if 0
	SIOCGMACTOPORT,
	#endif
	SIOCREATWAN,
	SIOCGLINKSTATUS,
	SIOCGLSWLINKSTATE,
	SIOCLAST
};
