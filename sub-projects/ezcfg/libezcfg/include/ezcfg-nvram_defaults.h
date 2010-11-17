/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-nvram_defaults.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-16   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_NVRAM_DEFAULTS_H_
#define _EZCFG_NVRAM_DEFAULTS_H_

#include "ezcfg-dnsmasq.h"

/* ezcfg nvram rc control names */
#define EZCFG_NVRAM_RC_SYSLOG_ENABLE_NAME            "rc_syslog_enable"
#define EZCFG_NVRAM_RC_TELNETD_ENABLE_NAME           "rc_telnetd_enable"
#define EZCFG_NVRAM_RC_DNSMASQ_ENABLE_NAME           "rc_dnsmasq_enable"

/* ezcfg nvram dnsmasq control names */
#define EZCFG_NVRAM_DNSMASQ_DOMAIN_NEEDED_NAME       "dnsmasq_domain-needed"
#define EZCFG_NVRAM_DNSMASQ_BOGUS_PRIV_NAME          "dnsmasq_bogus-priv"
#define EZCFG_NVRAM_DNSMASQ_FILTERWIN2K_NAME         "dnsmasq_filterwin2k"
#define EZCFG_NVRAM_DNSMASQ_RESOLV_FILE_NAME         "dnsmasq_resolv-file"
#define EZCFG_NVRAM_DNSMASQ_STRICT_ORDER_NAME        "dnsmasq_strict-order"
#define EZCFG_NVRAM_DNSMASQ_NO_RESOLV_NAME           "dnsmasq_no-resolv"
#define EZCFG_NVRAM_DNSMASQ_NO_POLL_NAME             "dnsmasq_no-poll"
#define EZCFG_NVRAM_DNSMASQ_SERVER_NAME              "dnsmasq_server"
#define EZCFG_NVRAM_DNSMASQ_LOCAL_NAME               "dnsmasq_local"
#define EZCFG_NVRAM_DNSMASQ_ADDRESS_NAME             "dnsmasq_address"
#define EZCFG_NVRAM_DNSMASQ_USER_NAME                "dnsmasq_user"
#define EZCFG_NVRAM_DNSMASQ_GROUP_NAME               "dnsmasq_group"
#define EZCFG_NVRAM_DNSMASQ_INTERFACE_NAME           "dnsmasq_interface"
#define EZCFG_NVRAM_DNSMASQ_EXCEPT_INTERFACE_NAME    "dnsmasq_except-interface"
#define EZCFG_NVRAM_DNSMASQ_LISTEN_ADDRESS_NAME      "dnsmasq_listen-address"
#define EZCFG_NVRAM_DNSMASQ_NO_DHCP_INTERFACE_NAME   "dnsmasq_no-dhcp-interface"
#define EZCFG_NVRAM_DNSMASQ_BIND_INTERFACE_NAME      "dnsmasq_bind-interface"
#define EZCFG_NVRAM_DNSMASQ_NO_HOSTS_NAME            "dnsmasq_no-hosts"
#define EZCFG_NVRAM_DNSMASQ_ADDN_HOSTS_NAME          "dnsmasq_addn-hosts"
#define EZCFG_NVRAM_DNSMASQ_EXPAND_HOSTS_NAME        "dnsmasq_expand-hosts"
#define EZCFG_NVRAM_DNSMASQ_DOMAIN_NAME              "dnsmasq_domain"
#define EZCFG_NVRAM_DNSMASQ_DHCP_RANGE_NAME          "dnsmasq_dhcp-range"
#define EZCFG_NVRAM_DNSMASQ_DHCP_HOST_NAME           "dnsmasq_dhcp-host"
#define EZCFG_NVRAM_DNSMASQ_DHCP_IGNORE_NAME         "dnsmasq_dhcp-ignore"
#define EZCFG_NVRAM_DNSMASQ_DHCP_VENDORCLASS_NAME    "dnsmasq_dhcp-vendorclass"
#define EZCFG_NVRAM_DNSMASQ_DHCP_USERCLASS_NAME      "dnsmasq_dhcp-userclass"
#define EZCFG_NVRAM_DNSMASQ_DHCP_MAC_NAME            "dnsmasq_dhcp-mac"
#define EZCFG_NVRAM_DNSMASQ_READ_ETHERS_NAME         "dnsmasq_read-ethers"
#define EZCFG_NVRAM_DNSMASQ_DHCP_OPTION_NAME         "dnsmasq_dhcp-option"
#define EZCFG_NVRAM_DNSMASQ_DHCP_OPTION_FORCE_NAME   "dnsmasq_dhcp-option-force"
#define EZCFG_NVRAM_DNSMASQ_DHCP_BOOT_NAME           "dnsmasq_dhcp-boot"
#define EZCFG_NVRAM_DNSMASQ_DHCP_MATCH_NAME          "dnsmasq_dhcp-match"
#define EZCFG_NVRAM_DNSMASQ_PXE_PROMPT_NAME          "dnsmasq_pxe-prompt"
#define EZCFG_NVRAM_DNSMASQ_PXE_SERVICE_NAME         "dnsmasq_pxe-service"
#define EZCFG_NVRAM_DNSMASQ_ENABLE_TFTP_NAME         "dnsmasq_enable-tftp"
#define EZCFG_NVRAM_DNSMASQ_TFTP_ROOT_NAME           "dnsmasq_tftp-root"
#define EZCFG_NVRAM_DNSMASQ_TFTP_SECURE_NAME         "dnsmasq_tftp-secure"
#define EZCFG_NVRAM_DNSMASQ_TFTP_NO_BLOCKSIZE_NAME   "dnsmasq_tftp-no-blocksize"
#define EZCFG_NVRAM_DNSMASQ_DHCP_LEASE_MAX_NAME      "dnsmasq_dhcp-lease-max"
#define EZCFG_NVRAM_DNSMASQ_DHCP_LEASEFILE_NAME      "dnsmasq_dhcp-leasefile"
#define EZCFG_NVRAM_DNSMASQ_DHCP_AUTHORITATIVE_NAME  "dnsmasq_dhcp-authoritative"
#define EZCFG_NVRAM_DNSMASQ_DHCP_SCRIPT_NAME         "dnsmasq_dhcp-script"
#define EZCFG_NVRAM_DNSMASQ_CACHE_SIZE_NAME          "dnsmasq_cache-size"
#define EZCFG_NVRAM_DNSMASQ_NO_NEGCACHE_NAME         "dnsmasq_no-negcache"
#define EZCFG_NVRAM_DNSMASQ_LOCAL_TTL_NAME           "dnsmasq_local-ttl"
#define EZCFG_NVRAM_DNSMASQ_BOGUS_NXDOMAIN_NAME      "dnsmasq_bogus-nxdomain"
#define EZCFG_NVRAM_DNSMASQ_ALIAS_NAME               "dnsmasq_alias"
#define EZCFG_NVRAM_DNSMASQ_MX_HOST_NAME             "dnsmasq_mx-host"
#define EZCFG_NVRAM_DNSMASQ_MX_TARGET_NAME           "dnsmasq_mx-target"
#define EZCFG_NVRAM_DNSMASQ_LOCALMX_NAME             "dnsmasq_localmx"
#define EZCFG_NVRAM_DNSMASQ_SELFMX_NAME              "dnsmasq_selfmx"
#define EZCFG_NVRAM_DNSMASQ_SRV_HOST_NAME            "dnsmasq_srv-host"
#define EZCFG_NVRAM_DNSMASQ_PTR_RECORD_NAME          "dnsmasq_ptr-record"
#define EZCFG_NVRAM_DNSMASQ_TXT_RECORD_NAME          "dnsmasq_txt-record"
#define EZCFG_NVRAM_DNSMASQ_CNAME_NAME               "dnsmasq_cname"
#define EZCFG_NVRAM_DNSMASQ_LOG_QUERIES_NAME         "dnsmasq_log-queries"
#define EZCFG_NVRAM_DNSMASQ_LOG_DHCP_NAME            "dnsmasq_log-dhcp"
#define EZCFG_NVRAM_DNSMASQ_CONF_FILE_NAME           "dnsmasq_conf-file"
#define EZCFG_NVRAM_DNSMASQ_CONF_DIR_NAME            "dnsmasq_conf-dir"

#endif
