/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-wpa_supplicant.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-08-04   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_WPA_SUPPLICANT_H_
#define _EZCFG_WPA_SUPPLICANT_H_

/* ezcfg nvram name prefix */
#define EZCFG_WPA_SUPPLICANT_NVRAM_PREFIX            "wpa_supplicant_"

/* hostapd wpa_supplicant related options */
#define EZCFG_WPA_SUPPLICANT_SCAN_SSID              "scan_ssid"
#define EZCFG_WPA_SUPPLICANT_SSID                   "ssid"
#define EZCFG_WPA_SUPPLICANT_BSSID                  "bssid"
#define EZCFG_WPA_SUPPLICANT_KEY_MGMT               "key_mgmt"
#define EZCFG_WPA_SUPPLICANT_PROTO                  "proto"
#define EZCFG_WPA_SUPPLICANT_IEEE80211W             "ieee80211w"
#define EZCFG_WPA_SUPPLICANT_PSK                    "psk"
#define EZCFG_WPA_SUPPLICANT_PAIRWISE               "pairwise"
#define EZCFG_WPA_SUPPLICANT_GROUP                  "group"
#define EZCFG_WPA_SUPPLICANT_EAP                    "eap"
#define EZCFG_WPA_SUPPLICANT_CA_CERT                "ca_cert"
#define EZCFG_WPA_SUPPLICANT_PRIV_KEY               "private_key"
#define EZCFG_WPA_SUPPLICANT_PRIV_KEY_PWD           "private_key_passwd"
#define EZCFG_WPA_SUPPLICANT_PHASE2                 "phase2"
#define EZCFG_WPA_SUPPLICANT_IDENTITY               "identity"
#define EZCFG_WPA_SUPPLICANT_PASSWORD               "password"
#define EZCFG_WPA_SUPPLICANT_WEP_KEY0               "wep_key0"
#define EZCFG_WPA_SUPPLICANT_WEP_KEY1               "wep_key1"
#define EZCFG_WPA_SUPPLICANT_WEP_KEY2               "wep_key2"
#define EZCFG_WPA_SUPPLICANT_WEP_KEY3               "wep_key3"
#define EZCFG_WPA_SUPPLICANT_WEP_TX_KEYIDX          "wep_tx_keyidx"

#endif
