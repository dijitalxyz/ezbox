/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-ezbox_distro.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-11   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_EZBOX_DISTRO_H_
#define _EZCFG_EZBOX_DISTRO_H_

#define EZCFG_EZBOX_DISTRO_KUAFU	1
#define EZCFG_EZBOX_DISTRO_HUANGDI	2
#define EZCFG_EZBOX_DISTRO_JINGWEI	3
#define EZCFG_EZBOX_DISTRO_QIAOCHUI	4

#if defined(CONFIG_EZCFG_EZBOX_DISTRO_KUAFU)
#define EZCFG_EZBOX_DISTRO	EZCFG_EZBOX_DISTRO_KUAFU
#elif defined(CONFIG_EZCFG_EZBOX_DISTRO_HUANGDI)
#define EZCFG_EZBOX_DISTRO	EZCFG_EZBOX_DISTRO_HUANGDI
#elif defined(CONFIG_EZCFG_EZBOX_DISTRO_JINGWEI)
#define EZCFG_EZBOX_DISTRO	EZCFG_EZBOX_DISTRO_JINGWEI
#elif defined(CONFIG_EZCFG_EZBOX_DISTRO_QIAOCHUI)
#define EZCFG_EZBOX_DISTRO	EZCFG_EZBOX_DISTRO_QIAOCHUI
#else
#error "unknown ezbox distro !!!"
#endif

#endif /* _EZCFG_EZBOX_DISTRO_H_ */
