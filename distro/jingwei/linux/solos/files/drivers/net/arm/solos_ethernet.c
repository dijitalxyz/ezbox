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

/* Driver Init Function */
#include<linux/module.h>
#include<linux/skbuff.h>
#include<linux/if_arp.h>
#include<linux/phy.h>

#include "solos_ethernet.h"

struct tasklet_struct port1RxTasklet, port2RxTasklet, port1TxTasklet, port2TxTasklet;

/*****************************************************************************
 *
 * Descriptor and Tx/Rx code
 *
 */

extern int solos_Mac_Claim(const char *client, char * pMac , int *idx);
static  void portRefillHandler (struct net_device *dev);

/* This function is exported if any other module wants to use the advantage of 
buffer queuing */

void * GetPreAllocated( void )
{
	struct sk_buff * refill = NULL;
	int trueSize = 0;

	if(skb_queue_len(refillQueue)!= 0)
	{
		refill = __skb_dequeue( refillQueue);
	}

	if (refill) {
		trueSize = refill->truesize;
		memset(refill,0,offsetof(struct sk_buff, truesize));

		refill->truesize = trueSize;
		/* Nitin Added since 16 bytes are reserved by kernel at the time of allocation of skb */
		refill->data=refill->head + 16;
		refill->tail=refill->head + 16;
		refill->end=refill->head + RX_SIZE + SOLOS_NET_IP_ALIGN + SOLOS_NET_STAG_RESERVE + 16;
		atomic_set(&refill->users,1);
		atomic_set(&(skb_shinfo(refill)->dataref),1);
		skb_shinfo(refill)->nr_frags=0;
		skb_shinfo(refill)->gso_size=0;
		skb_shinfo(refill)->gso_segs=0;
		skb_shinfo(refill)->frag_list=0;

		return refill;
	}

	return NULL;
}

EXPORT_SYMBOL(GetPreAllocated);

static int solos_alloc_descriptors(struct net_device *dev,
                   struct solos_eth_ctxt *ctxt)
{
	int err = 0;
	int i;

	if (ctxt->tx_desc_ring == NULL) {
        /* Put the desciptor ring in uncached SRAM. This isn't single-cycle,
         * but given how small the descriptors are it's probably quicker to
         * fetch them from (quite fast) on-chip RAM than mess about with cache
         * management. We will need to align this to 8 bytes. */
		struct solos_eth_desc *mem =
			solos_sram_alloc(sizeof(struct solos_eth_desc) *
			                 (ctxt->n_txdescs + ctxt->n_rxdescs) + 7);

		u32 tmp = (u32) mem;
		if (mem == NULL) {
			return -12;
		}
		/* Align the pointer */
		tmp = (tmp + 7) & (~7);
		mem = (struct solos_eth_desc *)tmp;
		ctxt->tx_desc_ring = mem;
		ctxt->rx_desc_ring = mem + ctxt->n_txdescs;
	}
	/* Set up the pointers into the rings */
	ctxt->tx_desc_ptr = ctxt->tx_desc_ring;
	ctxt->rx_desc_ptr = ctxt->rx_desc_ring;
	ctxt->tx_desc_done = ctxt->tx_desc_ring;
	ctxt->tx_refil = ctxt->tx_desc_ring;
	ctxt->refill_desc_ptr=ctxt->rx_desc_ring;

	ctxt->tx_buffs = kmalloc(ctxt->n_txdescs * sizeof(struct sk_buff *),GFP_KERNEL);
	ctxt->rx_buffs = kmalloc(ctxt->n_rxdescs * sizeof(struct sk_buff *),GFP_KERNEL);

	if ((ctxt->tx_buffs) == NULL || (ctxt->rx_buffs == NULL)) {
		err = -12;
		goto vmalloc_failed;
	}
	/* OK, we have a full set of descriptors. Now set them up. */

	memset(ctxt->tx_desc_ring, 0,
	sizeof(struct solos_eth_desc) * ctxt->n_txdescs);

	memset(ctxt->tx_buffs, 0, ctxt->n_txdescs * sizeof(struct sk_buff *));

	/* Zap the Rx descriptors too */
	for (i = 0; i < ctxt->n_rxdescs; i++) {
		struct sk_buff * skb=dev_alloc_skb(RX_SIZE + SOLOS_NET_IP_ALIGN + SOLOS_NET_STAG_RESERVE);

		if (skb == NULL) {
			err = -12;
			goto no_skbs;
		}
		skb_reserve(skb,SOLOS_NET_IP_ALIGN);

		skb_reserve(skb,SOLOS_NET_STAG_RESERVE);

		ctxt->rx_desc_ring[i].ctrl = SOLOS_ETHER_RXINFO_CPU_OWN | RX_SIZE;
#if DMA
		ctxt->rx_desc_ring[i].ptr = as_dma_map_single(NULL, as_get_skb_tail(skb), RX_SIZE, SOLOS_DMA_FROM_DEVICE);
#else
		ctxt->rx_desc_ring[i].ptr = virt_to_phys(skb->data);
#endif
		ctxt->rx_buffs[i] = skb;
	}
	return 0;

no_skbs:
	for (i = 0; i < ctxt->n_rxdescs; i++) {
		if (ctxt->rx_buffs[i] != NULL)
			dev_kfree_skb(ctxt->rx_buffs[i]);
	}

vmalloc_failed:
	if (ctxt->tx_buffs != NULL) {
		kfree(ctxt->tx_buffs);
		ctxt->tx_buffs = NULL;
	}

	if (ctxt->rx_buffs != NULL) {
		kfree(ctxt->rx_buffs);
		ctxt->rx_buffs = NULL;
	}
	return err;
}

static int solos_eth_irq(int irq, void *dev_id, void *regs)
{
	struct net_device *dev = dev_id;
	int status;

	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;

	/* Store the dev structure in global pointer to be used by tasklet */

	u32 interrupt = READ_REG((u32 *)(dev->base_addr + SOLOS_ETHER_STAT));
	/* We do NOT want to clear the MDIO completion bit as that is being polled from process context */
	interrupt &= ~SOLOS_ETHER_STAT_MDIO;

	WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_STAT), interrupt);

	/* Do the receives first as the most urgent thing */
	if (interrupt & SOLOS_ETHER_STAT_RXINT)
	{
#if 0
		status = READ_REG(dev->base_addr + SOLOS_ETHER_ENABLE);
		status =status & (~SOLOS_ETHER_STAT_RXINT);
		WRITE_REG(dev->base_addr + SOLOS_ETHER_ENABLE, status);
#endif
		/*Schedule the tasklet */
		if( irq == IRQ_ETH0 )
		{
			tasklet_schedule(&port1RxTasklet);
		}
		else
		{
			tasklet_schedule(&port2RxTasklet);
		}
	}

	if (interrupt & SOLOS_ETHER_STAT_TXINT)
	{

		status = READ_REG((u32 *)(dev->base_addr + SOLOS_ETHER_ENABLE));
		status =status & (~SOLOS_ETHER_STAT_TXINT);
		WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_ENABLE), status);

		if( irq == IRQ_ETH0 )
		{
			tasklet_schedule(&port1TxTasklet);
		}
		else
		{
			tasklet_schedule(&port2TxTasklet);
		}
	}

#if 0
	if (interrupt & (SOLOS_ETHER_STAT_TXCH | SOLOS_ETHER_STAT_MISSERR |
	    SOLOS_ETHER_STAT_RXCRC | SOLOS_ETHER_STAT_RXFRAME |
	    SOLOS_ETHER_STAT_RXOFLOW)) {
		if ( status & SOLOS_ETHER_STAT_MISSERR) ctxt->stats.rx_missed_errors++;
		if ( status & SOLOS_ETHER_STAT_RXCRC) ctxt->stats.rx_crc_errors++;
		if ( status & SOLOS_ETHER_STAT_RXFRAME) ctxt->stats.rx_frame_errors++;
		if ( status & SOLOS_ETHER_STAT_RXOFLOW) ctxt->stats.rx_over_errors++;
		if ( status & SOLOS_ETHER_STAT_TXCH ) ctxt->stats.tx_errors++;
		else ctxt->stats.rx_errors++;
	}
#endif
	return 1;
}


static void solos_kick_tx_real(struct NetDevPrivate *priv,struct solos_eth_ctxt *ctxt, u32 addr)
{

	/* Is there any space in the descriptor ring? */

/* Steps to Do :-
    1.Check if any descriptor currently owned by cpy is there
    2.Extract buffer from Queue
    3.Attach the buffer with the descriptor
    4.Loop Back until descriptor is free and packet is also present
    5.Now Kick the mac and exit.
*/
	while(skb_queue_len(ctxt->tx_queue))
	{
		if(!(ctxt->tx_desc_ptr->ctrl & SOLOS_ETHER_TXINFO_OWN) && (ctxt->tx_desc_ptr->ptr == 0))
		{

			/* Extract the buffer */
			struct sk_buff *buf = __skb_dequeue(ctxt->tx_queue);

			if(buf)
			{
				int idx = ctxt->tx_desc_ptr - ctxt->tx_desc_ring;

				priv->stats.tx_packets++;
				priv->stats.tx_bytes += buf->len;

				/* Remember where we put the skb */
				ctxt->tx_buffs[idx++] = buf;

				/* Sync the data by cleaning the cache */
				/* FIXME: porting issue */
				//consistent_sync(buf->data, buf->len, DMA_TO_DEVICE);
				//dma_cache_sync(struct device *dev, void *vaddr, size_t size, enum dma_data_direction direction);

#ifdef __arm__
				{
					volatile U32 info asm("r1") = buf->len | SOLOS_ETHER_TXINFO_CPU_OWN |
							                SOLOS_ETHER_TXINFO_CPU_FIRST |
							                SOLOS_ETHER_TXINFO_CPU_LAST;

					volatile U32 ptr asm("r0") = virt_to_phys(buf->data);

					asm volatile (
						"    stmia    %2, { %0, %1 }\n"
						:
						: "r" (info), "r" (ptr), "r" (ctxt->tx_desc_ptr)
						: "memory"
					 );
				}
#else
				ctxt->tx_desc_ptr->ptr= virt_to_phys(buf->data);
				ctxt->tx_desc_ptr->ctrl =
					buf->len | SOLOS_ETHER_TXINFO_CPU_OWN |
					SOLOS_ETHER_TXINFO_CPU_FIRST |
					SOLOS_ETHER_TXINFO_CPU_LAST;
#endif
				/* Handle wrap */
				ctxt->tx_desc_ptr++;
				if (idx == ctxt->n_txdescs)
					ctxt->tx_desc_ptr = ctxt->tx_desc_ring;
			}
		}
		else
		{
			break;
		}
	}

	/* Prod the MAC after whole queue is filled or no more descriptor left*/
	WRITE_REG((u32 *)(addr + SOLOS_ETHER_STAT),
                  SOLOS_ETHER_STAT_TXPOLL);
}


static int solos_eth_tx(struct sk_buff *skb, struct net_device *dev)
{
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;
	struct phy_device *phydev= ctxt->phy_list[0];
	
	if( phydev->bus->vlan_status == 1 )
	{
		skb = phydev->drv->handle_portsplit_tx(skb,priv->PhyDevice->addr);	
	}

	if(ctxt->curr_tx_queue_depth < ctxt->max_tx_queue_depth )
	{
		__skb_queue_tail(ctxt->tx_queue, skb);
		ctxt->curr_tx_queue_depth ++;
		/* If it is the only packet transmit it now */
		if(ctxt->curr_tx_queue_depth == 1 )
		{
			solos_kick_tx_real(priv,ctxt, dev->base_addr);
		}
	}
	else
	{
		#define ETH_NETDEV_TX_BUSY 1
		if(skb->len > 1000)
			return( ETH_NETDEV_TX_BUSY );
	
#ifdef BUFFER_QUEUING
		/* If buffer size is equal to max buffer then queue it otherwise free it*/
		if(((skb->end - skb->head) >= RX_SIZE + SOLOS_NET_IP_ALIGN + SOLOS_NET_STAG_RESERVE ) &&
		   (skb_queue_len(refillQueue) <refill_queue_len)) {
			if(!skb_cloned(skb))
			{
				__skb_queue_tail(refillQueue, skb );
			}
			else
			{
				dev_kfree_skb(skb);
			}
		}
		else
#endif
		{
			dev_kfree_skb(skb);
		}
	}
	return 0;
}


static void solos_do_tx_tasklet_handler(struct net_device *dev)
{
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;

	u32 status;	

	status = READ_REG((u32 *)(dev->base_addr + SOLOS_ETHER_ENABLE));
	status =status & (~SOLOS_ETHER_STAT_TXINT);
	WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_ENABLE), status);

	/* Check if buffer owned by CPU and buffer ptr is not null free the buffer */
	while( (!(ctxt->tx_refil->ctrl & SOLOS_ETHER_TXINFO_OWN)) && (ctxt->tx_refil->ptr != NULL))
	{
		int idx = ctxt->tx_refil - ctxt->tx_desc_ring;
		struct sk_buff *buf=ctxt->tx_buffs[idx++];
#ifdef BUFFER_QUEUING
		if (((buf->end - buf->head) >= RX_SIZE + SOLOS_NET_IP_ALIGN + SOLOS_NET_STAG_RESERVE ) && (skb_queue_len(refillQueue) <refill_queue_len) && (!skb_cloned(buf))) {
			__skb_queue_tail(refillQueue, buf );
		}
		else
#endif
		{
			dev_kfree_skb(buf);
		}
		ctxt->tx_refil->ptr=NULL;
		ctxt->curr_tx_queue_depth --;

		/* Handle wrap */
		ctxt->tx_refil++;
		if (idx == ctxt->n_txdescs)
			ctxt->tx_refil = ctxt->tx_desc_ring;
	}

	/* Attach packets to descriptors */
	solos_kick_tx_real(priv,ctxt,dev->base_addr);

//	portRefillHandler(dev);
	/* Enable the Ethernet interrupts */
#if 1
	status = READ_REG((u32 *)(dev->base_addr + SOLOS_ETHER_ENABLE));
	status =status | SOLOS_ETHER_STAT_TXINT;
	WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_ENABLE), status);
#endif

}

void DumpPacket(struct sk_buff *skb , char * Header ,char * Footer )
{
	int i=0;
	printk("\n########################%s#######################\n\n",Header);
	for(i=0;i<skb->len;i++)
	{
		printk("%x- ",*(skb->data + i));
		if((i%10) == 0 && i!=0)
			printk("\n");
	}	
	printk("\n########################%s#######################\n\n",Footer);
}

static void solos_do_rx(struct net_device *dev)
{

	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;
	struct phy_device *phydev=ctxt->phy_list[0];	
	struct solos_eth_desc *desc = ctxt->rx_desc_ptr;
	int idx = (desc - ctxt->rx_desc_ring);
	unsigned int status;
	int trueSize=0;
	int port_no;

#ifdef BUFFER_QUEUING
	int count=0;
#endif

	/* Disable RX INTERRUPT */
	status = READ_REG((u32 *)(dev->base_addr + SOLOS_ETHER_ENABLE));
	status =status & (~SOLOS_ETHER_STAT_RXINT);
	WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_ENABLE), status);

	while ((!(desc->ctrl & SOLOS_ETHER_RXINFO_OWN)) && ( desc->ptr !=NULL))
        {
		struct sk_buff *rx_skb = ctxt->rx_buffs[idx];
		int len = desc->ctrl & SOLOS_ETHER_RXINFO_RX_LEN;
		if (len >= 64)
		{
			len -= 4;
		}
		else
			printk("\n ERROR :: Packet too short");

		//##################NITIN BEGIN##############################
		if ( phydev->bus->vlan_status == 1 )
		{
			struct net_device *dest_intf;

			skb_put(rx_skb,len);
			port_no = phydev->drv->handle_portsplit_rx( rx_skb );
			dest_intf = priv->MacDevPtr->NdevList[port_no];

			if((port_no != -1 ) && (dest_intf !=NULL))
			{
				struct NetDevPrivate *temp_priv;

				rx_skb->dev = dest_intf;
				if ( rx_skb->dev->type != ARPHRD_ETHER )
				{
					rx_skb->protocol = htons(ETH_P_IP);
					rx_skb->mac.raw = rx_skb->data;
				}
				else
				{
					rx_skb->protocol = eth_type_trans( rx_skb, rx_skb->dev );
				}
				rx_skb->ip_summed = CHECKSUM_NONE;
				netif_rx(rx_skb);

				temp_priv=(struct NetDevPrivate *) dest_intf->priv;
				temp_priv->stats.rx_packets++;
				temp_priv->stats.rx_bytes += len;
			}
		}
		else
		{
			rx_skb->len = len;
			rx_skb->tail = rx_skb->data + rx_skb->len;
			rx_skb->dev = dev;

			if ( rx_skb->dev->type != ARPHRD_ETHER )
			{
				rx_skb->protocol = htons(ETH_P_IP);
				rx_skb->mac.raw = rx_skb->data;
			}
			else
			{
				rx_skb->protocol = eth_type_trans( rx_skb, rx_skb->dev );
			}

			rx_skb->ip_summed = CHECKSUM_NONE;
			netif_rx(rx_skb);

			priv->stats.rx_packets++;
			priv->stats.rx_bytes += len;
		}
		//##################NITIN END##############################	

		/* Update Descriptor Pointer */
		desc->ptr=NULL;

		idx++;
		if (idx == ctxt->n_rxdescs)
		{
			idx = 0;
			desc = ctxt->rx_desc_ring;
		}
		else
		{
			desc++;
		}
	}/* END WHILE */
	ctxt->rx_desc_ptr = desc;	

	portRefillHandler(dev);

	/* ENABLE RX INTERRUPT */
	status = READ_REG((u32 *)(dev->base_addr + SOLOS_ETHER_ENABLE));
	status =status | SOLOS_ETHER_STAT_RXINT;
	WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_ENABLE), status);
}


static void portRefillHandler(struct net_device *dev)
{
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;

	struct solos_eth_desc *desc = ctxt->refill_desc_ptr;

	int idx = (desc - ctxt->rx_desc_ring);
	unsigned int status;
	int trueSize=0;

	while (desc->ptr == NULL)
	{
		struct sk_buff * refill = NULL;
#ifdef BUFFER_QUEUING
		if(skb_queue_len(refillQueue)!= 0)
		{
			refill = __skb_dequeue( refillQueue);
		}

		if (refill) {
			trueSize = refill->truesize;
			memset(refill,0,offsetof(struct sk_buff, truesize));

			refill->truesize = trueSize;
			/* Nitin Added since 16 bytes are reserved by kernel at the time of allocation of skb */
			refill->data=refill->head + 16;
			refill->tail=refill->head + 16;
			refill->end=refill->head + RX_SIZE + SOLOS_NET_IP_ALIGN + SOLOS_NET_STAG_RESERVE + 16;
			atomic_set(&refill->users,1);
			atomic_set(&(skb_shinfo(refill)->dataref),1);
			skb_shinfo(refill)->nr_frags=0;
			skb_shinfo(refill)->gso_size=0;
			skb_shinfo(refill)->gso_segs=0;
			skb_shinfo(refill)->frag_list=0;
		}
/*
		else if ( refill_queue_len< ctxt->n_rxdescs) {
			refill_queue_len=ctxt->max_tx_queue_depth;
		}
*/
#endif
		if(!refill)
			refill=dev_alloc_skb(RX_SIZE + SOLOS_NET_IP_ALIGN + SOLOS_NET_STAG_RESERVE);

		if (refill != NULL) 
		{
			int len;
			skb_reserve(refill, SOLOS_NET_IP_ALIGN + SOLOS_NET_STAG_RESERVE);
			ctxt->rx_buffs[idx] = refill;
#ifdef __arm__
			{
				volatile U32 info asm("r1") = SOLOS_ETHER_RXINFO_CPU_OWN | RX_SIZE;
				volatile U32 new_ptr asm("r0") = virt_to_phys(refill->data);

				asm volatile (
					"    stmia    %2, { %0, %1 }\n"
					:
					: "r" (info), "r" (new_ptr), "r" (desc)
					: "memory"
				);
			}
#else
			desc->ptr= virt_to_phys(refill->data);
			desc->ctrl = SOLOS_ETHER_RXINFO_CPU_OWN | RX_SIZE;
#endif

			idx++;
			if (idx == ctxt->n_rxdescs)
			{
				idx = 0;
				desc = ctxt->rx_desc_ring;
			} else {
				desc++;
			}
		} else {
			break;
		}
	}/* END WHILE */

	ctxt->refill_desc_ptr = desc;
}

static void ProgramMAC(struct net_device *dev,struct solos_eth_ctxt* ctxt)
{
	u32 tmp;
	/* Program up the hardware. */
	WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_TXRINGPTR),
	           SRAM_PHYS(ctxt->tx_desc_ring));
	WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_RXRINGPTR),
	           SRAM_PHYS(ctxt->rx_desc_ring));

	tmp= dev->dev_addr[0] | (dev->dev_addr[1] << 8) | (dev->dev_addr[2] << 16) | (dev->dev_addr[3] << 24);
	WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_ADDRL), tmp);

	tmp=dev->dev_addr[4] | (dev->dev_addr[5] << 8);
	WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_ADDRH), tmp);

	WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_ENABLE),
	          SOLOS_ETHER_STAT_TXINT | SOLOS_ETHER_STAT_RXINT |
	          SOLOS_ETHER_STAT_ERR );

	/* Turn on misc extra features */
	tmp = READ_REG((u32 *)(dev->base_addr + SOLOS_ETHER_XTRACTRL));
	tmp |= SOLOS_ETHER_XTRACTRL_BVCIFIX |
	       SOLOS_ETHER_XTRACTRL_TXAUTOPAD |
	       SOLOS_ETHER_XTRACTRL_RXBURST8 |
	       SOLOS_ETHER_XTRACTRL_TXBURST8 |
	       SOLOS_ETHER_XTRACTRL_XTRAENB | SOLOS_ETHER_XTRACTRL_LOCKFIX;
	WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_XTRACTRL), tmp);

	/* Kick thinsg off in promiscous mode */
	tmp = SOLOS_ETHER_CONTROL_PROM | SOLOS_ETHER_CONTROL_ENABLE | SOLOS_ETHER_CONTROL_TXRUN |
	      SOLOS_ETHER_CONTROL_RXRUN | SOLOS_ETHER_CONTROL_ENBFULL |
	      (ctxt->n_txdescs << SOLOS_ETHER_CONTROL_TXBDTLEN_SHIFT)
	      | (ctxt->n_rxdescs << SOLOS_ETHER_CONTROL_RXBDTLEN_SHIFT);

	WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_CONTROL), tmp);
}

void init_skb_qhead(void **qhead, int size, int num_buffs)
{
	int i = 0;
	struct sk_buff  *pBuf ;
	*qhead = (struct sk_buff_head *)kmalloc(sizeof(struct sk_buff_head), GFP_KERNEL);
	skb_queue_head_init((struct sk_buff_head *)*qhead);

	/* If we invoke this function by passing num_buffs as '0'
	   then only the qhead will be initialised!
	 */
	if ( num_buffs )
	{
		for (i=0; i < num_buffs ; i++ ) {
			if ( (pBuf = (struct sk_buff *)dev_alloc_skb(size)) )
				__skb_queue_tail((struct sk_buff_head *)*qhead, pBuf);
		}
	}
}

static int bf_eth_open( struct net_device *dev )
{
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;

	u32 tmp;
	int status = 0;
	int phy_addr=0;

	if ( priv->PhyDevice->bus->vlan_status == 1 )
	{
		priv->PhyDevice->state = PHY_CHANGELINK;
		phy_start(priv->PhyDevice);
	}	

	if(ctxt->opened)
		return 0;
	else
		ctxt->opened = 1;	

	status = solos_alloc_descriptors(dev, ctxt);
	init_skb_qhead(&ctxt->tx_queue, 0, 0);

#ifdef BUFFER_QUEUING
	 /* This function allocates memory for the refillQueue &
	  * do the initialisations by allocating the skb's.
	  */
	init_skb_qhead(&refillQueue,(RX_SIZE + SOLOS_NET_IP_ALIGN + SOLOS_NET_STAG_RESERVE), MIN_REFILL_QUEUE_LEN);
	refill_queue_len = MIN_REFILL_QUEUE_LEN;
#endif

	if (status == 0)
	{
		status = request_irq(dev->irq, solos_eth_irq, 0, devname, dev);
	}
	if (status != 0)
		return status;
	if ( dev->irq  == IRQ_ETH0 )
	{
		tasklet_init(&port1RxTasklet,solos_do_rx,(long)dev);
		tasklet_init(&port1TxTasklet,solos_do_tx_tasklet_handler,(long)dev);
	}
	else
	{
		tasklet_init(&port2RxTasklet,solos_do_rx,(long)dev);
		tasklet_init(&port2TxTasklet,solos_do_tx_tasklet_handler,(long)dev);
	}

	ProgramMAC(dev,ctxt);

	if ( priv->PhyDevice->bus->vlan_status != 1 )
	{
		for (phy_addr = 0; phy_addr < ctxt->no_phys_detected; phy_addr++)
		{
			if (ctxt->phy_list[phy_addr])
			{
				struct phy_device *phydev = ctxt->phy_list[phy_addr];
				phydev->state = PHY_CHANGELINK;
				phy_start(phydev);
			}
		}
	}
	return 0;
}

static void *bf_get_stats(struct net_device *dev)
{
	int i;
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	return &priv->stats;
}

static void bf_eth_set_mcast( struct net_device *dev )
{

}

struct tif_data
{
        int status;
        int port_index;
};

struct ioctl_data
{
        #define READREG 1
        #define WRITEREG 2 
	int retval;
	int phyad;
	int reg;
	int setval;
	int op;
};

int bf_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
    	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;


	int *data=(int*)ifr->ifr_data;

	switch(cmd)
	{
		case SIOCGLINKSTATE:
		{
			struct tif_data * pifData = (struct tif_data *)ifr->ifr_data;
			if( pifData->port_index == -1 )
			{
				int loop;
				struct phy_device *phydev;
				for(loop=0;loop <ctxt->no_phys_detected;loop++)
				{
					phydev=ctxt->phy_list[loop];	
					if( phydev && phydev->link )
						break;	
				} 
				if(loop >=ctxt->no_phys_detected)
					pifData->status = 0;
				else	
					pifData->status = 1;

			}
			else
			{
				struct phy_device *phydev=ctxt->phy_list[pifData->port_index -2 ];
				if(phydev)
				{
					pifData->status = phydev->link;
				}
				else
					pifData->status = -1; // Unlikely

						
			}
		}
		break;

		case SIOCGQUERYNUMVLANPORTS :
			*data = ctxt->no_phys_detected;			
		break;

		case SIOCGQUERYNUMPORTS:
				*data = ctxt->no_phys_detected;			
		break;

		/* Write phy register expects phy , reg and value */
		case SIOCREADWRITEREG:
		{
				struct ioctl_data * ioctlData = (struct ioctl_data *)ifr->ifr_data;
				struct phy_device *phydev=ctxt->phy_list[0];
				if( ioctlData->op == READREG)
				{
					ioctlData->retval = phy_read( phydev, ioctlData->reg, ioctlData->phyad );
				}
				else if( ioctlData->op == WRITEREG )
				{
					phy_write( phydev, ioctlData->reg, ioctlData->setval, ioctlData->phyad );
				}

		}
		break;

	}


	return 0;
}

static void solos_stop_hw(struct net_device *dev)
{
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;

	int i;
	int not_stuck = 1000000;

	/* Stop the hardware */
	WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_CONTROL), 0);
	/* Turn off all interrupts */
	WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_ENABLE), 0);

	/* Zap the descriptor ring pointers */
	WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_TXRINGPTR), 0);
	WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_RXRINGPTR), 0);

	/* Do this while we wait for the DMA to wind down... */
	//Nitin __skb_queue_purge(&ctxt->tx_queue);

	/* Now spin until the DMA really does stop */
	while (not_stuck &&
	       (READ_REG((u32 *)(dev->base_addr + SOLOS_ETHER_TXPTRREAD)) |
	        READ_REG((u32 *)(dev->base_addr + SOLOS_ETHER_RXPTRREAD))))
		not_stuck--;

	if (!not_stuck)
		printk("Seem not to have quiesced DMA. Just giving up and hoping.\n");

	/* And clear out the descriptor rings */
	for (i = 0; i < ctxt->n_txdescs; i++) {
		ctxt->tx_desc_ring[i].ctrl = 0;
		ctxt->tx_desc_ring[i].ptr = 0;

		if (ctxt->tx_buffs[i] != NULL)
			dev_kfree_skb(ctxt->tx_buffs[i]);
	}
	/* And clear out the descriptor rings */
	for (i = 0; i < ctxt->n_rxdescs; i++) {
		ctxt->rx_desc_ring[i].ctrl = 0;
		ctxt->rx_desc_ring[i].ptr = 0;

		if (ctxt->rx_buffs[i] != NULL)
			dev_kfree_skb(ctxt->rx_buffs[i]);
	};
}

static int bf_eth_stop( struct net_device *dev )
{
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
#if 0
	if ( priv->PhyDevice->bus->vlan_status == 1 )
	{
		phy_stop(priv->PhyDevice);
	}
#endif
	solos_Mac_Release(dev->name, priv->index);
	return 0;
}

#ifdef  LEGACY_ETHER_PROC 

int get_phy_attr(char *attrname,struct phy_device *phydev)
{
	if( !strcmp(attrname,"LinkSpeed"))
		return phydev->speed;
	else if( !strcmp(attrname,"PortLinkStatus"))
		return phydev->link;
	else if( !strcmp(attrname,"FullDuplex"))
		return phydev->duplex;
	else if( !strcmp ( attrname,"ChipName" ))
		printk("\n ChipName     :: %s",phydev->drv->name);
	else if (! strcmp ( attrname,"SwtchLearnEnbl" ))
		return phydev->bus->learning_status;
	else if(! strcmp ( attrname,"FlowControl" ))
		return phydev->bus->flow_status;
	else if(! strcmp ( attrname,"VLANEnabled" ))
		return phydev->bus->vlan_status;

	return 0;
}

int set_phy_attr(char *attrname,struct phy_device *phydev,int val,int second_val)
{
	struct ethtool_cmd cmd;
	memset(&cmd,0,sizeof(cmd));

	if (! strcmp ( attrname,"SwtchLearnEnbl" ))
	{
		cmd.swtchInfo.configure_switch=1;

		if ( val >=1)
			cmd.swtchInfo.swtchLrnNeeded=1;
		if ( val == 0 )
			cmd.swtchInfo.swtchLrnNeeded=-1;

		configure_switch(phydev,&cmd);
		return 0;
	}
	else if(! strcmp ( attrname,"FlowControl" ))
	{
		cmd.swtchInfo.configure_switch=1;

		if ( val >=1)
			cmd.swtchInfo.flowctrlNeeded=1;
		if ( val == 0 )
			cmd.swtchInfo.flowctrlNeeded=-1;
		configure_switch(phydev,&cmd);
		return 0;
	}
	else if(!strcmp(attrname,"VLANEnabled"))
	{
		vlan_cmd.swtchInfo.configure_switch=1;

		if ( val == 0 )
		{
			/* Disable the VLAN from Switch */
			vlan_cmd.swtchInfo.vlanNeeded=-1;
			vlan_cmd.swtchInfo.NVlans=0;
			configure_switch(phydev,&vlan_cmd);
			memset(&vlan_cmd,0,sizeof(vlan_cmd));
			return 0;
		}
		if ( val >=1)
			vlan_cmd.swtchInfo.vlanNeeded=1;
	}
	else if(!strcmp(attrname,"NumVlans"))
		vlan_cmd.swtchInfo.NVlans=val;
	else if( !strcmp(attrname,"VlanMask"))
	{
		vlan_cmd.swtchInfo.VMask[val]=second_val;
	}
	/* since vlan is configured with multiple commands needed Configure the VLAN only if all the information needed is available */
	if(( vlan_cmd.swtchInfo.vlanNeeded >=1 ) && ( vlan_cmd.swtchInfo.NVlans > 0 ))
	{
		int i;
		for(i=0;i<vlan_cmd.swtchInfo.NVlans;i++)
		{
			if(vlan_cmd.swtchInfo.VMask[i] == 0 )
				return 0;
		}

		configure_switch(phydev,&vlan_cmd);
		vlan_cmd.swtchInfo.vlanNeeded=-1;
	}
}


void ETH_PROC_GET_HOOK( int mac_no, char * attrname , int value ,int second_val,int GETORSET)
{
	int i;
	struct net_device *dev;
	struct phy_device *phydev;
	if(GETORSET == 1)
	{
		/* Get Operation */
		if( ! strcmp( attrname,"NumPhyPorts"))
		{
			printk("\n NumPhyPorts 		:: %d",MacList[ mac_no ].MacContext.no_phys_detected);
			return ;
		}
		else if ( (! strcmp ( attrname,"ChipName" ))  )
		{
			get_phy_attr(attrname,MacList[ mac_no ].MacContext.phy_list[0]);
			return;
		}
		else if ( (! strcmp ( attrname,"SwtchLearnEnbl" )) || (! strcmp ( attrname,"FlowControl" )) || (! strcmp ( attrname,"VLANEnabled" )))
		{
			if ( get_phy_attr(attrname,MacList[ mac_no ].MacContext.phy_list[0]) == 1)		
				printk("\n %s :: %s		",attrname,"enabled" );
			else
				printk("\n %s :: %s		",attrname,"disabled" );
	
			return;
		}

			for(i=0 ; i< value; i++)
			{
				phydev = MacList[ mac_no ].MacContext.phy_list[i];
	
				if ( phydev )
				{
					printk("\n %s[%d] :: %d		",attrname,i,get_phy_attr(attrname,phydev) );
				}
			}	
	}
	else if(GETORSET == 2)
	{
		/* Set Operation */
		if(MacList[ mac_no ].MacContext.no_phys_detected < value)
		{
			printk("\n Invalid Port number !! MAX VALUE :: %d ",MacList[ mac_no ].MacContext.no_phys_detected);
			return 0;
		}		
		set_phy_attr(attrname,MacList[ mac_no ].MacContext.phy_list[0],value,second_val);
	}

}
EXPORT_SYMBOL(ETH_PROC_GET_HOOK);
#endif

int Get_PHY_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;
	int err;

	if ( priv->PhyDevice->bus->vlan_status == 1 )
	{
		err=phy_ethtool_gset(priv->PhyDevice,cmd);
		/* Tell the application how many PHY are attached */
		cmd->swtchInfo.num_phy_attached=ctxt->no_phys_detected;
		return err;
	}
	else
	{
		struct phy_device *phydev = ctxt->phy_list[ctxt->curr_phy_id];

		if (phydev)
		{
			int err;
			err=phy_ethtool_gset(phydev,cmd);
			/* Tell the application how many PHY are attached */
			cmd->swtchInfo.num_phy_attached=ctxt->no_phys_detected;
			cmd->phy_address = ctxt->curr_phy_id;
			return err;
		}
	}
	return -22;
}

int Set_PHY_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;
	struct phy_device *phydev;
  
	int index =(int)cmd->phy_address;
	
	if ( priv->PhyDevice->bus->vlan_status == 1 )
	{
		phydev=priv->PhyDevice;
	}
	else
	{
		phydev=ctxt->phy_list[ctxt->curr_phy_id];
		ctxt->old_index=ctxt->curr_phy_id;

		if(index < ctxt->no_phys_detected )
			ctxt->curr_phy_id=index;
		else
			return -22;

		if(ctxt->old_index != ctxt->curr_phy_id)
		{
			/* Nitin
			As of now facing some problem
			if we do not return from here while changing the phy address
			will resolve it some time later */
			ctxt->old_index=ctxt->curr_phy_id;
			return 0;
		}
	}

	ctxt->old_index=ctxt->curr_phy_id;

	if (phydev)
	{
		int k;
		if(cmd->swtchInfo.configure_switch)
		{
			int ret;	

			local_irq_disable();
			if((cmd->swtchInfo.vlanNeeded == 1) && (phydev->bus->vlan_status !=1))
			{
				rtnl_unlock();
				CreateSplitInterfaces( dev );
				rtnl_lock();
			}
			if ((cmd->swtchInfo.vlanNeeded == -1) && (phydev->bus->vlan_status ==1))
			{
				MergeInterfaces( dev );
			}
			ret =  configure_switch(phydev,cmd);
			local_irq_enable();
			return ret;
		}
		return phy_ethtool_sset(phydev,cmd);
	}
}

int Get_PHY_link(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;
	struct phy_device *phydev;

	if ( priv->PhyDevice->bus->vlan_status == 1 )
		phydev = priv->PhyDevice;
	else
		phydev=ctxt->phy_list[ctxt->curr_phy_id];

	phydev->drv->read_status(phydev);

	return phydev->link;
}


void Get_drvinfo(struct net_device *dev, void *info)
{

}


static int bf_ether_init_net(struct net_device *dev)
{
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;

	int tmp = dev->hard_header_len;
        int ret = 0;
        int idx = 0;

	ether_setup(dev);
	dev->hard_header_len += tmp;
	dev->get_stats = bf_get_stats;
	dev->open = bf_eth_open;
	dev->stop = bf_eth_stop;
	dev->hard_start_xmit = solos_eth_tx;
	dev->set_multicast_list = bf_eth_set_mcast;
	dev->do_ioctl = bf_ioctl;

	SET_ETHTOOL_OPS(dev, &solos_ethtool_phy_ops);
//	set_ndev_mac(0/*dev->irq   instance no*/
//	             ,dev);

	ret = solos_Mac_Claim(dev->name,dev->dev_addr,&(priv->index));
	return ret;
}

/* If previously posted request was reading, and it is completed,
 * store contents of MII data register to result and return 1. If previous
 * operation do not match to requested, post new MII read request and return 0
 */
static inline int solos_mii_read_posted(struct net_device *dev, int phy,
                    int reg, short *result)
{
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;

	unsigned int status;

	status = READ_REG((u32 *)(dev->base_addr + SOLOS_ETHER_STAT));

	if (ctxt->mii_pending && !(status & SOLOS_ETHER_STAT_MDIO)) {   /* Is the last MII op complete? */
		/* No. caller will just have to try again later... */
		return 0;
	}

	/* Clear the ready bit... */
	WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_STAT), SOLOS_ETHER_STAT_MDIO);

	if (ctxt->mii_pending == MII_READ) {
		*result = READ_REG((u32 *)(dev->base_addr + SOLOS_ETHER_MDIO)) & SOLOS_ETHER_MDIO_DATA_MASK;
		ctxt->mii_pending = MII_NONE;
		return 1;
	} else {
		u32 request;
		request = SOLOS_ETHER_MDIO_OP_READ | SOLOS_ETHER_MDIO_SFD | SOLOS_ETHER_MDIO_TA /* Set up a read */
		          | (phy << SOLOS_ETHER_MDIO_PHY_SHIFT)
		          | (reg << SOLOS_ETHER_MDIO_REG_SHIFT);
		/* Post read request */
		WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_MDIO), request);
		ctxt->mii_pending = MII_READ;
		return 0;
	}
}

static inline int solos_mii_write_posted(struct net_device *dev, int phy,
                     int reg, unsigned short value)
{
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;

	unsigned int status;
	u32 request;

	status = READ_REG((u32 *)(dev->base_addr + SOLOS_ETHER_STAT));

	if (ctxt->mii_pending && !(status & SOLOS_ETHER_STAT_MDIO)) {   /* Is the last MII op complete? */
		/* No. caller will just have to try again later... */
		return 0;
	}

	/* Clear the ready bit... */
	WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_STAT), SOLOS_ETHER_STAT_MDIO);

	if(ctxt->mii_pending == MII_WRITE)
	{
		ctxt->mii_pending = MII_NONE;
		return 1;
	}
	else
	{
		/*NS*/
		request = SOLOS_ETHER_MDIO_OP_WRITE | SOLOS_ETHER_MDIO_SFD | SOLOS_ETHER_MDIO_TA    /* Set up a write */
		          | (phy << SOLOS_ETHER_MDIO_PHY_SHIFT)
		          | (reg << SOLOS_ETHER_MDIO_REG_SHIFT)
		          | value;

		WRITE_REG((u32 *)(dev->base_addr + SOLOS_ETHER_MDIO), request);
		ctxt->mii_pending = MII_WRITE;
		return 0;
	}
}



/* Read MII register from specified ethernet PHY */
 int solos_mii_read(struct net_device *dev, int phy, int reg)
{
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;

	u16 value; 

	down(&priv->MacDevPtr->mutex); 
	while (!solos_mii_read_posted(dev, phy, reg, &value)) ;
	up(&priv->MacDevPtr->mutex);

	return value;
}

/* Write MII register to specified Ethernet PHY */
 int solos_mii_write(struct net_device *dev, int phy, int reg, int value)
{
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;
//	printk("\n dev :: %s phy %d reg %d val %d",dev->name,phy,reg,value);
	while (!solos_mii_write_posted(dev, phy, reg, value)) ;

	return 0;
}

int mdiobus_reset(void *bus)
{
	return 0;
}

int flag=0; 
int mdiobus_read(struct mii_bus *bus, int phy_addr, int regnum)
{
	struct net_device * dev = bus->priv;
	return solos_mii_read(dev,phy_addr,regnum);	
}

int mdiobus_write(struct mii_bus *bus, int phy_addr, int regnum, int value)
{
	struct net_device * dev = bus->priv;
	solos_mii_write(dev,phy_addr,regnum,value);
	return 0;
}

static void PhyCallback(struct net_device *dev )
{
	int phy_addr;
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;
	struct phy_device *phydev ;
	unsigned long flags;
	int status_change = 0;

	for (phy_addr = 0; phy_addr < ctxt->no_phys_detected; phy_addr++)
	{
		status_change=0;
		if (ctxt->phy_list[phy_addr])
		{
			phydev=ctxt->phy_list[phy_addr];
			// spin_lock_irqsave(&aup->lock, flags);
			if (phydev->link && (ctxt->old_speed[phy_addr] != phydev->speed))
			{
				// speed changed
				switch(phydev->speed) {
				case 10:
				case 100:
					break;
				default:
					printk("%s: Speed (%d) is not 10/100 ???\n",
					       dev->name, phydev->speed);
					break;
				}

				ctxt->old_speed[phy_addr] = phydev->speed;

				status_change = 1;
			}

			if (phydev->link && (ctxt->old_duplex[phy_addr] != phydev->duplex))
			{
				if (SOLOS_DUPLEX_FULL == phydev->duplex)
				{

				}
				else
				{

				}

				ctxt->old_duplex[phy_addr] =phydev->duplex;

				status_change = 1;
			}

			if(phydev->link != ctxt->old_link[phy_addr])
			{
				// link state changed

				if (phydev->link) { // link went up
					netif_schedule(dev);
				}
				else { // link went down
					ctxt->old_speed[phy_addr] = 0;
					ctxt->old_duplex[phy_addr] = -1;
				}

				ctxt->old_link[phy_addr] = phydev->link;
				status_change = 1;
			}

			if (status_change) {
				if (phydev->link)
				{
					if ( phydev->bus->vlan_status == 1 ) {
						printk("%s: port %d link up (%d/%s)\n",
						       priv->MacDevPtr->NdevList[phy_addr]->name,phydev->addr, phydev->speed, SOLOS_DUPLEX_FULL == phydev->duplex ? "Full" : "Half");
					}
					else {
						printk("%s: port %d link up (%d/%s)\n",
						       dev->name,phydev->addr, phydev->speed, SOLOS_DUPLEX_FULL == phydev->duplex ? "Full" : "Half");
					}
				}
				else {
					if ( phydev->bus->vlan_status == 1 ) {
						printk("%s: port %d link down\n", priv->MacDevPtr->NdevList[phy_addr]->name,phydev->addr);
					}
					else {
						printk("%s: port %d link down\n", dev->name,phydev->addr);
					}
				}
			}
		}
	}
}


int setup_phy_framework(struct mii_bus **mii_bus1,struct net_device *dev,char *name,int busid,struct phy_device *phy_list[], void *Callback,int old_link[],int old_duplex[],int old_speed[],int mode)
{
	int i,phy_addr;
	struct phy_device *phydev=NULL;
	struct mii_bus *mii_bus;
	int no_phys_detected=0;

	*mii_bus1= kmalloc(sizeof(struct mii_bus), GFP_KERNEL);
	mii_bus=*mii_bus1;

	mii_bus->priv = dev;

	mii_bus->dev=NULL;

	/* scan all the phy , ignore mask is now provided by the phy driver */
	mii_bus->phy_mask = 0;

	mii_bus->read  = mdiobus_read; /* Expects (device,phy_address,register*/
	mii_bus->write = mdiobus_write;   /* Expects (device,phy_address,register,value*/
	mii_bus->reset = mdiobus_reset;
	mii_bus->name  = name;
	mii_bus->id    = busid;
	mii_bus->irq   = kmalloc(sizeof(int)*PHY_MAX_ADDR, GFP_KERNEL);

	for(i = 0; i < PHY_MAX_ADDR; ++i)
		mii_bus->irq[i] = PHY_POLL;

	mdiobus_register(mii_bus);

	for (phy_addr = 0; phy_addr < PHY_MAX_ADDR; phy_addr++)
	{
		if (mii_bus->phy_map[phy_addr])
		{
			phydev = mii_bus->phy_map[phy_addr];
			phy_list[no_phys_detected]=phydev;
			if( mode == 1 )
			{
				old_link[no_phys_detected]=0;
				old_duplex[no_phys_detected]=-1;
				old_speed[no_phys_detected]=0;
			}
			phydev->link=0;
			phydev = phy_connect(dev, phydev->dev.bus_id,Callback, 0);
			if (IS_ERR(phydev)) {
				printk(KERN_ERR " Could not attach to PHY\n");
				kfree(mii_bus);
				*mii_bus1 = NULL;
				return PTR_ERR(phydev);
			}

			no_phys_detected ++;
		}
	}
	return no_phys_detected ;
}

static void solos_scan_mii(struct net_device *dev,int busid)
{
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* MacCtxt = &priv->MacDevPtr->MacContext;

	int i,phy_addr;
	struct phy_device *phydev=NULL;

	MacCtxt->no_phys_detected=
		setup_phy_framework(&MacCtxt->mii_bus, dev, 
		                    "solos_eth_mii", busid,
		                    &MacCtxt->phy_list,
		                    PhyCallback,
		                    &MacCtxt->old_link,
		                    &MacCtxt->old_duplex,
		                    MacCtxt->old_speed, 1);

/* Attach the network Device to First Phy Device */

	((struct NetDevPrivate *)priv->MacDevPtr->NdevList[0]->priv)->PhyDevice=MacCtxt->phy_list[0];

}

void TimerWait(u32 delay)
{
	set_current_state( TASK_INTERRUPTIBLE );
	/* Convert to jiffies. */
	schedule_timeout( (int)( (delay + 10) * HZ / 1000 ));
}


static inline RESET_GPIO()
{
#ifdef CONFIG_ARCH_SOLOSW_GALLUS
	/* Configure board to use Switch and not PHY */
	REGIO(DEVCONF, CONFIG_REGISTER) &= ~BF_DEVCONF_CONFIG_EPHY_MODE;
#if 0
	/* Set Ethernet Reset GPIO to output mode */
	solosw_gpio_set_mode(PHY_RESET_GPIO, SOLOSW_GPIO_CONTROL_OUTPUT);
	/*Hold the chip in reset */
	solosw_gpio_deassert_output(PHY_RESET_GPIO);   /* Take it low */
	TimerWait(1000);
	solosw_gpio_assert_output(PHY_RESET_GPIO);   /* Take it high */
	TimerWait(2000);
#endif
#endif
}

int MergeInterfaces( struct net_device *dev )
{
	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;
	int i;

	for ( i=1;i<ctxt->no_phys_detected  ;i++ )
	{
		struct NetDevPrivate *localpriv = priv->MacDevPtr->NdevList[i]->priv;
		struct phy_device *phydev = localpriv->PhyDevice;

		unregister_netdevice(priv->MacDevPtr->NdevList[i]);
	
//		unregister_netdev(priv->MacDevPtr->NdevList[i]);
		kfree(priv->MacDevPtr->NdevList[i]->priv);
		free_netdev(priv->MacDevPtr->NdevList[i]);
		priv->MacDevPtr->NdevList[i]=NULL;		
	}
#if 0
	for( i =0;i< ctxt->no_phys_detected;i++)
		phy_start(ctxt->phy_list[i]);	
#endif
}

int CreateSplitInterfaces( struct net_device *dev)
{

	struct NetDevPrivate *priv=(struct NetDevPrivate *) dev->priv;
	struct solos_eth_ctxt* ctxt = &priv->MacDevPtr->MacContext;
	int i;	
	int status;

	for ( i=1;i<ctxt->no_phys_detected ;i++)
	{
		struct net_device *new_dev = kmalloc( sizeof(struct net_device), GFP_KERNEL );
		if (!new_dev)
		{
			printk("No memory trying to create a device \n" );
			return -12;
		}

		memset( new_dev, 0, sizeof(struct net_device));
#ifdef FORCE_NAME
		/* Set the interface name */
//		portname[MAC_INDEX]=48 + 0;
		portname[PHY_INDEX]=48 + i;
		memcpy(new_dev->name,portname,strlen(portname));
#endif

		new_dev->init = bf_ether_init_net;
		struct NetDevPrivate *NDevPriv = kmalloc( sizeof(struct NetDevPrivate ), GFP_KERNEL );
		memset( NDevPriv, 0, sizeof(struct NetDevPrivate ));

		NDevPriv->MacDevPtr = priv->MacDevPtr;
		new_dev->priv=NDevPriv;
		new_dev->base_addr=priv->MacDevPtr->MacBaseAddr;
		new_dev->irq=priv->MacDevPtr->InterruptNo;

		/* Associate  Phy Device with This Netdevice */
		NDevPriv->PhyDevice= ctxt->phy_list[i];			

		if( (status = register_netdev( new_dev )) == 0 )
		{
			priv->MacDevPtr->NdevList[i]=new_dev;	
		}
		else
		{
			printk("\n Failed to register NetDevice ");
			return -1;
		}

		/* Stop The Phy now we will start it when corrosponding net device is opened */
		phy_stop( ctxt->phy_list[i] );
	}

//	unregister_netdev(dev);
	return 0;
}


static int bf_ether_load(void)
{
	struct net_device *new_dev;
	int n=0;
	int status=0;
	int i;
	int k;

	RESET_GPIO();

	for( i=0 ; i< sizeof(MacList)/sizeof(struct MAC); i++ )
	{
		struct MAC *MacDev = &MacList[i];
		struct NetDevPrivate *NDevPriv;
		u32 base_addr = MacDev->MacBaseAddr;
		u32 tmp;
		if ( MacDev->MacStatus != MAC_PRESENT )
			continue;

		if(READ_REG((u32 *)(base_addr + SOLOS_ETHER_ID)) == SOLOS_ETHER_ID_VALUE) 
		{
			n++;
			new_dev = kmalloc( sizeof(struct net_device), GFP_KERNEL );
			if (!new_dev)
			{
				printk("No memory trying to create a device \n" );
				return -12;
			}

			memset( new_dev, 0, sizeof(struct net_device));

			new_dev->init = bf_ether_init_net;
			NDevPriv = kmalloc( sizeof(struct NetDevPrivate ), GFP_KERNEL );
			memset( NDevPriv, 0, sizeof(struct NetDevPrivate ));
			NDevPriv->MacDevPtr = MacDev;
			new_dev->priv=NDevPriv;
			new_dev->base_addr=base_addr;
			new_dev->irq=MacDev->InterruptNo;

			MacDev->MacContext.n_txdescs = DEFAULT_NUM_TXDESCS;
			MacDev->MacContext.n_rxdescs = DEFAULT_NUM_RXDESCS;
            		/*NS Added the queue depth member*/
			MacDev->MacContext.max_tx_queue_depth = DEFAULT_TX_QUEUE_DEPTH;
			MacDev->MacContext.curr_tx_queue_depth = 0;

			/* Init. some register bits that are undefined on reset. */
			tmp = READ_REG((u32 *)(base_addr + SOLOS_ETHER_XTRACTRL));
			tmp &= ~SOLOS_ETHER_XTRACTRL_RESETMASK;
			WRITE_REG((u32 *)(base_addr + SOLOS_ETHER_XTRACTRL), tmp);

#ifdef FORCE_NAME
			//portname[MAC_INDEX]=48 + 0;
			portname[PHY_INDEX]=48 + i;
			memcpy(new_dev->name,portname,strlen(portname));
#endif

			if( (status = register_netdev( new_dev )) == 0 )
			{
				MacDev->NdevList[i]=new_dev;
			}
			else
			{
				printk("\n  Failed to register 10/100 MAC at 0x%x: %d\n", base_addr, status);
			}
		}
		init_MUTEX(&MacDev->mutex);
		MacDev->MacContext.opened=0;
		solos_scan_mii( new_dev,i);
	}

	if (n == 0) {
		printk("Failed to discover any MAC blocks. Suspicious as they are built-in!\n");
		status = -1;
	}

	return status;
}


static void __exit bf_ether_unload(void)
{
	int i;

	struct MAC *MacDev = &MacList[0];
	printk("\n Unregister ndev ...");
	unregister_netdev(MacDev->NdevList[0]);

//	solos_stop_hw(dev);
	/* If all the network devices are down then only stop the hardware */
//	as_free_irq(as_get_ndev_interrupt(dev),dev);
}
#if 0
void register_init(void **init,void **unload)
{
	*init = bf_ether_load;
	*unload = bf_ether_unload;
}
EXPORT_SYMBOL(register_init);
#endif

module_init(bf_ether_load);
module_cleanup(bf_ether_unload);
