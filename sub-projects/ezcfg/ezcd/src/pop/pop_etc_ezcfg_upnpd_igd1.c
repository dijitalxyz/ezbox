/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_ezcfg_upnpd_igd1.c
 *
 * Description  : ezbox /etc/ezcfg_upnpd/igd1/ files generating program
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

//#define IGD_XML_PATH	"/etc/ezcfg_upnpd/igd1/InternetGatewayDevice1.xml"
#define IGD_XML_PATH	EZCFG_UPNP_IGD1_ROOT_DEVICE_XML_FILE
#define L3F_XML_PATH	EZCFG_UPNP_IGD1_CONF_PATH "/service/Layer3Forwarding1.xml"
#define WCIC_XML_PATH	EZCFG_UPNP_IGD1_CONF_PATH "/service/WANCommonInterfaceConfig1.xml"
#define WCLC_XML_PATH	EZCFG_UPNP_IGD1_CONF_PATH "/service/WANCableLinkConfig1.xml"
#define WDLC_XML_PATH	EZCFG_UPNP_IGD1_CONF_PATH "/service/WANDSLLinkConfig1.xml"
#define WELC_XML_PATH	EZCFG_UPNP_IGD1_CONF_PATH "/service/WANEthernetLinkConfig1.xml"
#define WPLC_XML_PATH	EZCFG_UPNP_IGD1_CONF_PATH "/service/WANPOTSLinkConfig1.xml"
#define WPC_XML_PATH	EZCFG_UPNP_IGD1_CONF_PATH "/service/WANPPPConnection1.xml"
#define WIC_XML_PATH	EZCFG_UPNP_IGD1_CONF_PATH "/service/WANIPConnection1.xml"
#define LHCM_XML_PATH	EZCFG_UPNP_IGD1_CONF_PATH "/service/LANHostConfigManagement1.xml"

static int gen_igd1_InternetGatewayDevice1_xml(const char *path, int flag)
{
	int i;
	FILE *fp;

	fp = fopen(path, "w");
	if (fp == NULL)
		return (EXIT_FAILURE);

	i = 0;
	/* xml */
	fprintf_indent(i, fp, "%s\n", "<?xml version=\"1.0\"?>");

	/* root */
	fprintf_indent(i, fp, "%s\n", "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">");
	i++;

	/* <specVersion> */
	fprintf_indent(i, fp, "%s\n", "<specVersion>");
	i++;

	fprintf_indent(i, fp, "%s\n", "<major>1</major>");
	fprintf_indent(i, fp, "%s\n", "<minor>0</minor>");

	/* </specVersion> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</specVersion>");

	/* <device> */
	fprintf_indent(i, fp, "%s\n", "<device>");
	i++;

	/* <deviceType> */
	fprintf_indent(i, fp, "%s\n", "<deviceType>urn:schemas-upnp-org:device:InternetGatewayDevice:1</deviceType>");

	/* <friendlyName> */
	fprintf_indent(i, fp, "%s\n", "<friendlyName>ezbox Internet Gateway Device</friendlyName>");

	/* <manufacturer> */
	fprintf_indent(i, fp, "%s\n", "<manufacturer>ezbox Project</manufacturer>");

	/* <manufacturerURL> */
	fprintf_indent(i, fp, "%s\n", "<manufacturerURL>http://code.google.com/p/ezbox/</manufacturerURL>");

	/* <modelName> */
	fprintf_indent(i, fp, "%s\n", "<modelName>ezbox IGD Version 1.00</modelName>");

	/* <UDN> */
	fprintf_indent(i, fp, "%s\n", "<UDN>uuid:75802409-bccb-40e7-8e6c-fa095ecce13e</UDN>");

	/* <iconList> */
	fprintf_indent(i, fp, "%s\n", "<iconList>");
	i++;

	/* <icon> */
	fprintf_indent(i, fp, "%s\n", "<icon>");
	i++;

	/* <mimetype> */
	fprintf_indent(i, fp, "%s\n", "<mimetype>image/gif</mimetype>");
	/* <width>118</width> */
	fprintf_indent(i, fp, "%s\n", "<width>118</width>");
	/* <height>119</height> */
	fprintf_indent(i, fp, "%s\n", "<height>119</height>");
	/* <depth>8</depth> */
	fprintf_indent(i, fp, "%s\n", "<depth>8</depth>");
	/* <url>/igd.gif</url> */
	fprintf_indent(i, fp, "%s\n", "<url>/igd.gif</url>");

	/* </icon> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</icon>");

	/* </iconList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</iconList>");

	/* <serviceList> */
	fprintf_indent(i, fp, "%s\n", "<serviceList>");
	i++;

	/* <service> */
	fprintf_indent(i, fp, "%s\n", "<service>");
	i++;

	/* <serviceType> */
	fprintf_indent(i, fp, "%s\n", "<serviceType>urn:schemas-upnp-org:service:Layer3Forwarding:1</serviceType>");
	/* <serviceId> */
	fprintf_indent(i, fp, "%s\n", "<serviceId>urn:upnp-org:serviceId:Layer3Forwarding1</serviceId>");
	/* <SCPDURL> */
	fprintf_indent(i, fp, "%s\n", "<SCPDURL>/service/Layer3Forwarding1.xml</SCPDURL>");
	/* <controlURL> */
	fprintf_indent(i, fp, "%s\n", "<controlURL>/control?Layer3Forwarding1</controlURL>");
	/* <eventSubURL> */
	fprintf_indent(i, fp, "%s\n", "<eventSubURL>/event?Layer3Forwarding1</eventSubURL>");

	/* </service> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</service>");

	/* </serviceList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</serviceList>");

	/* <deviceList> */
	fprintf_indent(i, fp, "%s\n", "<deviceList>");
	i++;

	/* <device> */
	fprintf_indent(i, fp, "%s\n", "<device>");
	i++;

	/* <deviceType> */
	fprintf_indent(i, fp, "%s\n", "<deviceType>urn:schemas-upnp-org:device:WANDevice:1</deviceType>");
	/* <friendlyName> */
	fprintf_indent(i, fp, "%s\n", "<friendlyName>WANDevice</friendlyName>");
	/* <manufacturer> */
	fprintf_indent(i, fp, "%s\n", "<manufacturer>ezbox Project</manufacturer>");
	/* <manufacturerURL> */
	fprintf_indent(i, fp, "%s\n", "<manufacturerURL>http://code.google.com/p/ezbox/</manufacturerURL>");
	/* <modelDescription> */
	fprintf_indent(i, fp, "%s\n", "<modelDescription>WAN Device on ezbox IGD</modelDescription>");
	/* <modelName> */
	fprintf_indent(i, fp, "%s\n", "<modelName>ezbox IGD</modelName>");
	/* <modelNumber> */
	fprintf_indent(i, fp, "%s\n", "<modelNumber>1.00</modelNumber>");
	/* <modelURL> */
	fprintf_indent(i, fp, "%s\n", "<modelURL>http://code.google.com/p/ezbox/</modelURL>");
	/* <serialNumber> */
	fprintf_indent(i, fp, "%s\n", "<serialNumber>1.00</serialNumber>");
	/* <UDN> */
	fprintf_indent(i, fp, "%s\n", "<UDN>uuid:75802409-bccb-40e7-8e6c-fa095ecce13e</UDN>");
	/* <UPC> */
	fprintf_indent(i, fp, "%s\n", "<UPC>ezbox IGD</UPC>");

	/* <serviceList> */
	fprintf_indent(i, fp, "%s\n", "<serviceList>");
	i++;

	/* <service> */
	fprintf_indent(i, fp, "%s\n", "<service>");
	i++;

	/* <serviceType> */
	fprintf_indent(i, fp, "%s\n", "<serviceType>urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1</serviceType>");
	/* <serviceId> */
	fprintf_indent(i, fp, "%s\n", "<serviceId>urn:upnp-org:serviceId:WANCommonIFC1</serviceId>");
	/* <SCPDURL> */
	fprintf_indent(i, fp, "%s\n", "<SCPDURL>/service/WANCommonInterfaceConfig1.xml</SCPDURL>");
	/* <controlURL> */
	fprintf_indent(i, fp, "%s\n", "<controlURL>/control?WANCommonIFC1</controlURL>");
	/* <eventSubURL> */
	fprintf_indent(i, fp, "%s\n", "<eventSubURL>/event?WANCommonIFC1</eventSubURL>");

	/* </service> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</service>");

	/* </serviceList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</serviceList>");

	/* <deviceList> */
	fprintf_indent(i, fp, "%s\n", "<deviceList>");
	i++;

	/* <device> */
	fprintf_indent(i, fp, "%s\n", "<device>");
	i++;

	/* <deviceType> */
	fprintf_indent(i, fp, "%s\n", "<deviceType>urn:schemas-upnp-org:device:WANConnectionDevice:1</deviceType>");
	/* <friendlyName> */
	fprintf_indent(i, fp, "%s\n", "<friendlyName>WANConnectionDevice</friendlyName>");
	/* <manufacturer> */
	fprintf_indent(i, fp, "%s\n", "<manufacturer>ezbox Project</manufacturer>");
	/* <manufacturerURL> */
	fprintf_indent(i, fp, "%s\n", "<manufacturerURL>http://code.google.com/p/ezbox/</manufacturerURL>");
	/* <modelDescription> */
	fprintf_indent(i, fp, "%s\n", "<modelDescription>WanConnectionDevice on ezbox IGD</modelDescription>");
	/* <modelName> */
	fprintf_indent(i, fp, "%s\n", "<modelName>ezbox IGD</modelName>");
	/* <modelNumber> */
	fprintf_indent(i, fp, "%s\n", "<modelNumber>1.00</modelNumber>");
	/* <modelURL> */
	fprintf_indent(i, fp, "%s\n", "<modelURL>http://code.google.com/p/ezbox/</modelURL>");
	/* <serialNumber> */
	fprintf_indent(i, fp, "%s\n", "<serialNumber>1.00</serialNumber>");
	/* <UDN> */
	fprintf_indent(i, fp, "%s\n", "<UDN>uuid:75802409-bccb-40e7-8e6c-fa095ecce13e</UDN>");
	/* <UPC> */
	fprintf_indent(i, fp, "%s\n", "<UPC>ezbox IGD</UPC>");

	/* <serviceList> */
	fprintf_indent(i, fp, "%s\n", "<serviceList>");
	i++;

	/* <service> */
	fprintf_indent(i, fp, "%s\n", "<service>");
	i++;

	/* <serviceType> */
	fprintf_indent(i, fp, "%s\n", "<serviceType>urn:schemas-upnp-org:service:WANEthernetLinkConfig:1</serviceType>");
	/* <serviceId> */
	fprintf_indent(i, fp, "%s\n", "<serviceId>urn:upnp-org:serviceId:WANEthernetLinkCfg1</serviceId>");
	/* <SCPDURL> */
	fprintf_indent(i, fp, "%s\n", "<SCPDURL>/service/WANEthernetLinkConfig1.xml</SCPDURL>");
	/* <controlURL> */
	fprintf_indent(i, fp, "%s\n", "<controlURL>/control?WANEthernetLinkCfg1</controlURL>");
	/* <eventSubURL> */
	fprintf_indent(i, fp, "%s\n", "<eventSubURL>/event?WANEthernetLinkCfg1</eventSubURL>");

	/* </service> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</service>");

	/* <service> */
	fprintf_indent(i, fp, "%s\n", "<service>");
	i++;

	/* <serviceType> */
	fprintf_indent(i, fp, "%s\n", "<serviceType>urn:schemas-upnp-org:service:WANPPPConnection:1</serviceType>");
	/* <serviceId> */
	fprintf_indent(i, fp, "%s\n", "<serviceId>urn:upnp-org:serviceId:WANPPPConn1</serviceId>");
	/* <SCPDURL> */
	fprintf_indent(i, fp, "%s\n", "<SCPDURL>/service/WANPPPConnection1.xml</SCPDURL>");
	/* <controlURL> */
	fprintf_indent(i, fp, "%s\n", "<controlURL>/control?WANPPPConn1</controlURL>");
	/* <eventSubURL> */
	fprintf_indent(i, fp, "%s\n", "<eventSubURL>/event?WANPPPConn1</eventSubURL>");

	/* </service> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</service>");

	/* <service> */
	fprintf_indent(i, fp, "%s\n", "<service>");
	i++;

	/* <serviceType> */
	fprintf_indent(i, fp, "%s\n", "<serviceType>urn:schemas-upnp-org:service:WANIPConnection:1</serviceType>");
	/* <serviceId> */
	fprintf_indent(i, fp, "%s\n", "<serviceId>urn:upnp-org:serviceId:WANIPConn1</serviceId>");
	/* <SCPDURL> */
	fprintf_indent(i, fp, "%s\n", "<SCPDURL>/service/WANIPConnection1.xml</SCPDURL>");
	/* <controlURL> */
	fprintf_indent(i, fp, "%s\n", "<controlURL>/control?WANIPConn1</controlURL>");
	/* <eventSubURL> */
	fprintf_indent(i, fp, "%s\n", "<eventSubURL>/event?WANIPConn1</eventSubURL>");

	/* </service> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</service>");

	/* </serviceList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</serviceList>");

	/* </device> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</device>");

	/* </deviceList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</deviceList>");

	/* </device> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</device>");

	/* <device> */
	fprintf_indent(i, fp, "%s\n", "<device>");
	i++;

	/* <deviceType> */
	fprintf_indent(i, fp, "%s\n", "<deviceType>urn:schemas-upnp-org:device:LANDevice:1</deviceType>");
	/* <friendlyName> */
	fprintf_indent(i, fp, "%s\n", "<friendlyName>LANDevice</friendlyName>");
	/* <manufacturer> */
	fprintf_indent(i, fp, "%s\n", "<manufacturer>ezbox Project</manufacturer>");
	/* <manufacturerURL> */
	fprintf_indent(i, fp, "%s\n", "<manufacturerURL>http://code.google.com/p/ezbox/</manufacturerURL>");
	/* <modelDescription> */
	fprintf_indent(i, fp, "%s\n", "<modelDescription>LAN Device on ezbox IGD</modelDescription>");
	/* <modelName> */
	fprintf_indent(i, fp, "%s\n", "<modelName>ezbox IGD</modelName>");
	/* <modelNumber> */
	fprintf_indent(i, fp, "%s\n", "<modelNumber>1.00</modelNumber>");
	/* <modelURL> */
	fprintf_indent(i, fp, "%s\n", "<modelURL>http://code.google.com/p/ezbox/</modelURL>");
	/* <serialNumber> */
	fprintf_indent(i, fp, "%s\n", "<serialNumber>1.00</serialNumber>");
	/* <UDN> */
	fprintf_indent(i, fp, "%s\n", "<UDN>uuid:00000000-0000-0000-0000-000000000000</UDN>");
	/* <UPC> */
	fprintf_indent(i, fp, "%s\n", "<UPC>ezbox IGD</UPC>");

	/* <serviceList> */
	fprintf_indent(i, fp, "%s\n", "<serviceList>");
	i++;

	/* <service> */
	fprintf_indent(i, fp, "%s\n", "<service>");
	i++;

	/* <serviceType> */
	fprintf_indent(i, fp, "%s\n", "<serviceType>urn:schemas-upnp-org:service:LANHostConfigManagement:1</serviceType>");
	/* <serviceId> */
	fprintf_indent(i, fp, "%s\n", "<serviceId>urn:upnp-org:serviceId:LANHostCfg1</serviceId>");
	/* <SCPDURL> */
	fprintf_indent(i, fp, "%s\n", "<SCPDURL>/service/LANHostConfigManagement1.xml</SCPDURL>");
	/* <controlURL> */
	fprintf_indent(i, fp, "%s\n", "<controlURL>/control?LANHostCfg1</controlURL>");
	/* <eventSubURL> */
	fprintf_indent(i, fp, "%s\n", "<eventSubURL>/event?LANHostCfg1</eventSubURL>");

	/* </service> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</service>");

	/* </serviceList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</serviceList>");

	/* </device> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</device>");

	/* </deviceList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</deviceList>");

	/* </device> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</device>");

	/* </root> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</root>");

	fclose(fp);

	return (EXIT_SUCCESS);
}

static int gen_igd1_service_Layer3Forwarding1_xml(const char *path, int flag)
{
	FILE *fp;
	int i;

	fp = fopen(path, "w");
	if (fp == NULL)
		return (EXIT_FAILURE);

	i = 0;
	/* xml */
	fprintf_indent(i, fp, "%s\n", "<?xml version=\"1.0\"?>");

	/* <scpd> */
	fprintf_indent(i, fp, "%s\n", "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">");
	i++;

	/* <specVersion> */
	fprintf_indent(i, fp, "%s\n", "<specVersion>");
	i++;

	fprintf_indent(i, fp, "%s\n", "<major>1</major>");
	fprintf_indent(i, fp, "%s\n", "<minor>0</minor>");

	/* </specVersion> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</specVersion>");

	/* <actionList> */
	fprintf_indent(i, fp, "%s\n", "<actionList>");
	i++;

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SetDefaultConnectionService</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewDefaultConnectionService</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>DefaultConnectionService</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetDefaultConnectionService</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewDefaultConnectionService</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>DefaultConnectionService</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* </actionList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</actionList>");

	/* <serviceStateTable> */
	fprintf_indent(i, fp, "%s\n", "<serviceStateTable>");
	i++;

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>DefaultConnectionService</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* </serviceStateTable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</serviceStateTable>");

	/* </scpd> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</scpd>");

	fclose(fp);

	return (EXIT_SUCCESS);
}

static int gen_igd1_service_WANCommonInterfaceConfig1_xml(const char *path, int flag)
{
	FILE *fp;
	int i;

	fp = fopen(path, "w");
	if (fp == NULL)
		return (EXIT_FAILURE);

	i = 0;
	/* xml */
	fprintf_indent(i, fp, "%s\n", "<?xml version=\"1.0\"?>");

	/* <scpd> */
	fprintf_indent(i, fp, "%s\n", "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">");
	i++;

	/* <specVersion> */
	fprintf_indent(i, fp, "%s\n", "<specVersion>");
	i++;

	fprintf_indent(i, fp, "%s\n", "<major>1</major>");
	fprintf_indent(i, fp, "%s\n", "<minor>0</minor>");

	/* </specVersion> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</specVersion>");

	/* <actionList> */
	fprintf_indent(i, fp, "%s\n", "<actionList>");
	i++;

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SetEnabledForInternet</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewEnabledForInternet</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>EnabledForInternet</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetEnabledForInternet</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewEnabledForInternet</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>EnabledForInternet</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetCommonLinkProperties</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewWANAccessType</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>WANAccessType</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewLayer1UpstreamMaxBitRate</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>Layer1UpstreamMaxBitRate</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewLayer1DownstreamMaxBitRate</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>Layer1DownstreamMaxBitRate</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewPhysicalLinkStatus</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PhysicalLinkStatus</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetWANAccessProvider</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewWANAccessProvider</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>WANAccessProvider</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetMaximumActiveConnections</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewMaximumActiveConnections</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>MaximumActiveConnections</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetTotalBytesSent</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewTotalBytesSent</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>TotalBytesSent</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetTotalBytesReceived</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewTotalBytesReceived</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>TotalBytesReceived</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetTotalPacketsSent</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewTotalPacketsSent</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>TotalPacketsSent</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetTotalPacketsReceived</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewTotalPacketsReceived</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>TotalPacketsReceived</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetActiveConnection</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewActiveConnectionIndex</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>NumberOfActiveConnections</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewActiveConnDeviceContainer</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ActiveConnectionDeviceContainer</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewActiveConnectionServiceID</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ActiveConnectionServiceID</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* </actionList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</actionList>");

	/* <serviceStateTable> */
	fprintf_indent(i, fp, "%s\n", "<serviceStateTable>");
	i++;

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>WANAccessType</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* <allowedValueList> */
	fprintf_indent(i, fp, "%s\n", "<allowedValueList>");
	i++;

	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>DSL</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>POTS</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>Cable</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>Ethernet</allowedValue>");

	/* </allowedValueList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</allowedValueList>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>Layer1UpstreamMaxBitRate</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>Layer1DownstreamMaxBitRate</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>PhysicalLinkStatus</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* <allowedValueList> */
	fprintf_indent(i, fp, "%s\n", "<allowedValueList>");
	i++;

	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>Up</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>Down</allowedValue>");

	/* </allowedValueList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</allowedValueList>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>WANAccessProvider</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>MaximumActiveConnections</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui2</dataType>");

	/* <allowedValueRange> */
	fprintf_indent(i, fp, "%s\n", "<allowedValueRange>");
	i++;

	/* <minimum> */
	fprintf_indent(i, fp, "%s\n", "<minimum>1</minimum>");
	/* <step> */
	fprintf_indent(i, fp, "%s\n", "<step>1</step>");

	/* <allowedValueRange> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</allowedValueRange>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NumberOfActiveConnections</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui2</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>ActiveConnectionDeviceContainer</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>ActiveConnectionServiceID</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>TotalBytesSent</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>TotalBytesReceived</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>TotalPacketsSent</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>TotalPacketsReceived</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>EnabledForInternet</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>boolean</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* </serviceStateTable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</serviceStateTable>");

	/* </scpd> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</scpd>");

	fclose(fp);

	return (EXIT_SUCCESS);
}

static int gen_igd1_service_WANEthernetLinkConfig1_xml(const char *path, int flag)
{
	FILE *fp;
	int i;

	fp = fopen(path, "w");
	if (fp == NULL)
		return (EXIT_FAILURE);

	i = 0;
	/* xml */
	fprintf_indent(i, fp, "%s\n", "<?xml version=\"1.0\"?>");

	/* <scpd> */
	fprintf_indent(i, fp, "%s\n", "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">");
	i++;

	/* <specVersion> */
	fprintf_indent(i, fp, "%s\n", "<specVersion>");
	i++;

	fprintf_indent(i, fp, "%s\n", "<major>1</major>");
	fprintf_indent(i, fp, "%s\n", "<minor>0</minor>");

	/* </specVersion> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</specVersion>");

	/* <actionList> */
	fprintf_indent(i, fp, "%s\n", "<actionList>");
	i++;

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetEthernetLinkStatus</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewEthernetLinkStatus</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>EthernetLinkStatus</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* </actionList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</actionList>");

	/* <serviceStateTable> */
	fprintf_indent(i, fp, "%s\n", "<serviceStateTable>");
	i++;

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>EthernetLinkStatus</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* <allowedValueList> */
	fprintf_indent(i, fp, "%s\n", "<allowedValueList>");
	i++;

	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>Up</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>Down</allowedValue>");

	/* </allowedValueList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</allowedValueList>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* </serviceStateTable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</serviceStateTable>");

	/* </scpd> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</scpd>");

	fclose(fp);

	return (EXIT_SUCCESS);
}

static int gen_igd1_service_WANPPPConnection1_xml(const char *path, int flag)
{
	FILE *fp;
	int i;

	fp = fopen(path, "w");
	if (fp == NULL)
		return (EXIT_FAILURE);

	i = 0;
	/* xml */
	fprintf_indent(i, fp, "%s\n", "<?xml version=\"1.0\"?>");

	/* <scpd> */
	fprintf_indent(i, fp, "%s\n", "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">");
	i++;

	/* <specVersion> */
	fprintf_indent(i, fp, "%s\n", "<specVersion>");
	i++;

	fprintf_indent(i, fp, "%s\n", "<major>1</major>");
	fprintf_indent(i, fp, "%s\n", "<minor>0</minor>");

	/* </specVersion> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</specVersion>");

	/* <actionList> */
	fprintf_indent(i, fp, "%s\n", "<actionList>");
	i++;

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SetConnectionType</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewConnectionType</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ConnectionType</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetConnectionTypeInfo</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewConnectionType</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ConnectionType</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewPossibleConnectionTypes</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PossibleConnectionTypes</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>ConfigureConnection</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewUserName</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>UserName</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewPassword</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>Password</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>RequestConnection</name>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>RequestTermination</name>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>ForceTermination</name>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SetAutoDisconnectTime</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewAutoDisconnectTime</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>AutoDisconnectTime</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SetIdleDisconnectTime</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewIdleDisconnectTime</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>IdleDisconnectTime</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SetWarnDisconnectDelay</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewWarnDisconnectDelay</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>WarnDisconnectDelay</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetStatusInfo</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewConnectionStatus</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ConnectionStatus</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewLastConnectionError</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>LastConnectionError</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewUptime</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>Uptime</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetLinkLayerMaxBitRates</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewUpstreamMaxBitRate</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>UpstreamMaxBitRate</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewDownstreamMaxBitRate</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>DownstreamMaxBitRate</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetPPPEncryptionProtocol</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewPPPEncryptionProtocol</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PPPEncryptionProtocol</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetPPPCompressionProtocol</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewPPPCompressionProtocol</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PPPCompressionProtocol</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetPPPAuthenticationProtocol</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewPPPAuthenticationProtocol</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PPPAuthenticationProtocol</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetUserName</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewUserName</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>UserName</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetPassword</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewPassword</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>Password</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetAutoDisconnectTime</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewAutoDisconnectTime</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>AutoDisconnectTime</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetIdleDisconnectTime</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewIdleDisconnectTime</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>IdleDisconnectTime</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetWarnDisconnectDelay</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewWarnDisconnectDelay</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>WarnDisconnectDelay</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetNATRSIPStatus</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewRSIPAvailable</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>RSIPAvailable</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewNATEnabled</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>NATEnabled</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetGenericPortMappingEntry</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewPortMappingIndex</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingNumberOfEntries</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewRemoteHost</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>RemoteHost</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewExternalPort</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ExternalPort</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewProtocol</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingProtocol</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewInternalPort</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>InternalPort</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewInternalClient</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>InternalClient</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewEnabled</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingEnabled</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewPortMappingDescription</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingDescription</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewLeaseDuration</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingLeaseDuration</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetSpecificPortMappingEntry</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewRemoteHost</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>RemoteHost</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewExternalPort</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ExternalPort</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewProtocol</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingProtocol</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewInternalPort</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>InternalPort</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewInternalClient</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>InternalClient</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewEnabled</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingEnabled</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewPortMappingDescription</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingDescription</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewLeaseDuration</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingLeaseDuration</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>AddPortMapping</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewRemoteHost</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>RemoteHost</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewExternalPort</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ExternalPort</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewProtocol</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingProtocol</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewInternalPort</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>InternalPort</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewInternalClient</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>InternalClient</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewEnabled</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingEnabled</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewPortMappingDescription</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingDescription</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewLeaseDuration</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingLeaseDuration</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>DeletePortMapping</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewRemoteHost</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>RemoteHost</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewExternalPort</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ExternalPort</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewProtocol</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingProtocol</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetExternalIPAddress</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewExternalIPAddress</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ExternalIPAddress</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* </actionList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</actionList>");

	/* <serviceStateTable> */
	fprintf_indent(i, fp, "%s\n", "<serviceStateTable>");
	i++;

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>ConnectionType</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>PossibleConnectionTypes</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* <allowedValueList> */
	fprintf_indent(i, fp, "%s\n", "<allowedValueList>");
	i++;

	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>Unconfigured</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>IP_Routed</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>DHCP_Spoofed</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>PPPoE_Bridged</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>PPTP_Relay</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>L2TP_Relay</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>PPPoE_Relay</allowedValue>");

	/* </allowedValueList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</allowedValueList>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>ConnectionStatus</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* <allowedValueList> */
	fprintf_indent(i, fp, "%s\n", "<allowedValueList>");
	i++;

	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>Unconfigured</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>Connected</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>Disconnected</allowedValue>");

	/* </allowedValueList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</allowedValueList>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>Uptime</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>UpstreamMaxBitRate</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>DownstreamMaxBitRate</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>LastConnectionError</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* <allowedValueList> */
	fprintf_indent(i, fp, "%s\n", "<allowedValueList>");
	i++;

	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>ERROR_NONE</allowedValue>");

	/* </allowedValueList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</allowedValueList>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>AutoDisconnectTime</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>IdleDisconnectTime</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>WarnDisconnectDelay</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>RSIPAvailable</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>boolean</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NATEnabled</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>boolean</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>UserName</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>Password</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>PPPEncryptionProtocol</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>PPPCompressionProtocol</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>PPPAuthenticationProtocol</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>ExternalIPAddress</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>PortMappingNumberOfEntries</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui2</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>PortMappingEnabled</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>boolean</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>PortMappingLeaseDuration</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>RemoteHost</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>ExternalPort</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui2</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>InternalPort</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui2</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>PortMappingProtocol</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* <allowedValueList> */
	fprintf_indent(i, fp, "%s\n", "<allowedValueList>");
	i++;

	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>TCP</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>UDP</allowedValue>");

	/* </allowedValueList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</allowedValueList>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>InternalClient</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>PortMappingDescription</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* </serviceStateTable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</serviceStateTable>");

	/* </scpd> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</scpd>");

	fclose(fp);

	return (EXIT_SUCCESS);
}

static int gen_igd1_service_WANIPConnection1_xml(const char *path, int flag)
{
	FILE *fp;
	int i;

	fp = fopen(path, "w");
	if (fp == NULL)
		return (EXIT_FAILURE);

	i = 0;
	/* xml */
	fprintf_indent(i, fp, "%s\n", "<?xml version=\"1.0\"?>");

	/* <scpd> */
	fprintf_indent(i, fp, "%s\n", "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">");
	i++;

	/* <specVersion> */
	fprintf_indent(i, fp, "%s\n", "<specVersion>");
	i++;

	fprintf_indent(i, fp, "%s\n", "<major>1</major>");
	fprintf_indent(i, fp, "%s\n", "<minor>0</minor>");

	/* </specVersion> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</specVersion>");

	/* <actionList> */
	fprintf_indent(i, fp, "%s\n", "<actionList>");
	i++;

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SetConnectionType</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewConnectionType</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ConnectionType</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetConnectionTypeInfo</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewConnectionType</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ConnectionType</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewPossibleConnectionTypes</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PossibleConnectionTypes</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>RequestConnection</name>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>RequestTermination</name>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>ForceTermination</name>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SetAutoDisconnectTime</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewAutoDisconnectTime</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>AutoDisconnectTime</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SetIdleDisconnectTime</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewIdleDisconnectTime</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>IdleDisconnectTime</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SetWarnDisconnectDelay</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewWarnDisconnectDelay</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>WarnDisconnectDelay</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetStatusInfo</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewConnectionStatus</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ConnectionStatus</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewLastConnectionError</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>LastConnectionError</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewUptime</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>Uptime</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetAutoDisconnectTime</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewAutoDisconnectTime</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>AutoDisconnectTime</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetIdleDisconnectTime</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewIdleDisconnectTime</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>IdleDisconnectTime</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetWarnDisconnectDelay</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewWarnDisconnectDelay</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>WarnDisconnectDelay</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetNATRSIPStatus</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewRSIPAvailable</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>RSIPAvailable</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewNATEnabled</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>NATEnabled</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetGenericPortMappingEntry</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewPortMappingIndex</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingNumberOfEntries</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewRemoteHost</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>RemoteHost</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewExternalPort</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ExternalPort</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewProtocol</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingProtocol</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewInternalPort</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>InternalPort</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewInternalClient</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>InternalClient</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewEnabled</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingEnabled</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewPortMappingDescription</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingDescription</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewLeaseDuration</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingLeaseDuration</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetSpecificPortMappingEntry</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewRemoteHost</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>RemoteHost</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewExternalPort</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ExternalPort</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewProtocol</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingProtocol</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewInternalPort</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>InternalPort</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewInternalClient</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>InternalClient</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewEnabled</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingEnabled</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewPortMappingDescription</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingDescription</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewLeaseDuration</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingLeaseDuration</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>AddPortMapping</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewRemoteHost</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>RemoteHost</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewExternalPort</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ExternalPort</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewProtocol</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingProtocol</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewInternalPort</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>InternalPort</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewInternalClient</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>InternalClient</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewEnabled</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingEnabled</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewPortMappingDescription</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingDescription</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewLeaseDuration</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingLeaseDuration</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>DeletePortMapping</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewRemoteHost</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>RemoteHost</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewExternalPort</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ExternalPort</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewProtocol</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>PortMappingProtocol</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetExternalIPAddress</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewExternalIPAddress</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ExternalIPAddress</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* </actionList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</actionList>");



	/* <serviceStateTable> */
	fprintf_indent(i, fp, "%s\n", "<serviceStateTable>");
	i++;

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>ConnectionType</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>PossibleConnectionTypes</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* <allowedValueList> */
	fprintf_indent(i, fp, "%s\n", "<allowedValueList>");
	i++;

	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>Unconfigured</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>IP_Routed</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>IP_Bridged</allowedValue>");

	/* </allowedValueList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</allowedValueList>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>ConnectionStatus</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* <allowedValueList> */
	fprintf_indent(i, fp, "%s\n", "<allowedValueList>");
	i++;

	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>Unconfigured</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>Connected</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>Disconnected</allowedValue>");

	/* </allowedValueList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</allowedValueList>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>Uptime</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>LastConnectionError</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* <allowedValueList> */
	fprintf_indent(i, fp, "%s\n", "<allowedValueList>");
	i++;

	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>ERROR_NONE</allowedValue>");

	/* </allowedValueList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</allowedValueList>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>AutoDisconnectTime</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>IdleDisconnectTime</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>WarnDisconnectDelay</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>RSIPAvailable</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>boolean</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NATEnabled</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>boolean</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>ExternalIPAddress</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"yes\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>PortMappingNumberOfEntries</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui2</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>PortMappingEnabled</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>boolean</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>PortMappingLeaseDuration</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui4</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>RemoteHost</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>ExternalPort</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui2</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>InternalPort</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>ui2</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>PortMappingProtocol</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* <allowedValueList> */
	fprintf_indent(i, fp, "%s\n", "<allowedValueList>");
	i++;

	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>TCP</allowedValue>");
	/* <allowedValue> */
	fprintf_indent(i, fp, "%s\n", "<allowedValue>UDP</allowedValue>");

	/* </allowedValueList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</allowedValueList>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>InternalClient</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>PortMappingDescription</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* </serviceStateTable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</serviceStateTable>");

	/* </scpd> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</scpd>");

	fclose(fp);

	return (EXIT_SUCCESS);
}

static int gen_igd1_service_LANHostConfigManagement1_xml(const char *path, int flag)
{
	FILE *fp;
	int i;

	fp = fopen(path, "w");
	if (fp == NULL)
		return (EXIT_FAILURE);

	i = 0;
	/* xml */
	fprintf_indent(i, fp, "%s\n", "<?xml version=\"1.0\"?>");

	/* <scpd> */
	fprintf_indent(i, fp, "%s\n", "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">");
	i++;

	/* <specVersion> */
	fprintf_indent(i, fp, "%s\n", "<specVersion>");
	i++;

	fprintf_indent(i, fp, "%s\n", "<major>1</major>");
	fprintf_indent(i, fp, "%s\n", "<minor>0</minor>");

	/* </specVersion> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</specVersion>");

	/* <actionList> */
	fprintf_indent(i, fp, "%s\n", "<actionList>");
	i++;

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SetDHCPServerConfigurable</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewDHCPServerConfigurable</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>DHCPServerConfigurable</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetDHCPServerConfigurable</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewDHCPServerConfigurable</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>DHCPServerConfigurable</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SetDHCPRelay</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewDHCPRelay</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>DHCPRelay</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetDHCPRelay</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewDHCPRelay</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>DHCPRelay</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SetSubnetMask</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewSubnetMask</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>SubnetMask</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetSubnetMask</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewSubnetMask</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>SubnetMask</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SetIPRouter</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewIPRouters</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>IPRouters</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>DeleteIPRouter</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewIPRouters</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>IPRouters</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetIPRoutersList</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewIPRouters</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>IPRouters</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SetDomainName</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewDomainName</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>DomainName</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetDomainName</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewDomainName</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>DomainName</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SetAddressRange</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewMinAddress</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>MinAddress</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewMaxAddress</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>MaxAddress</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetAddressRange</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewMinAddress</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>MinAddress</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewMaxAddress</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>MaxAddress</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SetReservedAddress</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewReservedAddresses</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ReservedAddresses</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>DeleteReservedAddress</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewReservedAddresses</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ReservedAddresses</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetReservedAddresses</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewReservedAddresses</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>ReservedAddresses</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SetDNSServer</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewDNSServers</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>DNSServers</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>DeleteDNSServer</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewDNSServers</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>DNSServers</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>in</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* <action> */
	fprintf_indent(i, fp, "%s\n", "<action>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>GetDNSServers</name>");

	/* <argumentList> */
	fprintf_indent(i, fp, "%s\n", "<argumentList>");
	i++;

	/* <argument> */
	fprintf_indent(i, fp, "%s\n", "<argument>");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>NewDNSServers</name>");
	/* <relatedStateVariable> */
	fprintf_indent(i, fp, "%s\n", "<relatedStateVariable>DNSServers</relatedStateVariable>");
	/* <direction> */
	fprintf_indent(i, fp, "%s\n", "<direction>out</direction>");

	/* </argument> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argument>");

	/* </argumentList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</argumentList>");

	/* </action> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</action>");

	/* </actionList> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</actionList>");

	/* <serviceStateTable> */
	fprintf_indent(i, fp, "%s\n", "<serviceStateTable>");
	i++;

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>DHCPServerConfigurable</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>boolean</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>DHCPRelay</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>boolean</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>SubnetMask</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>IPRouters</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>DNSServers</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>DomainName</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>MinAddress</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>MaxAddress</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* <stateVariable> */
	fprintf_indent(i, fp, "%s\n", "<stateVariable sendEvents=\"no\">");
	i++;

	/* <name> */
	fprintf_indent(i, fp, "%s\n", "<name>ReservedAddresses</name>");
	/* <dataType> */
	fprintf_indent(i, fp, "%s\n", "<dataType>string</dataType>");

	/* </stateVariable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</stateVariable>");

	/* </serviceStateTable> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</serviceStateTable>");

	/* </scpd> */
	i--;
	fprintf_indent(i, fp, "%s\n", "</scpd>");

	fclose(fp);

	return (EXIT_SUCCESS);
}

int pop_etc_ezcfg_upnpd_igd1(int flag)
{
	switch (flag) {
	case RC_ACT_BOOT :
	case RC_ACT_START :
	case RC_ACT_RELOAD :
	case RC_ACT_RESTART :
		mkdir("/etc/ezcfg_upnpd/igd1", 0755);
		mkdir("/etc/ezcfg_upnpd/igd1/service", 0755);

		/* IGD root device */
		if (gen_igd1_InternetGatewayDevice1_xml(IGD_XML_PATH, flag) == EXIT_FAILURE)
			return (EXIT_FAILURE);

		/* Layer3Forwarding service */
		if (gen_igd1_service_Layer3Forwarding1_xml(L3F_XML_PATH, flag) == EXIT_FAILURE)
			return (EXIT_FAILURE);

		/* WANCommonInterfaceConfig service */
		if (gen_igd1_service_WANCommonInterfaceConfig1_xml(WCIC_XML_PATH, flag) == EXIT_FAILURE)
			return (EXIT_FAILURE);

		/* WANEthernetLinkConfig service */
		if (gen_igd1_service_WANEthernetLinkConfig1_xml(WELC_XML_PATH, flag) == EXIT_FAILURE)
			return (EXIT_FAILURE);

		/* WANPPPConnection service */
		if (gen_igd1_service_WANPPPConnection1_xml(WPC_XML_PATH, flag) == EXIT_FAILURE)
			return (EXIT_FAILURE);

		/* WANIPConnection service */
		if (gen_igd1_service_WANIPConnection1_xml(WIC_XML_PATH, flag) == EXIT_FAILURE)
			return (EXIT_FAILURE);

		/* LANHostConfigManagement service */
		if (gen_igd1_service_LANHostConfigManagement1_xml(LHCM_XML_PATH, flag) == EXIT_FAILURE)
			return (EXIT_FAILURE);

		break;

	default:
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}
