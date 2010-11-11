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
#define EZCFG_EZBOX_DISTRO_JINGWEI	2
#define EZCFG_EZBOX_DISTRO_HUANGDI	3

#ifndef EZCFG_EZBOX_DISTRO
#error undefined ezbox distro !!!
#else

#if EZCFG_EZBOX_DISTRO == EZCFG_EZBOX_DISTRO_KUAFU
/* good define */
#elif EZCFG_EZBOX_DISTRO == EZCFG_EZBOX_DISTRO_JINGWEI
/* good define */
#elif EZCFG_EZBOX_DISTRO == EZCFG_EZBOX_DISTRO_HUANGDI
/* good define */
#else
#error unknown ezbox distro !!!
#endif

#endif /* EZCFG_EZBOX_DISTRO */

#endif /* _EZCFG_EZBOX_DISTRO_H_ */
