;--------------------------------------------------------------------------
;  ezus.s
;
;  Copyright (C) 2010, Zeta Labs
;
;  This library is free software; you can redistribute it and/or modify it
;  under the terms of the GNU General Public License as published by the
;  Free Software Foundation; either version 2.1, or (at your option) any
;  later version.
;
;  This library is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License 
;  along with this library; see the file COPYING. If not, write to the
;  Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
;   MA 02110-1301, USA.
;
;  As a special exception, if you link this library with other files,
;  some of which are compiled with SDCC, to produce an executable,
;  this library does not by itself cause the resulting executable to
;  be covered by the GNU General Public License. This exception does
;  not however invalidate any other reasons why the executable file
;  might be covered by the GNU General Public License.
;--------------------------------------------------------------------------

;--------------------------------------------------------
; interrupt vector
;--------------------------------------------------------
	.area HOME (CODE)
;	0x0000	systerm startup/reset
system_reset:
	ljmp	stay_at_chaos
;	0x0003	external 0 interrupt
	ljmp	_external0_interrupt_handler
	.ds	5
;	0x000b	timer 0 interrupt
;	disable interrupts
	clr	_EA
	ljmp	_timer0_interrupt_handler
	.ds	3
;	0x0013	external 1 interrupt
	ljmp	_extenal1_interrupt_handler
	.ds	5
;	0x001b	timer 1 interrupt
	ljmp	_timer1_interrupt_handler
	.ds	5
;	0x0023	serial interrupt
	ljmp	_serial_interrupt_handler
	.ds	5

