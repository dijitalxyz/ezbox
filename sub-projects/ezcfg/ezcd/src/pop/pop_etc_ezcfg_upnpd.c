/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_ezcfg_upnpd.c
 *
 * Description  : ezbox /etc/ezcfg_upnpd/ files generating program
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-27   0.1       Write it from scratch
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

#include "ezcd.h"

#if 0
#define DBG(format, args...) do {\
	FILE *fp = fopen("/dev/kmsg", "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

#define fprintf_indent(i, fp, format, args...) do { \
	int j; \
	for (j = 0; j < i; j++) fprintf(fp, "\t"); \
	fprintf(fp, format, ## args); \
} while(0)

static int generate_igd1_InternetGatewayDevice1_xml(FILE *file, int flag)
{
	//char name[64];
	//char buf[256];
	int i;
	//int rc;

	i = 0;
	/* xml */
	fprintf_indent(i, file, "%s\n", "<?xml version=\"1.0\"?>");

	/* root */
	fprintf_indent(i, file, "%s\n", "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">");
	i++;

	/* <specVersion> */
	fprintf_indent(i, file, "%s\n", "<specVersion>");
	i++;

	fprintf_indent(i, file, "%s\n", "<major>1</major>");
	fprintf_indent(i, file, "%s\n", "<minor>0</minor>");

	/* </specVersion> */
	i--;
	fprintf_indent(i, file, "%s\n", "</specVersion>");

	/* <device> */
	fprintf_indent(i, file, "%s\n", "<device>");
	i++;

	/* <deviceType> */
	fprintf_indent(i, file, "%s\n", "<deviceType>urn:schemas-upnp-org:device:InternetGatewayDevice:1</deviceType>");

	/* <friendlyName> */
	fprintf_indent(i, file, "%s\n", "<friendlyName>ezbox Internet Gateway Device</friendlyName>");

	/* <manufacturer> */
	fprintf_indent(i, file, "%s\n", "<manufacturer>ezbox Project</manufacturer>");

	/* <manufacturerURL> */
	fprintf_indent(i, file, "%s\n", "<manufacturerURL>http://code.google.com/p/ezbox/</manufacturerURL>");

	/* <modelName> */
	fprintf_indent(i, file, "%s\n", "<modelName>ezbox IGD Version 1.00</modelName>");

	/* <UDN> */
	fprintf_indent(i, file, "%s\n", "<UDN>uuid:75802409-bccb-40e7-8e6c-fa095ecce13e</UDN>");

	/* <iconList> */
	fprintf_indent(i, file, "%s\n", "<iconList>");
	i++;

	/* <icon> */
	fprintf_indent(i, file, "%s\n", "<icon>");
	i++;

	/* <mimetype> */
	fprintf_indent(i, file, "%s\n", "<mimetype>image/gif</mimetype>");

	/* <width>118</width> */
	fprintf_indent(i, file, "%s\n", "<width>118</width>");

	/* <height>119</height> */
	fprintf_indent(i, file, "%s\n", "<height>119</height>");

	/* <depth>8</depth> */
	fprintf_indent(i, file, "%s\n", "<depth>8</depth>");

	/* <url>/ligd.gif</url> */
	fprintf_indent(i, file, "%s\n", "<url>/igd.gif</url>");

	/* </icon> */
	i--;
	fprintf_indent(i, file, "%s\n", "</icon>");

	/* </iconList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</iconList>");

	/* <serviceList> */
	fprintf_indent(i, file, "%s\n", "<serviceList>");
	i++;

	/* <service> */
	fprintf_indent(i, file, "%s\n", "<service>");
	i++;

	/* <serviceType> */
	fprintf_indent(i, file, "%s\n", "<serviceType>urn:schemas-upnp-org:service:Layer3Forwarding:1</serviceType>");

	/* <serviceId> */
	fprintf_indent(i, file, "%s\n", "<serviceId>urn:upnp-org:serviceId:Layer3Forwarding1</serviceId>");

	/* <SCPDURL> */
	fprintf_indent(i, file, "%s\n", "<SCPDURL>/service/Layer3Forwarding1.xml</SCPDURL>");

	/* <controlURL> */
	fprintf_indent(i, file, "%s\n", "<controlURL>/control?Layer3Forwarding1</controlURL>");

	/* <eventSubURL> */
	fprintf_indent(i, file, "%s\n", "<eventSubURL>/event?Layer3Forwarding1</eventSubURL>");

	/* </service> */
	i--;
	fprintf_indent(i, file, "%s\n", "</service>");

	/* </serviceList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</serviceList>");

	/* <deviceList> */
	fprintf_indent(i, file, "%s\n", "<deviceList>");
	i++;

	/* <device> */
	fprintf_indent(i, file, "%s\n", "<device>");
	i++;

	/* <deviceType> */
	fprintf_indent(i, file, "%s\n", "<deviceType>urn:schemas-upnp-org:device:WANDevice:1</deviceType>");

	/* <friendlyName> */
	fprintf_indent(i, file, "%s\n", "<friendlyName>WANDevice</friendlyName>");

	/* <manufacturer> */
	fprintf_indent(i, file, "%s\n", "<manufacturer>ezbox Project</manufacturer>");

	/* <manufacturerURL> */
	fprintf_indent(i, file, "%s\n", "<manufacturerURL>http://code.google.com/p/ezbox/</manufacturerURL>");

	/* <modelDescription> */
	fprintf_indent(i, file, "%s\n", "<modelDescription>WAN Device on ezbox IGD</modelDescription>");

	/* <modelName> */
	fprintf_indent(i, file, "%s\n", "<modelName>ezbox IGD</modelName>");

	/* <modelNumber> */
	fprintf_indent(i, file, "%s\n", "<modelNumber>1.00</modelNumber>");

	/* <modelURL> */
	fprintf_indent(i, file, "%s\n", "<modelURL>http://code.google.com/p/ezbox/</modelURL>");

	/* <serialNumber> */
	fprintf_indent(i, file, "%s\n", "<serialNumber>1.00</serialNumber>");

	/* <UDN> */
	fprintf_indent(i, file, "%s\n", "<UDN>uuid:75802409-bccb-40e7-8e6c-fa095ecce13e</UDN>");

	/* <UPC> */
	fprintf_indent(i, file, "%s\n", "<UPC>ezbox IGD</UPC>");

	/* <serviceList> */
	fprintf_indent(i, file, "%s\n", "<serviceList>");
	i++;

	/* <service> */
	fprintf_indent(i, file, "%s\n", "<service>");
	i++;

	/* <serviceType> */
	fprintf_indent(i, file, "%s\n", "<serviceType>urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1</serviceType>");

	/* <serviceId> */
	fprintf_indent(i, file, "%s\n", "<serviceId>urn:upnp-org:serviceId:WANCommonIFC1</serviceId>");

	/* <SCPDURL> */
	fprintf_indent(i, file, "%s\n", "<SCPDURL>/service/WANCommonInterfaceConfig1.xml</SCPDURL>");

	/* <controlURL> */
	fprintf_indent(i, file, "%s\n", "<controlURL>/control?WANCommonIFC1</controlURL>");

	/* <eventSubURL> */
	fprintf_indent(i, file, "%s\n", "<eventSubURL>/event?WANCommonIFC1</eventSubURL>");

	/* </service> */
	i--;
	fprintf_indent(i, file, "%s\n", "</service>");

	/* </serviceList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</serviceList>");

	/* <deviceList> */
	fprintf_indent(i, file, "%s\n", "<deviceList>");
	i++;

	/* <device> */
	fprintf_indent(i, file, "%s\n", "<device>");
	i++;

	/* <deviceType> */
	fprintf_indent(i, file, "%s\n", "<deviceType>urn:schemas-upnp-org:device:WANConnectionDevice:1</deviceType>");

	/* <friendlyName> */
	fprintf_indent(i, file, "%s\n", "<friendlyName>WANConnectionDevice</friendlyName>");

	/* <manufacturer> */
	fprintf_indent(i, file, "%s\n", "<manufacturer>ezbox Project</manufacturer>");

	/* <manufacturerURL> */
	fprintf_indent(i, file, "%s\n", "<manufacturerURL>http://code.google.com/p/ezbox/</manufacturerURL>");

	/* <modelDescription> */
	fprintf_indent(i, file, "%s\n", "<modelDescription>WanConnectionDevice on ezbox IGD</modelDescription>");

	/* <modelName> */
	fprintf_indent(i, file, "%s\n", "<modelDescription>ezbox IGD</modelDescription>");

	/* <modelNumber> */
	fprintf_indent(i, file, "%s\n", "<modelNumber>1.00</modelNumber>");

	/* <modelURL> */
	fprintf_indent(i, file, "%s\n", "<modelURL>http://code.google.com/p/ezbox/</modelURL>");

	/* <serialNumber> */
	fprintf_indent(i, file, "%s\n", "<serialNumber>1.00</serialNumber>");

	/* <UDN> */
	fprintf_indent(i, file, "%s\n", "<UDN>uuid:75802409-bccb-40e7-8e6c-fa095ecce13e</UDN>");

	/* <UPC> */
	fprintf_indent(i, file, "%s\n", "<UPC>ezbox IGD</UPC>");

	/* <serviceList> */
	fprintf_indent(i, file, "%s\n", "<serviceList>");
	i++;

	/* <service> */
	fprintf_indent(i, file, "%s\n", "<service>");
	i++;

	/* <serviceType> */
	fprintf_indent(i, file, "%s\n", "<serviceType>urn:schemas-upnp-org:service:WANIPConnection:1</serviceType>");

	/* <serviceId> */
	fprintf_indent(i, file, "%s\n", "<serviceId>urn:upnp-org:serviceId:WANIPConn1</serviceId>");

	/* <SCPDURL> */
	fprintf_indent(i, file, "%s\n", "<SCPDURL>/service/WANIPConnection1.xml</SCPDURL>");

	/* <controlURL> */
	fprintf_indent(i, file, "%s\n", "<controlURL>/control?WANIPConn1</controlURL>");

	/* <eventSubURL> */
	fprintf_indent(i, file, "%s\n", "<eventSubURL>/event?WANIPConn1</eventSubURL>");

	/* </service> */
	i--;
	fprintf_indent(i, file, "%s\n", "</service>");

	/* </serviceList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</serviceList>");

	/* </device> */
	i--;
	fprintf_indent(i, file, "%s\n", "</device>");

	/* </deviceList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</deviceList>");

	/* </device> */
	i--;
	fprintf_indent(i, file, "%s\n", "</device>");

	/* <device> */
	fprintf_indent(i, file, "%s\n", "<device>");
	i++;

	/* <deviceType> */
	fprintf_indent(i, file, "%s\n", "<deviceType>urn:schemas-upnp-org:device:LANDevice:1</deviceType>");

	/* <friendlyName> */
	fprintf_indent(i, file, "%s\n", "<friendlyName>LANDevice</friendlyName>");

	/* <manufacturer> */
	fprintf_indent(i, file, "%s\n", "<manufacturer>ezbox Project</manufacturer>");

	/* <manufacturerURL> */
	fprintf_indent(i, file, "%s\n", "<manufacturerURL>http://code.google.com/p/ezbox/</manufacturerURL>");

	/* <modelDescription> */
	fprintf_indent(i, file, "%s\n", "<modelDescription>LAN Device on ezbox IGD</modelDescription>");

	/* <modelName> */
	fprintf_indent(i, file, "%s\n", "<modelName>ezbox IGD</modelName>");

	/* <modelNumber> */
	fprintf_indent(i, file, "%s\n", "<modelNumber>1.00</modelNumber>");

	/* <modelURL> */
	fprintf_indent(i, file, "%s\n", "<modelURL>http://code.google.com/p/ezbox/</modelURL>");

	/* <serialNumber> */
	fprintf_indent(i, file, "%s\n", "<serialNumber>1.00</serialNumber>");

	/* <UDN> */
	fprintf_indent(i, file, "%s\n", "<UDN>uuid:00000000-0000-0000-0000-000000000000</UDN>");

	/* <UPC> */
	fprintf_indent(i, file, "%s\n", "<UPC>ezbox IGD</UPC>");

	/* <serviceList> */
	fprintf_indent(i, file, "%s\n", "<serviceList>");
	i++;

	/* <service> */
	fprintf_indent(i, file, "%s\n", "<service>");
	i++;

	/* <serviceType> */
	fprintf_indent(i, file, "%s\n", "<serviceType>urn:schemas-upnp-org:service:LANHostConfigManagement:1</serviceType>");

	/* <serviceId> */
	fprintf_indent(i, file, "%s\n", "<serviceId>urn:upnp-org:serviceId:LANHostCfg1</serviceId>");

	/* <SCPDURL> */
	fprintf_indent(i, file, "%s\n", "<SCPDURL>/service/LANHostConfigManagement1.xml</SCPDURL>");

	/* <controlURL> */
	fprintf_indent(i, file, "%s\n", "<controlURL>/control?LANHostCfg1</controlURL>");

	/* <eventSubURL> */
	fprintf_indent(i, file, "%s\n", "<eventSubURL>/event?LANHostCfg1</eventSubURL>");

	/* </service> */
	i--;
	fprintf_indent(i, file, "%s\n", "</service>");

	/* </serviceList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</serviceList>");

	/* </device> */
	i--;
	fprintf_indent(i, file, "%s\n", "</device>");

	/* </deviceList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</deviceList>");

	/* </device> */
	i--;
	fprintf_indent(i, file, "%s\n", "</device>");

	/* </root> */
	i--;
	fprintf_indent(i, file, "%s\n", "</root>");

	return EXIT_SUCCESS;
}

static int generate_igd1_service_Layer3Forwarding1_xml(FILE *file, int flag)
{
	//char name[64];
	//char buf[256];
	int i;
	//int rc;

	i = 0;
	/* xml */
	fprintf_indent(i, file, "%s\n", "<?xml version=\"1.0\"?>");

	/* <scpd> */
	fprintf_indent(i, file, "%s\n", "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">");
	i++;

	/* <specVersion> */
	fprintf_indent(i, file, "%s\n", "<specVersion>");
	i++;

	fprintf_indent(i, file, "%s\n", "<major>1</major>");
	fprintf_indent(i, file, "%s\n", "<minor>0</minor>");

	/* </specVersion> */
	i--;
	fprintf_indent(i, file, "%s\n", "</specVersion>");

	/* <actionList> */
	fprintf_indent(i, file, "%s\n", "<actionList>");
	i++;

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>SetDefaultConnectionService</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewDefaultConnectionService</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>DefaultConnectionService</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetDefaultConnectionService</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewDefaultConnectionService</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>DefaultConnectionService</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* </actionList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</actionList>");

	/* <serviceStateTable> */
	fprintf_indent(i, file, "%s\n", "<serviceStateTable>");
	i++;

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>DefaultConnectionService</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* </serviceStateTable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</serviceStateTable>");

	/* </scpd> */
	i--;
	fprintf_indent(i, file, "%s\n", "</scpd>");

	return EXIT_SUCCESS;
}

static int generate_igd1_service_WANCommonInterfaceConfig1_xml(FILE *file, int flag)
{
	//char name[64];
	//char buf[256];
	int i;
	//int rc;

	i = 0;
	/* xml */
	fprintf_indent(i, file, "%s\n", "<?xml version=\"1.0\"?>");

	/* <scpd> */
	fprintf_indent(i, file, "%s\n", "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">");
	i++;

	/* <specVersion> */
	fprintf_indent(i, file, "%s\n", "<specVersion>");
	i++;

	fprintf_indent(i, file, "%s\n", "<major>1</major>");
	fprintf_indent(i, file, "%s\n", "<minor>0</minor>");

	/* </specVersion> */
	i--;
	fprintf_indent(i, file, "%s\n", "</specVersion>");

	/* <actionList> */
	fprintf_indent(i, file, "%s\n", "<actionList>");
	i++;

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>SetEnabledForInternet</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewEnabledForInternet</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>EnabledForInternet</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetEnabledForInternet</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewEnabledForInternet</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>EnabledForInternet</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetCommonLinkProperties</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewWANAccessType</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>WANAccessType</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewLayer1UpstreamMaxBitRate</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>Layer1UpstreamMaxBitRate</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewLayer1DownstreamMaxBitRate</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>Layer1DownstreamMaxBitRate</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewPhysicalLinkStatus</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>PhysicalLinkStatus</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetWANAccessProvider</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewWANAccessProvider</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>WANAccessProvider</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetMaximumActiveConnections</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewMaximumActiveConnections</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>MaximumActiveConnections</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetTotalBytesSent</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewTotalBytesSent</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>TotalBytesSent</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetTotalBytesReceived</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewTotalBytesReceived</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>TotalBytesReceived</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetTotalPacketsSent</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewTotalPacketsSent</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>TotalPacketsSent</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetTotalPacketsReceived</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewTotalPacketsReceived</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>TotalPacketsReceived</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetActiveConnection</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewActiveConnectionIndex</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>NumberOfActiveConnections</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewActiveConnDeviceContainer</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>ActiveConnectionDeviceContainer</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewActiveConnectionServiceID</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>ActiveConnectionServiceID</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* </actionList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</actionList>");

	/* <serviceStateTable> */
	fprintf_indent(i, file, "%s\n", "<serviceStateTable>");
	i++;

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>WANAccessType</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* <allowedValueList> */
	fprintf_indent(i, file, "%s\n", "<allowedValueList>");
	i++;

	/* <allowedValue> */
	fprintf_indent(i, file, "%s\n", "<allowedValue>DSL</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, file, "%s\n", "<allowedValue>POTS</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, file, "%s\n", "<allowedValue>Cable</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, file, "%s\n", "<allowedValue>Ethernet</allowedValue>");

	/* </allowedValueList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</allowedValueList>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>Layer1UpstreamMaxBitRate</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>Layer1DownstreamMaxBitRate</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>PhysicalLinkStatus</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* <allowedValueList> */
	fprintf_indent(i, file, "%s\n", "<allowedValueList>");
	i++;

	/* <allowedValue> */
	fprintf_indent(i, file, "%s\n", "<allowedValue>Up</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, file, "%s\n", "<allowedValue>Down</allowedValue>");

	/* </allowedValueList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</allowedValueList>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>WANAccessProvider</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>MaximumActiveConnections</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>ui2</dataType>");

	/* <allowedValueRange> */
	fprintf_indent(i, file, "%s\n", "<allowedValueRange>");
	i++;

	/* <minimum> */
	fprintf_indent(i, file, "%s\n", "<minimum>1</minimum>");

	/* <step> */
	fprintf_indent(i, file, "%s\n", "<step>1</step>");

	/* <allowedValueRange> */
	i--;
	fprintf_indent(i, file, "%s\n", "</allowedValueRange>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NumberOfActiveConnections</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>ui2</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>ActiveConnectionDeviceContainer</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>ActiveConnectionServiceID</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>TotalBytesSent</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>TotalBytesReceived</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>TotalPacketsSent</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>TotalPacketsReceived</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>EnabledForInternet</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>boolean</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* </serviceStateTable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</serviceStateTable>");

	/* </scpd> */
	i--;
	fprintf_indent(i, file, "%s\n", "</scpd>");

	return EXIT_SUCCESS;
}

static int generate_igd1_service_WANIPConnection1_xml(FILE *file, int flag)
{
	//char name[64];
	//char buf[256];
	int i;
	//int rc;

	i = 0;
	/* xml */
	fprintf_indent(i, file, "%s\n", "<?xml version=\"1.0\"?>");

	/* <scpd> */
	fprintf_indent(i, file, "%s\n", "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">");
	i++;

	/* <specVersion> */
	fprintf_indent(i, file, "%s\n", "<specVersion>");
	i++;

	fprintf_indent(i, file, "%s\n", "<major>1</major>");
	fprintf_indent(i, file, "%s\n", "<minor>0</minor>");

	/* </specVersion> */
	i--;
	fprintf_indent(i, file, "%s\n", "</specVersion>");

	/* <actionList> */
	fprintf_indent(i, file, "%s\n", "<actionList>");
	i++;

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>SetConnectionType</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewConnectionType</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>ConnectionType</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetConnectionTypeInfo</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewConnectionType</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>ConnectionType</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewPossibleConnectionTypes</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>PossibleConnectionTypes</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>RequestConnection</name>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>RequestTermination</name>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>ForceTermination</name>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>SetAutoDisconnectTime</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewAutoDisconnectTime</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>AutoDisconnectTime</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>SetIdleDisconnectTime</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewIdleDisconnectTime</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>IdleDisconnectTime</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>SetWarnDisconnectDelay</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewWarnDisconnectDelay</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>WarnDisconnectDelay</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetStatusInfo</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewConnectionStatus</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>ConnectionStatus</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewLastConnectionError</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>LastConnectionError</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewUptime</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>Uptime</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetAutoDisconnectTime</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewAutoDisconnectTime</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>AutoDisconnectTime</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetIdleDisconnectTime</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewIdleDisconnectTime</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>IdleDisconnectTime</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetWarnDisconnectDelay</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewWarnDisconnectDelay</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>WarnDisconnectDelay</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetNATRSIPStatus</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewRSIPAvailable</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>RSIPAvailable</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewNATEnabled</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>NATEnabled</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetGenericPortMappingEntry</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewPortMappingIndex</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>PortMappingNumberOfEntries</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewRemoteHost</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>RemoteHost</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewExternalPort</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>ExternalPort</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewProtocol</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>PortMappingProtocol</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewInternalPort</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>InternalPort</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewInternalClient</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>InternalClient</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewEnabled</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>PortMappingEnabled</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewPortMappingDescription</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>PortMappingDescription</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewLeaseDuration</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>PortMappingLeaseDuration</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetSpecificPortMappingEntry</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewRemoteHost</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>RemoteHost</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewExternalPort</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>ExternalPort</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewProtocol</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>PortMappingProtocol</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewInternalPort</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>InternalPort</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewInternalClient</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>InternalClient</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewEnabled</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>PortMappingEnabled</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewPortMappingDescription</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>PortMappingDescription</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewLeaseDuration</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>PortMappingLeaseDuration</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>AddPortMapping</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewRemoteHost</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>RemoteHost</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewExternalPort</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>ExternalPort</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewProtocol</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>PortMappingProtocol</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewInternalPort</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>InternalPort</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewInternalClient</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>InternalClient</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewEnabled</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>PortMappingEnabled</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewPortMappingDescription</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>PortMappingDescription</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewLeaseDuration</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>PortMappingLeaseDuration</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>DeletePortMapping</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewRemoteHost</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>RemoteHost</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewExternalPort</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>ExternalPort</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewProtocol</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>PortMappingProtocol</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetExternalIPAddress</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewExternalIPAddress</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>ExternalIPAddress</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* </actionList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</actionList>");



	/* <serviceStateTable> */
	fprintf_indent(i, file, "%s\n", "<serviceStateTable>");
	i++;

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>ConnectionType</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>PossibleConnectionTypes</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* <allowedValueList> */
	fprintf_indent(i, file, "%s\n", "<allowedValueList>");
	i++;

	/* <allowedValue> */
	fprintf_indent(i, file, "%s\n", "<allowedValue>Unconfigured</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, file, "%s\n", "<allowedValue>IP_Routed</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, file, "%s\n", "<allowedValue>IP_Bridged</allowedValue>");

	/* </allowedValueList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</allowedValueList>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>ConnectionStatus</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* <allowedValueList> */
	fprintf_indent(i, file, "%s\n", "<allowedValueList>");
	i++;

	/* <allowedValue> */
	fprintf_indent(i, file, "%s\n", "<allowedValue>Unconfigured</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, file, "%s\n", "<allowedValue>Connected</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, file, "%s\n", "<allowedValue>Disconnected</allowedValue>");

	/* </allowedValueList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</allowedValueList>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>Uptime</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>LastConnectionError</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* <allowedValueList> */
	fprintf_indent(i, file, "%s\n", "<allowedValueList>");
	i++;

	/* <allowedValue> */
	fprintf_indent(i, file, "%s\n", "<allowedValue>ERROR_NONE</allowedValue>");

	/* </allowedValueList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</allowedValueList>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>AutoDisconnectTime</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>IdleDisconnectTime</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>WarnDisconnectDelay</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>RSIPAvailable</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>boolean</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NATEnabled</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>boolean</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>ExternalIPAddress</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>PortMappingNumberOfEntries</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>ui2</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>PortMappingEnabled</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>boolean</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>PortMappingLeaseDuration</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>RemoteHost</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>ExternalPort</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>ui2</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>InternalPort</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>ui2</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>PortMappingProtocol</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* <allowedValueList> */
	fprintf_indent(i, file, "%s\n", "<allowedValueList>");
	i++;

	/* <allowedValue> */
	fprintf_indent(i, file, "%s\n", "<allowedValue>TCP</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, file, "%s\n", "<allowedValue>UDP</allowedValue>");

	/* </allowedValueList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</allowedValueList>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>InternalClient</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>PortMappingDescription</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* </serviceStateTable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</serviceStateTable>");

	/* </scpd> */
	i--;
	fprintf_indent(i, file, "%s\n", "</scpd>");

	return EXIT_SUCCESS;
}

static int generate_igd1_service_LANHostConfigManagement1_xml(FILE *file, int flag)
{
	//char name[64];
	//char buf[256];
	int i;
	//int rc;

	i = 0;
	/* xml */
	fprintf_indent(i, file, "%s\n", "<?xml version=\"1.0\"?>");

	/* <scpd> */
	fprintf_indent(i, file, "%s\n", "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">");
	i++;

	/* <specVersion> */
	fprintf_indent(i, file, "%s\n", "<specVersion>");
	i++;

	fprintf_indent(i, file, "%s\n", "<major>1</major>");
	fprintf_indent(i, file, "%s\n", "<minor>0</minor>");

	/* </specVersion> */
	i--;
	fprintf_indent(i, file, "%s\n", "</specVersion>");

	/* <actionList> */
	fprintf_indent(i, file, "%s\n", "<actionList>");
	i++;

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>SetDHCPServerConfigurable</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewDHCPServerConfigurable</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>DHCPServerConfigurable</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetDHCPServerConfigurable</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewDHCPServerConfigurable</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>DHCPServerConfigurable</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>SetDHCPRelay</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewDHCPRelay</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>DHCPRelay</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetDHCPRelay</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewDHCPRelay</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>DHCPRelay</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>SetSubnetMask</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewSubnetMask</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>SubnetMask</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetSubnetMask</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewSubnetMask</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>SubnetMask</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>SetIPRouter</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewIPRouters</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>IPRouters</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>DeleteIPRouter</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewIPRouters</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>IPRouters</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, file, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>GetIPRoutersList</name>");

	/* <argumentList> */
	fprintf_indent(i, file, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, file, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>NewIPRouters</name>");

	/* <relatedStateVariable> */
	fprintf_indent(i, file, "%s\n", "<relatedStateVariable>IPRouters</relatedStateVariable>");

	/* <direction> */
	fprintf_indent(i, file, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, file, "%s\n", "</action>");

	/* </actionList> */
	i--;
	fprintf_indent(i, file, "%s\n", "</actionList>");

	/* <serviceStateTable> */
	fprintf_indent(i, file, "%s\n", "<serviceStateTable>");
	i++;

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>DHCPServerConfigurable</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>boolean</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>DHCPRelay</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>boolean</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>SubnetMask</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>IPRouters</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>DNSServers</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>DomainName</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>MinAddress</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>MaxAddress</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, file, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, file, "%s\n", "<name>ReservedAddresses</name>");

	/* <dataType> */
	fprintf_indent(i, file, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</stateVariable>");

	/* </serviceStateTable> */
	i--;
	fprintf_indent(i, file, "%s\n", "</serviceStateTable>");

	/* </scpd> */
	i--;
	fprintf_indent(i, file, "%s\n", "</scpd>");

	return EXIT_SUCCESS;
}

static int gen_igd1_files(int flag)
{
	FILE *file;
	int ret;

	mkdir("/etc/ezcfg_upnpd/igd1", 0755);
	mkdir("/etc/ezcfg_upnpd/igd1/device", 0755);
	mkdir("/etc/ezcfg_upnpd/igd1/service", 0755);

	file = fopen("/etc/ezcfg_upnpd/igd1/InternetGatewayDevice1.xml", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	ret = generate_igd1_InternetGatewayDevice1_xml(file, flag);
	fclose(file);
	if (ret == EXIT_FAILURE)
		return ret;

	file = fopen("/etc/ezcfg_upnpd/igd1/service/Layer3Forwarding1.xml", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	ret = generate_igd1_service_Layer3Forwarding1_xml(file, flag);
	fclose(file);
	if (ret == EXIT_FAILURE)
		return ret;

	file = fopen("/etc/ezcfg_upnpd/igd1/service/WANCommonInterfaceConfig1.xml", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	ret = generate_igd1_service_WANCommonInterfaceConfig1_xml(file, flag);
	fclose(file);
	if (ret == EXIT_FAILURE)
		return ret;

	file = fopen("/etc/ezcfg_upnpd/igd1/service/WANIPConnection1.xml", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	ret = generate_igd1_service_WANIPConnection1_xml(file, flag);
	fclose(file);
	if (ret == EXIT_FAILURE)
		return ret;

	file = fopen("/etc/ezcfg_upnpd/igd1/service/LANHostConfigManagement1.xml", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	ret = generate_igd1_service_LANHostConfigManagement1_xml(file, flag);
	fclose(file);
	if (ret == EXIT_FAILURE)
		return ret;

	return EXIT_SUCCESS;
}

int pop_etc_ezcfg_upnpd(int flag)
{

	switch (flag) {
	case RC_ACT_BOOT :
	case RC_ACT_START :
	case RC_ACT_RELOAD :
	case RC_ACT_RESTART :
		mkdir("/etc/ezcfg_upnpd", 0755);
		/* IGD root device */
		if (gen_igd1_files(flag) == EXIT_FAILURE)
			return (EXIT_FAILURE);
		break;
	}

	return (EXIT_SUCCESS);
}
