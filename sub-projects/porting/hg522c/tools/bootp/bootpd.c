/************************************************************************
          Copyright 1988, 1991 by Carnegie Mellon University

                          All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of Carnegie Mellon University not be used
in advertising or publicity pertaining to distribution of the software
without specific, written prior permission.

CARNEGIE MELLON UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
IN NO EVENT SHALL CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
************************************************************************/

/*
 * BOOTP (bootstrap protocol) server daemon.
 *
 * Answers BOOTP request packets from booting client machines.
 * See [SRI-NIC]<RFC>RFC951.TXT for a description of the protocol.
 * See [SRI-NIC]<RFC>RFC1048.TXT for vendor-information extensions.
 * See RFC 1395 for option tags 14-17.
 * See accompanying man page -- bootpd.8
 *
 * HISTORY
 *	See ./Changes
 *
 * BUGS
 *	See ./ToDo
 */



#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/select.h>

#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>	/* inet_ntoa */

#ifndef	NO_UNISTD
#include <unistd.h>
#endif

#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <netdb.h>
#include <syslog.h>
#include <assert.h>

#ifdef	NO_SETSID
# include <fcntl.h>		/* for O_RDONLY, etc */
#endif

#ifndef	USE_BFUNCS
# include <memory.h>
/* Yes, memcpy is OK here (no overlapped copies). */
# define bcopy(a,b,c)    memcpy(b,a,c)
# define bzero(p,l)      memset(p,0,l)
# define bcmp(a,b,c)     memcmp(a,b,c)
#endif
#ifdef __linux__
/* Use sigaction to make signal last... */
inline void (*signal(int sig,void (*handler)(int)))(int) {
#if !defined(__GLIBC__)
	struct sigaction so,sa = {NULL,0,SA_NOMASK|SA_RESTART,NULL};
#else /* __GLIBC__ */
	struct sigaction so,sa;
        so.sa_handler = NULL;
        so.sa_flags = sa.sa_flags = SA_NOMASK|SA_RESTART;
        sigemptyset(&so.sa_mask);
        sigemptyset(&sa.sa_mask);
#endif /* __GLIBC__ */
	sa.sa_handler = handler;
	if (sigaction(sig,&sa,&so)<0) return NULL;
	return so.sa_handler;
}
#endif

#include "bootp.h"
#include "hash.h"
#include "hwaddr.h"
#include "bootpd.h"
#include "dovend.h"
#include "getif.h"
#include "readfile.h"
#include "report.h"
#include "tzone.h"
#include "patchlevel.h"

#ifndef CONFIG_FILE
#define CONFIG_FILE		"/etc/bootptab"
#endif
#ifndef DUMPTAB_FILE
#define DUMPTAB_FILE		"/var/run/bootpd.dump"
#endif



/*
 * Externals, forward declarations, and global variables
 */

#ifdef	__STDC__
#define P(args) args
#else
#define P(args) ()
#endif

extern void do_tftp(int sock, char *pktbuf, int pktlen, struct sockaddr_in recv_addr, socklen_t ra_len);

extern void dumptab P((char *));

PRIVATE void catcher P((int));
PRIVATE int chk_access P((char *, int32_t *));
#ifdef VEND_CMU
PRIVATE void dovend_cmu P((struct bootp *, struct host *));
#endif
PRIVATE int  dovend_rfc1048 P((struct bootp *, struct host *, int32_t));
PRIVATE void handle_reply P((void));
PRIVATE void handle_request P((void));
PRIVATE void sendreply P((int forward, int32_t dest_override));
PRIVATE void usage P((void));

#ifdef DHCP
PRIVATE int dhcp_discover P((struct bootp *, struct host *, byte *, int));
PRIVATE int dhcp_request P((struct bootp *, struct host *, byte *, int));
PRIVATE int dhcp_decline P((struct bootp *, struct host *, byte *, int));
PRIVATE int dhcp_release P((struct bootp *, struct host *, byte *, int));
PRIVATE int dhcp_offer P((struct bootp *, struct host *, byte *, int));
PRIVATE int dhcp_ack P((struct bootp *, struct host *, byte *, int));
PRIVATE int dhcp_lease P((struct bootp *, struct host *, byte **));
#endif

#undef	P

/*
 * IP port numbers for client and server obtained from /etc/services
 */

u_short bootps_port, bootpc_port;


/*
 * Internet socket and interface config structures
 */

struct sockaddr_in bind_addr;	/* Listening */
struct sockaddr_in recv_addr;	/* Packet source */
struct sockaddr_in send_addr;	/*  destination */


/*
 * option defaults
 */
int debug = 0;					/* Debugging flag (level) */
struct timeval actualtimeout =
{								/* fifteen minutes */
	15 * 60L,					/* tv_sec */
	0							/* tv_usec */
};

/*
 * General
 */

int s;							/* Socket file descriptor */
char *pktbuf;					/* Receive packet buffer */
int pktlen;
char *progname;
char *chdir_path;
struct in_addr my_ip_addr;

char *hostname;

/* Flags set by signal catcher. */
PRIVATE int do_readtab = 0;
PRIVATE int do_dumptab = 0;

/*
 * Globals below are associated with the bootp database file (bootptab).
 */

char *bootptab = CONFIG_FILE;
char *bootpd_dump = DUMPTAB_FILE;



/*
 * Initialization such as command-line processing is done and then the
 * main server loop is started.
 */

int
main(argc, argv)
	int argc;
	char **argv;
{
	struct timeval *timeout;
	struct bootp *bp;
	struct servent *servp;
	struct hostent *hep;
	struct utsname my_uname;
	char *stmp;
#if !defined(__GLIBC__)
	int n, ba_len, ra_len;
#else /* __GLIBC__ */
        int n;
        socklen_t ra_len, ba_len;
#endif /* __GLIBC__ */   
	int nfound;
	fd_set readfds;
	int standalone;
#ifdef	SA_NOCLDSTOP	/* Have POSIX sigaction(2). */
	struct sigaction sa;
#endif

	progname = strrchr(argv[0], '/');
	if (progname) progname++;
	else progname = argv[0];

	/*
	 * Initialize logging.
	 */
	report_init(0);				/* uses progname */

	/*
	 * Log startup
	 */
	report(LOG_INFO, "version %s.%d", VERSION, PATCHLEVEL);

	/* Debugging for compilers with struct padding. */
	assert(sizeof(struct bootp) == BP_MINPKTSZ);

	/* Get space for receiving packets and composing replies. */
	pktbuf = malloc(MAX_MSG_SIZE);
	if (!pktbuf) {
		report(LOG_ERR, "malloc failed");
		exit(1);
	}
	bp = (struct bootp *) pktbuf;

	/*
	 * Check to see if a socket was passed to us from inetd.
	 *
	 * Use getsockname() to determine if descriptor 0 is indeed a socket
	 * (and thus we are probably a child of inetd) or if it is instead
	 * something else and we are running standalone.
	 */
	s = 0;
	ba_len = sizeof(bind_addr);
	bzero((char *) &bind_addr, ba_len);
	errno = 0;
	standalone = TRUE;
	if (getsockname(s, (struct sockaddr *) &bind_addr, &ba_len) == 0) {
		/*
		 * Descriptor 0 is a socket.  Assume we are a child of inetd.
		 */
		if (bind_addr.sin_family == AF_INET) {
			standalone = FALSE;
			bootps_port = ntohs(bind_addr.sin_port);
		} else {
			/* Some other type of socket? */
			report(LOG_ERR, "getsockname: not an INET socket");
		}
	}

	/*
	 * Set defaults that might be changed by option switches.
	 */
	stmp = NULL;
	timeout = &actualtimeout;

	if (uname(&my_uname) < 0) {
		fprintf(stderr, "bootpd: can't get hostname\n");
		exit(1);
	}
	hostname = my_uname.nodename;

	/*
	 * Read switches.
	 */
	for (argc--, argv++; argc > 0; argc--, argv++) {
		if (argv[0][0] != '-')
			break;
		switch (argv[0][1]) {

		case 'c':				/* chdir_path */
			if (argv[0][2]) {
				stmp = &(argv[0][2]);
			} else {
				argc--;
				argv++;
				stmp = argv[0];
			}
			if (!stmp || (stmp[0] != '/')) {
				fprintf(stderr,
						"bootpd: invalid chdir specification\n");
				break;
			}
			chdir_path = stmp;
			break;

		case 'd':				/* debug level */
			if (argv[0][2]) {
				stmp = &(argv[0][2]);
			} else if (argv[1] && argv[1][0] == '-') {
				/*
				 * Backwards-compatible behavior:
				 * no parameter, so just increment the debug flag.
				 */
				debug++;
				break;
			} else {
				argc--;
				argv++;
				stmp = argv[0];
			}
			if (!stmp || (sscanf(stmp, "%d", &n) != 1) || (n < 0)) {
				fprintf(stderr,
						"%s: invalid debug level\n", progname);
				break;
			}
			debug = n;
			break;

		case 'h':				/* override hostname */
			if (argv[0][2]) {
				stmp = &(argv[0][2]);
			} else {
				argc--;
				argv++;
				stmp = argv[0];
			}
			if (!stmp) {
				fprintf(stderr,
						"bootpd: missing hostname\n");
				break;
			}
			hostname = stmp;
			break;

		case 'i':				/* inetd mode */
			standalone = FALSE;
			break;

		case 's':				/* standalone mode */
			standalone = TRUE;
			break;

		case 't':				/* timeout */
			if (argv[0][2]) {
				stmp = &(argv[0][2]);
			} else {
				argc--;
				argv++;
				stmp = argv[0];
			}
			if (!stmp || (sscanf(stmp, "%d", &n) != 1) || (n < 0)) {
				fprintf(stderr,
						"%s: invalid timeout specification\n", progname);
				break;
			}
			actualtimeout.tv_sec = (int32_t) (60 * n);
			/*
			 * If the actual timeout is zero, pass a NULL pointer
			 * to select so it blocks indefinitely, otherwise,
			 * point to the actual timeout value.
			 */
			timeout = (n > 0) ? &actualtimeout : NULL;
			break;

			case 'v':
#ifdef DHCP
			printf("bootpd+dhcp %s.%d\n", VERSION, PATCHLEVEL);
#else
			printf("bootpd %s.%d\n", VERSION, PATCHLEVEL);
#endif
			exit (0);

		default:
			fprintf(stderr, "%s: unknown switch: -%c\n",
					progname, argv[0][1]);
			usage();
			break;

		} /* switch */
	} /* for args */

	/*
	 * Override default file names if specified on the command line.
	 */
	if (argc > 0)
		bootptab = argv[0];

	if (argc > 1)
		bootpd_dump = argv[1];

	/*
	 * Get my hostname and IP address.
	 */

	hep = gethostbyname(hostname);
	if (!hep) {
		fprintf(stderr, "Can not get my IP address\n");
		exit(1);
	}
	bcopy(hep->h_addr, (char *)&my_ip_addr, sizeof(my_ip_addr));
	hostname = strdup(hep->h_name);
	if (!hostname) {
		report(LOG_ERR, "strdup failed");
		exit(1);
	}

	if (standalone) {
		/*
		 * Go into background and disassociate from controlling terminal.
		 */
		if (debug < 3) {
			if (fork())
				exit(0);
#ifdef	NO_SETSID
			setpgrp(0,0);
#ifdef TIOCNOTTY
			n = open("/dev/tty", O_RDWR);
			if (n >= 0) {
				ioctl(n, TIOCNOTTY, (char *) 0);
				(void) close(n);
			}
#endif	/* TIOCNOTTY */
#else	/* SETSID */
			if (setsid() < 0)
				perror("setsid");
#endif	/* SETSID */
		} /* if debug < 3 */

		/*
		 * Nuke any timeout value
		 */
		timeout = NULL;

	} /* if standalone (1st) */

	/* Set the cwd (i.e. to /tftpboot) */
	if (chdir_path) {
		if (chdir(chdir_path) < 0)
			report(LOG_ERR, "%s: chdir failed", chdir_path);
	}

	/* Get the timezone. */
	tzone_init();

	/* Allocate hash tables. */
	rdtab_init();

	/*
	 * Read the bootptab file.
	 */
	readtab(1);					/* force read */

	if (standalone) {

		/*
		 * Create a socket.
		 */
		if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			report(LOG_ERR, "socket: %s", get_network_errmsg());
			exit(1);
		}

		/*
		 * Get server's listening port number
		 */
#if 0
		servp = getservbyname("bootps", "udp");
		if (servp) {
			bootps_port = ntohs((u_short) servp->s_port);
		} else {
			bootps_port = (u_short) IPPORT_BOOTPS;
			report(LOG_ERR,
				   "udp/bootps: unknown service -- assuming port %d",
				   bootps_port);
		}
#else
		{
			bootps_port = (u_short) IPPORT_BOOTPS;
			report(LOG_INFO,
				   "udp/bootps: assuming port %d",
				   bootps_port);
		}
#endif

		/*
		 * Bind socket to BOOTPS port.
		 */
		bind_addr.sin_family = AF_INET;
		bind_addr.sin_addr.s_addr = INADDR_ANY;
		bind_addr.sin_port = htons(bootps_port);
		printf("bootps_port=%d\n", bootps_port);
		if (bind(s, (struct sockaddr *) &bind_addr,
				 sizeof(bind_addr)) < 0)
		{
			report(LOG_ERR, "bind: %s", get_network_errmsg());
			exit(1);
		}
	} /* if standalone (2nd)*/

	/*
	 * Get destination port number so we can reply to client
	 */
#if 0
	servp = getservbyname("bootpc", "udp");
	if (servp) {
		bootpc_port = ntohs(servp->s_port);
	} else {
		report(LOG_ERR,
			   "udp/bootpc: unknown service -- assuming port %d",
			   IPPORT_BOOTPC);
		bootpc_port = (u_short) IPPORT_BOOTPC;
	}
#else
	{
		report(LOG_INFO,
			   "udp/bootpc: assuming port %d",
			   IPPORT_BOOTPC);
		bootpc_port = (u_short) IPPORT_BOOTPC;
	}
#endif

#ifdef DHCP
	/*
	 * Maybe we have to broadcast, so enable it.
	 */
	n = 1;
	if (setsockopt(s,SOL_SOCKET,SO_BROADCAST,&n,sizeof(n))<0) {
		report(LOG_ERR, "setsockopt: %s\n", get_errmsg());
		exit(1);
	}
#endif

	/*
	 * Set up signals to read or dump the table.
	 */
#ifdef	SA_NOCLDSTOP	/* Have POSIX sigaction(2). */
	sa.sa_handler = catcher;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0) {
		report(LOG_ERR, "sigaction: %s", get_errmsg());
		exit(1);
	}
	if (sigaction(SIGUSR1, &sa, NULL) < 0) {
		report(LOG_ERR, "sigaction: %s", get_errmsg());
		exit(1);
	}
#else	/* SA_NOCLDSTOP */
	/* Old-fashioned UNIX signals */
	if ((int) signal(SIGHUP, catcher) < 0) {
		report(LOG_ERR, "signal: %s", get_errmsg());
		exit(1);
	}
	if ((int) signal(SIGUSR1, catcher) < 0) {
		report(LOG_ERR, "signal: %s", get_errmsg());
		exit(1);
	}
#endif	/* SA_NOCLDSTOP */

	/*
	 * Process incoming requests.
	 */
	for (;;) {
		struct timeval tv;

		FD_ZERO(&readfds);
		FD_SET(s, &readfds);
		if (timeout)
			tv = *timeout;

		nfound = select(s + 1, &readfds, NULL, NULL,
						(timeout) ? &tv : NULL);
		if (nfound < 0) {
			if (errno != EINTR) {
				report(LOG_ERR, "select: %s", get_errmsg());
			}
			/*
			 * Call readtab() or dumptab() here to avoid the
			 * dangers of doing I/O from a signal handler.
			 */
			if (do_readtab) {
				do_readtab = 0;
				readtab(1);		/* force read */
			}
			if (do_dumptab) {
				do_dumptab = 0;
				dumptab(bootpd_dump);
			}
			continue;
		}
		if (!FD_ISSET(s, &readfds)) {
			if (debug > 1)
				report(LOG_INFO, "exiting after %ld minutes of inactivity",
					   actualtimeout.tv_sec / 60);
			exit(0);
		}
		ra_len = sizeof(recv_addr);
		n = recvfrom(s, pktbuf, MAX_MSG_SIZE, 0,
					 (struct sockaddr *) &recv_addr, &ra_len);
		if (n <= 0) {
			continue;
		}
		if (debug > 1) {
			report(LOG_INFO, "recvd pkt from IP addr %s",
				   inet_ntoa(recv_addr.sin_addr));
		}
		if (n < sizeof(struct bootp)) {
			if (debug) {
				report(LOG_NOTICE, "received short packet");
			}
#if 0
			continue;
#else
			/* FIXME: Oops, this is only for HG522-C bootloader */
			{
				pktlen = n;
				do_tftp(s, pktbuf, pktlen, recv_addr, ra_len);
				break;
			}
#endif
		}
		pktlen = n;

		readtab(0);				/* maybe re-read bootptab */

		switch (bp->bp_op) {
		case BOOTREQUEST:
			handle_request();
			break;
		case BOOTREPLY:
			handle_reply();
			break;
		}
	}

	return 0;
}




/*
 * Print "usage" message and exit
 */

PRIVATE void
usage()
{
	fprintf(stderr,
			"usage:  bootpd [-d level] [-i] [-s] [-t timeout] [configfile [dumpfile]]\n");
	fprintf(stderr, "\t -c n\tset current directory\n");
	fprintf(stderr, "\t -d n\tset debug level\n");
	fprintf(stderr, "\t -i\tforce inetd mode (run as child of inetd)\n");
	fprintf(stderr, "\t -s\tforce standalone mode (run without inetd)\n");
	fprintf(stderr, "\t -t n\tset inetd exit timeout to n minutes\n");
	exit(1);
}

/* Signal catchers */
PRIVATE void
catcher(sig)
	int sig;
{
	if (sig == SIGHUP)
		do_readtab = 1;
	if (sig == SIGUSR1)
		do_dumptab = 1;
#if	!defined(SA_NOCLDSTOP) && defined(SYSV)
	/* For older "System V" derivatives with no sigaction(). */
	signal(sig, catcher);
#endif
}



/*
 * Process BOOTREQUEST packet.
 *
 * Note:  This version of the bootpd.c server never forwards
 * a request to another server.  That is the job of a gateway
 * program such as the "bootpgw" program included here.
 *
 * (Also this version does not interpret the hostname field of
 * the request packet;  it COULD do a name->address lookup and
 * forward the request there.)
 */
PRIVATE void
handle_request()
{
	struct bootp *bp = (struct bootp *) pktbuf;
	struct host *hp = NULL;
	struct host dummyhost;
	struct hostent *hep;
	int32_t bootsize = 0;
	unsigned hlen, hashcode;
	int32_t dest;
	char realpath[1024];
	char *clntpath;
	char *homedir, *bootfile;
	int n;
	int lpos;

	/* XXX - SLIP init: Set bp_ciaddr = recv_addr here? */

	/*
	 * If the servername field is set, compare it against us.
	 * If we're not being addressed, ignore this request.
	 * If the server name field is null, throw in our name.
	 */
	if (strlen(bp->bp_sname)) {
		hep = gethostbyname(bp->bp_sname);
		if (!hep || strcmp(hep->h_name, hostname)) {
			if (debug)
				report(LOG_INFO, "\
ignoring request for server %s from client at %s address %s",
					   bp->bp_sname, netname(bp->bp_htype),
					   haddrtoa(bp->bp_chaddr, bp->bp_hlen));
			/* XXX - Is it correct to ignore such a request? -gwr */
			return;
		}
	} else {
		strcpy(bp->bp_sname, hostname);
	}

	/* cevans - security as reported on Bugtraq! */
	if (bp->bp_htype >= hwinfocnt) {
		if (debug)
			report(LOG_INFO,
				"Request with unknown network type %u",
				bp->bp_htype);
		return;
	}


	/* Convert the request into a reply. */
	bp->bp_op = BOOTREPLY;
	if (bp->bp_ciaddr.s_addr == 0) {
		/*
		 * client doesnt know his IP address,
		 * search by hardware address.
		 */
		if (debug > 1) {
			report(LOG_INFO, "request from %s address %s",
				   netname(bp->bp_htype),
				   haddrtoa(bp->bp_chaddr, bp->bp_hlen));
		}
		hlen = haddrlength(bp->bp_htype);
		if (hlen != bp->bp_hlen) {
			report(LOG_NOTICE, "bad addr len from %s address %s",
				   netname(bp->bp_htype),
				   haddrtoa(bp->bp_chaddr, hlen));
		}
		dummyhost.htype = bp->bp_htype;
		bcopy(bp->bp_chaddr, dummyhost.haddr, hlen);
		hashcode = hash_HashFunction(bp->bp_chaddr, hlen);
		hp = (struct host *) hash_Lookup(hwhashtable, hashcode, hwlookcmp,
										 &dummyhost);
		if (hp == NULL &&
			bp->bp_htype == HTYPE_IEEE802)
		{
			/* Try again with address in "canonical" form. */
			haddr_conv802(bp->bp_chaddr, dummyhost.haddr, hlen);
			if (debug > 1) {
				report(LOG_INFO, "\
HW addr type is IEEE 802.  convert to %s and check again\n",
					   haddrtoa(dummyhost.haddr, bp->bp_hlen));
			}
			hashcode = hash_HashFunction(dummyhost.haddr, hlen);
			hp = (struct host *) hash_Lookup(hwhashtable, hashcode,
											 hwlookcmp, &dummyhost);
		}
		if (hp == NULL) {
			/*
			 * XXX - Add dynamic IP address assignment?
			 */
			if (debug)
				report(LOG_NOTICE, "unknown client %s address %s",
					   netname(bp->bp_htype),
					   haddrtoa(bp->bp_chaddr, bp->bp_hlen));
			return; /* not found */
		}

	} else {

		/*
		 * search by IP address.
		 */
		if (debug > 1) {
			report(LOG_INFO, "request from IP addr %s",
				   inet_ntoa(bp->bp_ciaddr));
		}
		dummyhost.iaddr.s_addr = bp->bp_ciaddr.s_addr;
		hashcode = hash_HashFunction((u_char *) &(bp->bp_ciaddr.s_addr), 4);
		hp = (struct host *) hash_Lookup(iphashtable, hashcode, iplookcmp,
										 &dummyhost);
		if (hp == NULL) {
			if (debug) {
				report(LOG_NOTICE, "IP address not found: %s",
					   inet_ntoa(bp->bp_ciaddr));
			}
			return;
		}
	}
	(bp->bp_yiaddr).s_addr = hp->iaddr.s_addr;

	if (debug) {
		report(LOG_INFO, "found %s (%s)", inet_ntoa(hp->iaddr),
			   hp->hostname->string);
	}

	/*
	 * If there is a response delay threshold, ignore requests
	 * with a timestamp lower than the threshold.
	 */
	if (hp->flags.min_wait) {
		u_int32_t t = (u_int32_t) ntohs(bp->bp_secs);
		if (t < hp->min_wait) {
			if (debug > 1)
				report(LOG_INFO,
					   "ignoring request due to timestamp (%d < %d)",
					   t, hp->min_wait);
			return;
		}
	}

#ifdef	YORK_EX_OPTION
	/*
	 * The need for the "ex" tag arose out of the need to empty
	 * shared networked drives on diskless PCs.  This solution is
	 * not very clean but it does work fairly well.
	 * Written by Edmund J. Sutcliffe <edmund@york.ac.uk>
	 *
	 * XXX - This could compromise security if a non-trusted user
	 * managed to write an entry in the bootptab with :ex=trojan:
	 * so I would leave this turned off unless you need it. -gwr
	 */
	/* Run a program, passing the client name as a parameter. */
	if (hp->flags.exec_file) {
		char tst[100];
		snprintf(tst, sizeof(tst), "%s %s &",
			hp->exec_file->string,
			hp->hostname->string);
		if (debug)
			report(LOG_INFO, "executing %s", tst);
		system(tst);	/* Hope this finishes soon... */
	}
#endif	/* YORK_EX_OPTION */

	/*
	 * If a specific TFTP server address was specified in the bootptab file,
	 * fill it in, otherwise zero it.
	 * XXX - Rather than zero it, should it be the bootpd address? -gwr
	 */
	(bp->bp_siaddr).s_addr = (hp->flags.bootserver) ?
		hp->bootserver.s_addr : 0L;

#ifdef	STANFORD_PROM_COMPAT
	/*
	 * Stanford bootp PROMs (for a Sun?) have no way to leave
	 * the boot file name field blank (because the boot file
	 * name is automatically generated from some index).
	 * As a work-around, this little hack allows those PROMs to
	 * specify "sunboot14" with the same effect as a NULL name.
	 * (The user specifies boot device 14 or some such magic.)
	 */
	if (strcmp(bp->bp_file, "sunboot14") == 0)
		bp->bp_file[0] = '\0';	/* treat it as unspecified */
#endif

	/*
	 * Fill in the client's proper bootfile.
	 *
	 * If the client specifies an absolute path, try that file with a
	 * ".host" suffix and then without.  If the file cannot be found, no
	 * reply is made at all.
	 *
	 * If the client specifies a null or relative file, use the following
	 * table to determine the appropriate action:
	 *
	 *  Homedir      Bootfile    Client's file
	 * specified?   specified?   specification   Action
	 * -------------------------------------------------------------------
	 *      No          No          Null         Send null filename
	 *      No          No          Relative     Discard request
	 *      No          Yes         Null         Send if absolute else null
	 *      No          Yes         Relative     Discard request     *XXX
	 *      Yes         No          Null         Send null filename
	 *      Yes         No          Relative     Lookup with ".host"
	 *      Yes         Yes         Null         Send home/boot or bootfile
	 *      Yes         Yes         Relative     Lookup with ".host" *XXX
	 *
	 */

	/*
	 * XXX - I don't like the policy of ignoring a client when the
	 * boot file is not accessible.  The TFTP server might not be
	 * running on the same machine as the BOOTP server, in which
	 * case checking accessibility of the boot file is pointless.
	 *
	 * Therefore, file accessibility is now demanded ONLY if you
	 * define CHECK_FILE_ACCESS in the Makefile options. -gwr
	 */

	/*
	 * The "real" path is as seen by the BOOTP daemon on this
	 * machine, while the client path is relative to the TFTP
	 * daemon chroot directory (i.e. /tftpboot).
	 *
	 * The bootfile might not be properly zero terminated. We
	 * need to play safe - AC
	 */
	if (hp->flags.tftpdir) {
		lpos=strlen(hp->tftpdir->string);
		if(lpos>=sizeof(realpath)-1)
			return;
		strcpy(realpath, hp->tftpdir->string);
		clntpath = &realpath[lpos];
	} else {
		realpath[0] = '\0';
		clntpath = realpath;
		lpos=0;
	}

	/*
	 * Determine client's requested homedir and bootfile.
	 */
	homedir = NULL;
	bootfile = NULL;
	if (bp->bp_file[0]) {
		homedir = bp->bp_file;
		bootfile = strrchr(homedir, '/');
		if (bootfile) {
			if (homedir == bootfile)
				homedir = NULL;
			*bootfile++ = '\0';
		} else {
			/* no "/" in the string */
			bootfile = homedir;
			homedir = NULL;
		}
		if (debug > 2) {
			report(LOG_INFO, "requested path=\"%s\"  file=\"%s\"",
				   (homedir) ? homedir : "",
				   (bootfile) ? bootfile : "");
		}
	}

	/*
	 * Specifications in bootptab override client requested values.
	 */
	if (hp->flags.homedir)
		homedir = hp->homedir->string;
	if (hp->flags.bootfile)
		bootfile = hp->bootfile->string;

	/*
	 * Construct bootfile path.
	 */
	if (homedir) {
		if (homedir[0] != '/')
			strcat(clntpath, "/");
		lpos+=strlen(homedir);
		if(lpos>=sizeof(realpath))
			return;
		strcat(clntpath, homedir);
		homedir = NULL;
	}
	if (bootfile) {
		if (bootfile[0] != '/')
			strcat(clntpath, "/");
		lpos+=strlen(bootfile);
		if(lpos>=sizeof(realpath))
			return;
		strcat(clntpath, bootfile);
		bootfile = NULL;
	}

	/*
	 * First try to find the file with a ".host" suffix
	 */
	n = strlen(clntpath);
	
	/*
	 *	Don't test if it wont fit.
	 */
	if(n+1+strlen(hp->hostname->string)<sizeof(realpath))
	{
		strcat(clntpath, ".");
		strcat(clntpath, hp->hostname->string);
	}
	if (chk_access(realpath, &bootsize) < 0) {
		clntpath[n] = 0;			/* Try it without the suffix */
		if (chk_access(realpath, &bootsize) < 0) {
			/* neither "file.host" nor "file" was found */
#ifdef	CHECK_FILE_ACCESS

			if (bp->bp_file[0]) {
				/*
				 * Client wanted specific file
				 * and we didn't have it.
				 */
				report(LOG_NOTICE,
					   "requested file not found: \"%s\"", clntpath);
				return;
			}
			/*
			 * Client didn't ask for a specific file and we couldn't
			 * access the default file, so just zero-out the bootfile
			 * field in the packet and continue processing the reply.
			 */
			bzero(bp->bp_file, sizeof(bp->bp_file));
			goto null_file_name;

#else	/* CHECK_FILE_ACCESS */

			/* Complain only if boot file size was needed. */
			if (hp->flags.bootsize_auto) {
				report(LOG_ERR, "can not determine size of file \"%s\"",
					   clntpath);
			}

#endif	/* CHECK_FILE_ACCESS */
		}
	}
	strncpy(bp->bp_file, clntpath, BP_FILE_LEN);
	if (debug > 2)
		report(LOG_INFO, "bootfile=\"%s\"", clntpath);

#ifdef	CHECK_FILE_ACCESS
null_file_name:
#endif	/* CHECK_FILE_ACCESS */


	/*
	 * Handle vendor options based on magic number.
	 */

	if (debug > 1) {
		report(LOG_INFO, "vendor magic field is %d.%d.%d.%d",
			   (int) ((bp->bp_vend)[0]),
			   (int) ((bp->bp_vend)[1]),
			   (int) ((bp->bp_vend)[2]),
			   (int) ((bp->bp_vend)[3]));
	}
	/*
	 * If this host isn't set for automatic vendor info then copy the
	 * specific cookie into the bootp packet, thus forcing a certain
	 * reply format.  Only force reply format if user specified it.
	 */
	if (hp->flags.vm_cookie) {
		/* Slam in the user specified magic number. */
		bcopy(hp->vm_cookie, bp->bp_vend, 4);
	}
	/*
	 * Figure out the format for the vendor-specific info.
	 * Note that bp->bp_vend may have been set above.
	 */
	if (!bcmp(bp->bp_vend, vm_rfc1048, 4)) {
		/* RFC1048 conformant bootp client */
		if (!dovend_rfc1048(bp, hp, bootsize))
			return;
		if (debug > 1) {
			report(LOG_INFO, "sending reply (with RFC1048 options)");
		}
	}
#ifdef VEND_CMU
	else if (!bcmp(bp->bp_vend, vm_cmu, 4)) {
		dovend_cmu(bp, hp);
		if (debug > 1) {
			report(LOG_INFO, "sending reply (with CMU options)");
		}
	}
#endif
	else {
		if (debug > 1) {
			report(LOG_INFO, "sending reply (with no options)");
		}
	}

	dest = (hp->flags.reply_addr) ?
		hp->reply_addr.s_addr : 0L;

	/* not forwarded */
	sendreply(0, dest);
}


/*
 * Process BOOTREPLY packet.
 */
PRIVATE void
handle_reply()
{
	if (debug) {
		report(LOG_INFO, "processing boot reply");
	}
	/* forwarded, no destination override */
	sendreply(1, 0);
}


/*
 * Send a reply packet to the client.  'forward' flag is set if we are
 * not the originator of this reply packet.
 */
PRIVATE void
sendreply(forward, dst_override)
	int forward;
	int32_t dst_override;
{
	struct bootp *bp = (struct bootp *) pktbuf;
	struct in_addr dst;
	u_short port = bootpc_port;
	unsigned char *ha;
	int len, haf;

	/*
	 * If the destination address was specified explicitly
	 * (i.e. the broadcast address for HP compatiblity)
	 * then send the response to that address.  Otherwise,
	 * act in accordance with RFC951:
	 *   If the client IP address is specified, use that
	 * else if gateway IP address is specified, use that
	 * else make a temporary arp cache entry for the client's
	 * NEW IP/hardware address and use that.
	 */
	if (dst_override) {
		dst.s_addr = dst_override;
		if (debug > 1) {
			report(LOG_INFO, "reply address override: %s",
				   inet_ntoa(dst));
		}
	} else if (bp->bp_ciaddr.s_addr) {
		dst = bp->bp_ciaddr;
	} else if (bp->bp_giaddr.s_addr && forward == 0) {
		dst = bp->bp_giaddr;
		port = bootps_port;
		if (debug > 1) {
			report(LOG_INFO, "sending reply to gateway %s",
				   inet_ntoa(dst));
		}
	} else if (ntohs(bp->bp_flags) & 0x8000) {
		struct ifreq *ifr;
		ifr = getif(s, &bp->bp_yiaddr);
		if (ifr) {
			struct sockaddr_in *bip;
			struct ifreq myreq;
			strcpy( myreq.ifr_name, ifr->ifr_name );
			if (ioctl(s, SIOCGIFBRDADDR, &myreq) < 0) {
			    report(LOG_ERR, "ioctl SIOCGIFBRDADDR");
			    dst.s_addr = INADDR_BROADCAST;
			} else {
			    bip = (struct sockaddr_in *)&myreq.ifr_broadaddr;
			    dst = bip->sin_addr;
			}
		} else {
			dst.s_addr = INADDR_BROADCAST;
		}
	} else {
		dst = bp->bp_yiaddr;
		ha = bp->bp_chaddr;
		len = bp->bp_hlen;
		if (len > MAXHADDRLEN)
			len = MAXHADDRLEN;
		haf = (int) bp->bp_htype;
		if (haf == 0)
			haf = HTYPE_ETHERNET;

		if (debug > 1)
			report(LOG_INFO, "setarp %s - %s",
				   inet_ntoa(dst), haddrtoa(ha, len));
		setarp(s, &dst, haf, ha, len);
	}

	if ((forward == 0) &&
		(bp->bp_siaddr.s_addr == 0))
	{
		struct ifreq *ifr;
		struct in_addr siaddr;
		/*
		 * If we are originating this reply, we
		 * need to find our own interface address to
		 * put in the bp_siaddr field of the reply.
		 * If this server is multi-homed, pick the
		 * 'best' interface (the one on the same net
		 * as the client).  Of course, the client may
		 * be on the other side of a BOOTP gateway...
		 */
		ifr = getif(s, &dst);
		if (ifr) {
			struct sockaddr_in *sip;
			sip = (struct sockaddr_in *) &(ifr->ifr_addr);
			siaddr = sip->sin_addr;
		} else {
			/* Just use my "official" IP address. */
			siaddr = my_ip_addr;
		}

		/* XXX - No need to set bp_giaddr here. */

		/* Finally, set the server address field. */
		bp->bp_siaddr = siaddr;
	}
	/* Set up socket address for send. */
	send_addr.sin_family = AF_INET;
	send_addr.sin_port = htons(port);
	send_addr.sin_addr = dst;

	/* Send reply with same size packet as request used. */
	if (sendto(s, pktbuf, pktlen, 0,
			   (struct sockaddr *) &send_addr,
			   sizeof(send_addr)) < 0)
	{
		report(LOG_ERR, "sendto: %s", get_network_errmsg());
	}
} /* sendreply */


/* nmatch() - now in getif.c */
/* setarp() - now in hwaddr.c */


/*
 * This call checks read access to a file.  It returns 0 if the file given
 * by "path" exists and is publically readable.  A value of -1 is returned if
 * access is not permitted or an error occurs.  Successful calls also
 * return the file size in bytes using the long pointer "filesize".
 *
 * The read permission bit for "other" users is checked.  This bit must be
 * set for tftpd(8) to allow clients to read the file.
 */

PRIVATE int
chk_access(path, filesize)
	char *path;
	int32_t *filesize;
{
	struct stat st;

	if ((stat(path, &st) == 0) && (st.st_mode & (S_IREAD >> 6))) {
		*filesize = (int32_t) st.st_size;
		return 0;
	} else {
		return -1;
	}
}


/*
 * Now in dumptab.c :
 *	dumptab()
 *	dump_host()
 *	list_ipaddresses()
 */

#ifdef VEND_CMU

/*
 * Insert the CMU "vendor" data for the host pointed to by "hp" into the
 * bootp packet pointed to by "bp".
 */

PRIVATE void
dovend_cmu(bp, hp)
	struct bootp *bp;
	struct host *hp;
{
	struct cmu_vend *vendp;
	struct in_addr_list *taddr;

	/*
	 * Initialize the entire vendor field to zeroes.
	 */
	bzero(bp->bp_vend, sizeof(bp->bp_vend));

	/*
	 * Fill in vendor information. Subnet mask, default gateway,
	 * domain name server, ien name server, time server
	 */
	vendp = (struct cmu_vend *) bp->bp_vend;
	strcpy(vendp->v_magic, (char *)vm_cmu);
	if (hp->flags.subnet_mask) {
		(vendp->v_smask).s_addr = hp->subnet_mask.s_addr;
		(vendp->v_flags) |= VF_SMASK;
		if (hp->flags.gateway) {
			(vendp->v_dgate).s_addr = hp->gateway->addr->s_addr;
		}
	}
	if (hp->flags.domain_server) {
		taddr = hp->domain_server;
		if (taddr->addrcount > 0) {
			(vendp->v_dns1).s_addr = (taddr->addr)[0].s_addr;
			if (taddr->addrcount > 1) {
				(vendp->v_dns2).s_addr = (taddr->addr)[1].s_addr;
			}
		}
	}
	if (hp->flags.name_server) {
		taddr = hp->name_server;
		if (taddr->addrcount > 0) {
			(vendp->v_ins1).s_addr = (taddr->addr)[0].s_addr;
			if (taddr->addrcount > 1) {
				(vendp->v_ins2).s_addr = (taddr->addr)[1].s_addr;
			}
		}
	}
	if (hp->flags.time_server) {
		taddr = hp->time_server;
		if (taddr->addrcount > 0) {
			(vendp->v_ts1).s_addr = (taddr->addr)[0].s_addr;
			if (taddr->addrcount > 1) {
				(vendp->v_ts2).s_addr = (taddr->addr)[1].s_addr;
			}
		}
	}
	/* Log message now done by caller. */
} /* dovend_cmu */

#endif /* VEND_CMU */



/*
 * Insert the RFC1048 vendor data for the host pointed to by "hp" into the
 * bootp packet pointed to by "bp".
 */
#define	NEED(LEN, MSG) do \
	if (bytesleft < (LEN)) { \
		report(LOG_NOTICE, noroom, \
			   hp->hostname->string, MSG); \
		return 0; \
	} while (0)
PRIVATE int
dovend_rfc1048(bp, hp, bootsize)
	struct bootp *bp;
	struct host *hp;
	int32_t bootsize;
{
	int bytesleft, len;
	byte *vp;
#ifdef DHCP
	int dhcp = 0;
	int isme = TRUE;	/* DHCP uses this for not-mine-requests */
#endif

	static char noroom[] = "%s: No room for \"%s\" option";

	vp = bp->bp_vend;

	if (hp->flags.msg_size) {
		pktlen = hp->msg_size;
	} else {
		/*
		 * If the request was longer than the official length, build
		 * a response of that same length where the additional length
		 * is assumed to be part of the bp_vend (options) area.
		 */
		if (pktlen > sizeof(*bp)) {
			if (debug > 1)
				report(LOG_INFO, "request message length=%d", pktlen);
		}
		/*
		 * Check whether the request contains the option:
		 * Maximum DHCP Message Size (RFC1533 sec. 9.8)
		 * and if so, override the response length with its value.
		 * This request must lie within the first BP_VEND_LEN
		 * bytes of the option space.
		 */
		{
			byte *p, *ep;
			byte tag, len;
			short msgsz = 0;
			
			p = vp + 4;
			ep = p + BP_VEND_LEN - 4;
			while (p < ep) {
				tag = *p++;
				/* Check for tags with no data first. */
				if (tag == TAG_PAD)
					continue;
				if (tag == TAG_END)
					break;
				/* Now scan the length byte. */
				len = *p++;
				switch (tag) {
				case TAG_MAX_MSGSZ:
					if (len == 2) {
						bcopy(p, (char*)&msgsz, 2);
						msgsz = ntohs(msgsz);
					}
					break;
				case TAG_SUBNET_MASK:
					/* XXX - Should preserve this if given... */
					break;
#ifdef DHCP
				case TAG_DHCP_MSG:
					dhcp = *p;
					break;
				case TAG_DHCP_SERVERID:
					{
						struct in_addr tmp_addr;
						isme = (len == 4) && (
							memcpy(
								&tmp_addr, p,
								sizeof(tmp_addr)
							),
							my_ip_addr.s_addr ==
							tmp_addr.s_addr
						);
					}
					break;
#endif
				} /* switch */
				p += len;
			}

			if (msgsz > sizeof(*bp)) {
				if (debug > 1)
					report(LOG_INFO, "request has DHCP msglen=%d", msgsz);
				pktlen = msgsz;
			}
		}
	}

	if (pktlen < sizeof(*bp)) {
		report(LOG_ERR, "invalid response length=%d", pktlen);
		pktlen = sizeof(*bp);
	}
	bytesleft = ((byte*)bp + pktlen) - vp;
	if (pktlen > sizeof(*bp)) {
		if (debug > 1)
			report(LOG_INFO, "extended reply, length=%d, options=%d",
				   pktlen, bytesleft);
	}

	/* Copy in the magic cookie */
	bcopy(vm_rfc1048, vp, 4);
	vp += 4;
	bytesleft -= 4;

	if (hp->flags.subnet_mask) {
		/* always enough room here. */
		*vp++ = TAG_SUBNET_MASK;/* -1 byte  */
		*vp++ = 4;				/* -1 byte  */
		insert_u_long(hp->subnet_mask.s_addr, &vp);	/* -4 bytes */
		bytesleft -= 6;			/* Fix real count */
		if (hp->flags.gateway) {
			(void) insert_ip(TAG_GATEWAY,
							 hp->gateway,
							 &vp, &bytesleft);
		}
	}
	if (hp->flags.bootsize) {
		/* always enough room here */
		bootsize = (hp->flags.bootsize_auto) ?
			((bootsize + 511) / 512) : (hp->bootsize);	/* Round up */
		*vp++ = TAG_BOOT_SIZE;
		*vp++ = 2;
		*vp++ = (byte) ((bootsize >> 8) & 0xFF);
		*vp++ = (byte) (bootsize & 0xFF);
		bytesleft -= 4;			/* Tag, length, and 16 bit blocksize */
	}
	/*
	 * This one is special: Remaining options go in the ext file.
	 * Only the subnet_mask, bootsize, and gateway should precede.
	 */
	if (hp->flags.exten_file) {
		/*
		 * Check for room for exten_file.  Add 3 to account for
		 * TAG_EXTEN_FILE, length, and TAG_END.
		 */
		len = strlen(hp->exten_file->string);
		NEED((len + 3), "ef");
		*vp++ = TAG_EXTEN_FILE;
		*vp++ = (byte) (len & 0xFF);
		bcopy(hp->exten_file->string, vp, len);
		vp += len;
		*vp++ = TAG_END;
		bytesleft -= len + 3;
		return 1;				/* no more options here. */
	}

#ifdef DHCP
	/*
	 * Check if this is a DHCP request.
	 */
	if (dhcp!=0) {
		if (!isme)
			return 0;	/* Not mine, discard! */

		switch (dhcp) {
		 case 1 : len = dhcp_discover(bp,hp,vp,bytesleft); break;
		 case 3 : len = dhcp_request(bp,hp,vp,bytesleft); break;
		 case 4 : len = dhcp_decline(bp,hp,vp,bytesleft); break;
		 case 7 : len = dhcp_release(bp,hp,vp,bytesleft); break;
		 default : report(LOG_NOTICE,"Unknown DHCP request (%d)",dhcp);
			   return 0;
		}
		/* Is there a DHCP reply at all? */
		if (len==0)
			return 0;
		vp += len;
		bytesleft -= len;
	}
	else {
#endif
		/*
		 * The remaining options are inserted by the following
		 * function (which is shared with bootpef.c).
		 * Keep back one byte for the TAG_END.
		 */
		len = dovend_rfc1497(hp, vp, bytesleft);
		vp += len;
		bytesleft -= len;
#ifdef DHCP
	}
#endif

	/* There should be at least one byte left. */
	NEED(1, "(end)");
	*vp++ = TAG_END;
	bytesleft--;

	/* Log message done by caller. */

	/* Remove unnecessary bits. */
	pktlen -= bytesleft;
	bytesleft = 64 - (vp - bp->bp_vend);
	if (bytesleft > 0) {
		/*
		 * Zero out any remaining part of the vendor area.
		 */
		bzero(vp, bytesleft);
		pktlen += bytesleft;
	}

	return 1;	/* sent reply */
} /* dovend_rfc1048 */
#undef	NEED


/*
 * Now in readfile.c:
 * 	hwlookcmp()
 *	iplookcmp()
 */

/* haddrtoa() - now in hwaddr.c */
/*
 * Now in dovend.c:
 * insert_ip()
 * insert_generic()
 * insert_u_long()
 */

/* get_errmsg() - now in report.c */


#ifdef DHCP

/*
 * PeP hic facet
 * Stuff the packet with the Lease info, We need to do this on the Offer and
 * the ack so separated out here
 */
PRIVATE
int dhcp_lease(bp, hp, vp)
    struct bootp *bp;
    struct host *hp;
    byte **vp;
{
	*(*vp)++ = TAG_DHCP_IPRENEW;	/* DHCP Renewal time 50% of lease */
	*(*vp)++ = 4;			/* Length */
	insert_u_long(htonl(hp->dhcp_lease/2),vp);

	*(*vp)++ = TAG_DHCP_IPREBIND;	/* DHCP Rebinding time 85% of lease */
	*(*vp)++ = 4;
	insert_u_long(htonl(hp->dhcp_lease*7/8),vp);
	
	*(*vp)++ = TAG_DHCP_IPLEASE;	/* IP address lease time */
	*(*vp)++ = 4;			/* Length */
	insert_u_long(htonl(hp->dhcp_lease),vp); /* PeP hic facet, lets see if this works */

	return(19);
}


/*
 * Formulate an DHCP_DISCOVER reply
 */
PRIVATE
int dhcp_discover(bp, hp, vp, bytesleft)
    struct bootp *bp;
    struct host *hp;
    byte *vp;
    int bytesleft;
{
	if(debug)
		report(LOG_INFO, "Received: DHCPDISCOVER");
	return(dhcp_offer(bp,hp,vp,bytesleft));
}

/*
 * formulate an DHCP_RELEASE reply
 */
PRIVATE
int dhcp_release(bp, hp, vp, bytesleft)
    struct bootp *bp;
    struct host *hp;
    byte *vp;
    int bytesleft;
{
	if (debug)
		report(LOG_INFO, "Received: DHCPRELEASE (discarded)");
	return 0;
}

PRIVATE
int dhcp_offer(bp, hp, vp, bytesleft)
    struct bootp *bp;
    struct host *hp;
    byte *vp;
    int bytesleft;
{
	int len=0;
	if (debug)
		report(LOG_INFO, "Sent: DHCPOFFER");

	bp->bp_secs = bp->bp_hops = 0;
	bp->bp_ciaddr.s_addr = 0;

	*vp++ = TAG_DHCP_MSG;		/* DHCP */
	*vp++ = 1;			/* length */
	*vp++ = 2;			/* DHCPOFFER */
	len +=  3;	

	if (hp->dhcp_lease)
		len += dhcp_lease(bp,hp,&vp);

	*vp++ = TAG_DHCP_SERVERID;
	*vp++ = 4;
	insert_u_long(my_ip_addr.s_addr,&vp);
	len += 6;

	return len + dovend_rfc1497(hp, vp, bytesleft - len);
}

/*
 * Formulate an DHCP_REQUEST reply
 */
PRIVATE
int dhcp_request(bp, hp, vp, bytesleft)
    struct bootp *bp;
    struct host *hp;
    byte *vp;
    int bytesleft;
{
	bp->bp_secs = bp->bp_hops = 0;

	if(debug)
		report(LOG_INFO,"Received: DHCPREQUEST");
	/*
	 * Make absolutely sure that if the client requests an address,
	 * it is its own address, and also make sure the hardware
	 * addresses match perfectly. We want to minimize spoofing!
	 */
	if ((bp->bp_ciaddr.s_addr && bp->bp_ciaddr.s_addr!=bp->bp_yiaddr.s_addr) ||
	    bp->bp_htype != hp->htype ||
            bcmp(bp->bp_chaddr, hp->haddr, haddrlength(hp->htype))) {
		if (debug)
			report(LOG_INFO, "Sent: DHCPNAK");

		*vp++ = TAG_DHCP_MSG;	/* DHCPNAK */
		*vp++ = 1;
		*vp++ = 6;
		return 3;
	}
	else 
		return(dhcp_ack(bp,hp,vp,bytesleft));	
}

PRIVATE
int dhcp_ack(bp, hp, vp, bytesleft)
    struct bootp *bp;
    struct host *hp;
    byte *vp;
    int bytesleft;
{
	int len=0;
	if (debug)
		report(LOG_INFO, "Sent: DHCPACK");

	*vp++ = TAG_DHCP_MSG;	/* DHCPACK */
	*vp++ = 1;
	*vp++ = 5;
	len += 3;

	if (hp->dhcp_lease)
		len += dhcp_lease(bp,hp,&vp);

	*vp++ = TAG_DHCP_SERVERID;	/* Server id */
	*vp++ = 4;
	insert_u_long(my_ip_addr.s_addr,&vp);
	len += 6;

	return len + dovend_rfc1497(hp, vp, bytesleft - len);
}

/*
 * formulate an DHCP_DECLINE reply
 */
PRIVATE
int dhcp_decline(bp, hp, vp, bytesleft)
    struct bootp *bp;
    struct host *hp;
    byte *vp;
    int bytesleft;
{
	if (debug)
		report(LOG_INFO, "Received: DHCPDECLINE (ignored)");
	return 0;
}
#endif

/*
 * Local Variables:
 * tab-width: 4
 * c-indent-level: 4
 * c-argdecl-indent: 4
 * c-continued-statement-offset: 4
 * c-continued-brace-offset: -4
 * c-label-offset: -4
 * c-brace-offset: 0
 * End:
 */