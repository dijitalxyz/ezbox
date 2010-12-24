#ifndef _WORLD_H_
#define _WORLD_H_

#include <stdint.h>
#include <8051.h>

#define W_P1_0	P1_0
#define W_P1_1	P1_1

#define W_THREAD_MAX	8
#define W_THREAD_NUM	2

#if (W_THREAD_MAX < W_THREAD_NUM)
#error "W_THREAD_NUM should be less than or equal to W_THREAD_MAX"
#endif

typedef struct world_data_s {
	uint8_t	wid;
	uint32_t time_ticks;		/* time ticks since last update
	                    		   of world_uptime_seconds */
	uint32_t uptime_seconds;	/* seconds since world starts */
	uint8_t cur_thread_id;		/* current running thread ID */
	uint8_t thread_quantum[W_THREAD_NUM];	/* thread running quantum of ticks */
} world_data_t;

typedef struct world_rules_s {
	__code void (* space_init)(__data world_data_t *dp) __reentrant;
	__code void (* time_init)(__data world_data_t *dp) __reentrant;
	__code void (* threads[W_THREAD_NUM])(void) __reentrant;
	__code void (* thread_init)(__data world_data_t *dp) __reentrant;
	__code void (* thread_context_switch)(
		__code const struct world_rules_s *rp,
		__data world_data_t *dp, __data uint8_t i) __reentrant;
	__code void (* thread_schedule)(
		__code const struct world_rules_s *rp,
		__data world_data_t *dp) __reentrant;
	__code void (* startup)(
		__code const struct world_rules_s *rp,
		__data world_data_t *dp) __reentrant;
} world_rules_t;

extern __code void w_space_init(__data world_data_t *dp) __using 2;
extern __code void w_time_init(__data world_data_t *dp) __using 2;
extern __code void (* w_threads[W_THREAD_NUM])(void);
extern __code void w_thread_init(__data world_data_t *dp) __using 2;
extern __code void w_thread_context_switch(__code const world_rules_t *rp,
                                           __data world_data_t *dp,
                                            __data uint8_t i) __using 2;
extern __code void w_thread_schedule(__code const world_rules_t *rp,
                                     __data world_data_t *dp) __using 2;
extern __code void w_startup(__code const world_rules_t *rp,
                             __data world_data_t *dp) __using 2;

#endif
