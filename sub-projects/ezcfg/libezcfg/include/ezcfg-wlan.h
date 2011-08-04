/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-wlan.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-08-02   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_WLAN_H_
#define _EZCFG_WLAN_H_

/* ezcfg nvram name prefix */
#define EZCFG_WLAN_NVRAM_PREFIX            "wlan_"

/* ezcfg wlan control names */
/* WLAN H/W parameters */
#define EZCFG_WLAN_IFNAME                 "ifname"
#define EZCFG_WLAN_IFNAMES                "ifnames"
#define EZCFG_WLAN_HWNAME                 "hwname"
#define EZCFG_WLAN_HWADDR                 "hwaddr"
#define EZCFG_WLAN_PHYMODE                "phymode"

/* hostapd wpa_supplicant related options */
#define EZCFG_WLAN_SCAN_SSID              "scan_ssid"
#define EZCFG_WLAN_SSID                   "ssid"
#define EZCFG_WLAN_BSSID                  "bssid"
#define EZCFG_WLAN_KEY_MGMT               "key_mgmt"
#define EZCFG_WLAN_PROTO                  "proto"
#define EZCFG_WLAN_IEEE80211W             "ieee80211w"
#define EZCFG_WLAN_PASSPHRASE             "passphrase"
#define EZCFG_WLAN_PAIRWISE               "pairwise"
#define EZCFG_WLAN_GROUP                  "group"
#define EZCFG_WLAN_EAP_TYPE               "eap_type"
#define EZCFG_WLAN_CA_CERT                "ca_cert"
#define EZCFG_WLAN_PRIV_KEY               "priv_key"
#define EZCFG_WLAN_PRIV_KEY_PWD           "priv_key_pwd"
#define EZCFG_WLAN_PHASE2                 "phase2"
#define EZCFG_WLAN_IDENTITY               "identity"
#define EZCFG_WLAN_PASSWORD               "password"
#define EZCFG_WLAN_WEP_KEY0               "wep_key0"
#define EZCFG_WLAN_WEP_KEY1               "wep_key1"
#define EZCFG_WLAN_WEP_KEY2               "wep_key2"
#define EZCFG_WLAN_WEP_KEY3               "wep_key3"
#define EZCFG_WLAN_WEP_TX_KEYIDX          "wep_tx_keyidx"

#endif
