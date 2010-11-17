/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-dnsmasq.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-17   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_DNSMASQ_H_
#define _EZCFG_DNSMASQ_H_

/* ezcfg nvram name prefix */
#define EZCFG_DNSMASQ_NVRAM_PREFIX             "dnsmasq_"

/* ezcfg dnsmasq control names */
#define EZCFG_DNSMASQ_DOMAIN_NEEDED_NAME       "domain-needed"
#define EZCFG_DNSMASQ_BOGUS_PRIV_NAME          "bogus-priv"
#define EZCFG_DNSMASQ_FILTERWIN2K_NAME         "filterwin2k"
#define EZCFG_DNSMASQ_RESOLV_FILE_NAME         "resolv-file"
#define EZCFG_DNSMASQ_STRICT_ORDER_NAME        "strict-order"
#define EZCFG_DNSMASQ_NO_RESOLV_NAME           "no-resolv"
#define EZCFG_DNSMASQ_NO_POLL_NAME             "no-poll"
#define EZCFG_DNSMASQ_SERVER_NAME              "server"
#define EZCFG_DNSMASQ_LOCAL_NAME               "local"
#define EZCFG_DNSMASQ_ADDRESS_NAME             "address"
#define EZCFG_DNSMASQ_USER_NAME                "user"
#define EZCFG_DNSMASQ_GROUP_NAME               "group"
#define EZCFG_DNSMASQ_INTERFACE_NAME           "interface"
#define EZCFG_DNSMASQ_EXCEPT_INTERFACE_NAME    "except-interface"
#define EZCFG_DNSMASQ_LISTEN_ADDRESS_NAME      "listen-address"
#define EZCFG_DNSMASQ_NO_DHCP_INTERFACE_NAME   "no-dhcp-interface"
#define EZCFG_DNSMASQ_BIND_INTERFACE_NAME      "bind-interface"
#define EZCFG_DNSMASQ_NO_HOSTS_NAME            "no-hosts"
#define EZCFG_DNSMASQ_ADDN_HOSTS_NAME          "addn-hosts"
#define EZCFG_DNSMASQ_EXPAND_HOSTS_NAME        "expand-hosts"
#define EZCFG_DNSMASQ_DOMAIN_NAME              "domain"
#define EZCFG_DNSMASQ_DHCP_RANGE_NAME          "dhcp-range"
#define EZCFG_DNSMASQ_DHCP_HOST_NAME           "dhcp-host"
#define EZCFG_DNSMASQ_DHCP_IGNORE_NAME         "dhcp-ignore"
#define EZCFG_DNSMASQ_DHCP_VENDORCLASS_NAME    "dhcp-vendorclass"
#define EZCFG_DNSMASQ_DHCP_USERCLASS_NAME      "dhcp-userclass"
#define EZCFG_DNSMASQ_DHCP_MAC_NAME            "dhcp-mac"
#define EZCFG_DNSMASQ_READ_ETHERS_NAME         "read-ethers"
#define EZCFG_DNSMASQ_DHCP_OPTION_NAME         "dhcp-option"
#define EZCFG_DNSMASQ_DHCP_OPTION_FORCE_NAME   "dhcp-option-force"
#define EZCFG_DNSMASQ_DHCP_BOOT_NAME           "dhcp-boot"
#define EZCFG_DNSMASQ_DHCP_MATCH_NAME          "dhcp-match"
#define EZCFG_DNSMASQ_PXE_PROMPT_NAME          "pxe-prompt"
#define EZCFG_DNSMASQ_PXE_SERVICE_NAME         "pxe-service"
#define EZCFG_DNSMASQ_ENABLE_TFTP_NAME         "enable-tftp"
#define EZCFG_DNSMASQ_TFTP_ROOT_NAME           "tftp-root"
#define EZCFG_DNSMASQ_TFTP_SECURE_NAME         "tftp-secure"
#define EZCFG_DNSMASQ_TFTP_NO_BLOCKSIZE_NAME   "tftp-no-blocksize"
#define EZCFG_DNSMASQ_DHCP_LEASE_MAX_NAME      "dhcp-lease-max"
#define EZCFG_DNSMASQ_DHCP_LEASEFILE_NAME      "dhcp-leasefile"
#define EZCFG_DNSMASQ_DHCP_AUTHORITATIVE_NAME  "dhcp-authoritative"
#define EZCFG_DNSMASQ_DHCP_SCRIPT_NAME         "dhcp-script"
#define EZCFG_DNSMASQ_CACHE_SIZE_NAME          "cache-size"
#define EZCFG_DNSMASQ_NO_NEGCACHE_NAME         "no-negcache"
#define EZCFG_DNSMASQ_LOCAL_TTL_NAME           "local-ttl"
#define EZCFG_DNSMASQ_BOGUS_NXDOMAIN_NAME      "bogus-nxdomain"
#define EZCFG_DNSMASQ_ALIAS_NAME               "alias"
#define EZCFG_DNSMASQ_MX_HOST_NAME             "mx-host"
#define EZCFG_DNSMASQ_MX_TARGET_NAME           "mx-target"
#define EZCFG_DNSMASQ_LOCALMX_NAME             "localmx"
#define EZCFG_DNSMASQ_SELFMX_NAME              "selfmx"
#define EZCFG_DNSMASQ_SRV_HOST_NAME            "srv-host"
#define EZCFG_DNSMASQ_PTR_RECORD_NAME          "ptr-record"
#define EZCFG_DNSMASQ_TXT_RECORD_NAME          "txt-record"
#define EZCFG_DNSMASQ_CNAME_NAME               "cname"
#define EZCFG_DNSMASQ_LOG_QUERIES_NAME         "log-queries"
#define EZCFG_DNSMASQ_LOG_DHCP_NAME            "log-dhcp"
#define EZCFG_DNSMASQ_CONF_FILE_NAME           "conf-file"
#define EZCFG_DNSMASQ_CONF_DIR_NAME            "conf-dir"

#endif
