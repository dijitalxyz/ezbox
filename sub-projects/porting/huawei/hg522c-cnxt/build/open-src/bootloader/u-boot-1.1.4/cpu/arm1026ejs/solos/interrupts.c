/*
 * (C) Copyright 2002
 * Lineo, Inc. <www.lineo.com>
 * Bernhard Kuhn <bkuhn@lineo.com>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/arch/hardware.h>
#include <include/solos.h>

/* the number of clocks per CFG_HZ */
#define TIMER_LOAD_VAL (CFG_HZ_CLOCK/CFG_HZ)

/* macro to read the 16 bit timer */
#define READ_TIMER CHARMIO(TIMER1_VALUE)
AT91PS_TC tmr;

static ulong timestamp;
static ulong lastinc;

int interrupt_init (void)
{
  /*
   * Do nothing in case of Solos as we user Timer 1 which is
   * an incrementing counter with a frequency of 8Mhz
   */
  return (0);
}

/*
 * timer without interrupts
 */

void reset_timer (void)
{
	reset_timer_masked ();
}

ulong get_timer (ulong base)
{
	return get_timer_masked () - base;
}

void set_timer (ulong t)
{
	timestamp = t;
}

void udelay (unsigned long usec)
{
	udelay_masked(usec);
}

void reset_timer_masked (void)
{
	/* reset time */
	lastinc = READ_TIMER;
	timestamp = 0;
}

ulong get_timer_raw (void)
{
	ulong now = READ_TIMER;

	timestamp += now - lastinc;
	lastinc = now;

	return timestamp;
}

ulong get_timer_masked (void)
{
	return get_timer_raw();
}

void udelay_masked (unsigned long usec)
{
	ulong tmo;
	ulong endtime;
	signed long diff;

	usec *= 8; /* Hard-coded assumption about an 8MHz clock */
	endtime = READ_TIMER + usec;

	do {
		ulong now = READ_TIMER;
		diff = endtime - now;
	} while (diff >= 0);
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk (void)
{
	ulong tbclk;

	tbclk = CFG_HZ;
	return tbclk;
}

/*
 * Reset the cpu through the watchdog register
 */
void reset_cpu (ulong ignored)
{
  CHARMIO(CHARM_WDOG_CTRL) = CHARM_WDOG_CTRL_SWRESET;
}
