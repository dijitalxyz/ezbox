/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_network_interfaces.c
 *
 * Description  : ezbox /etc/network/interfaces generating program
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-03   0.1       Write it from scratch
 * ============================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>
#include <net/if.h>

#include "ezcd.h"

int pop_etc_network_interfaces(int flag)
{
	FILE *file;
	char lan_ifname[IFNAMSIZ];
	int lan_ipaddr[4];
	int lan_netmask[4];
	char wan_ifname[IFNAMSIZ];
	int wan_ipaddr[4];
	int wan_netmask[4];

	file = fopen("/etc/network/interfaces", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	snprintf(lan_ifname, sizeof(lan_ifname), "eth0");

	lan_ipaddr[0] = 192;
	lan_ipaddr[1] = 168;
	lan_ipaddr[2] = 1;
	lan_ipaddr[3] = 1;

	lan_netmask[0] = 255;
	lan_netmask[1] = 255;
	lan_netmask[2] = 255;
	lan_netmask[3] = 0;

	snprintf(wan_ifname, sizeof(wan_ifname), "eth1");

	wan_ipaddr[0] = 10;
	wan_ipaddr[1] = 10;
	wan_ipaddr[2] = 10;
	wan_ipaddr[3] = 10;

	wan_netmask[0] = 255;
	wan_netmask[1] = 0;
	wan_netmask[2] = 0;
	wan_netmask[3] = 0;

	switch (flag) {
	case RC_BOOT :
		/* get the kernel module name from kernel cmdline */
		break;
	case RC_START :
		/* set loopback & lan interface auto start */
		//fprintf(file, "auto lo %s\n", lan_ifname);
		//fprintf(file, "\n");

		/* set loopback */
		fprintf(file, "iface lo inet loopback\n");
		fprintf(file, "\n");

		/* set lan interface */
		fprintf(file, "iface %s inet static\n", lan_ifname);
		fprintf(file, "\taddress %d.%d.%d.%d\n",
			lan_ipaddr[0],
			lan_ipaddr[1],
			lan_ipaddr[2],
			lan_ipaddr[3]);
		fprintf(file, "\tnetmask %d.%d.%d.%d\n",
			lan_netmask[0],
			lan_netmask[1],
			lan_netmask[2],
			lan_netmask[3]);
		fprintf(file, "\tnetwork %d.%d.%d.%d\n",
			lan_ipaddr[0] & lan_netmask[0],
			lan_ipaddr[1] & lan_netmask[1],
			lan_ipaddr[2] & lan_netmask[2],
			lan_ipaddr[3] & lan_netmask[3]);
		fprintf(file, "\n");

		/* set wan interface */
		fprintf(file, "iface %s inet static\n", wan_ifname);
		fprintf(file, "\taddress %d.%d.%d.%d\n",
			wan_ipaddr[0],
			wan_ipaddr[1],
			wan_ipaddr[2],
			wan_ipaddr[3]);
		fprintf(file, "\tnetmask %d.%d.%d.%d\n",
			wan_netmask[0],
			wan_netmask[1],
			wan_netmask[2],
			wan_netmask[3]);
		fprintf(file, "\tnetwork %d.%d.%d.%d\n",
			wan_ipaddr[0] & wan_netmask[0],
			wan_ipaddr[1] & wan_netmask[1],
			wan_ipaddr[2] & wan_netmask[2],
			wan_ipaddr[3] & wan_netmask[3]);
		fprintf(file, "\n");
		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
