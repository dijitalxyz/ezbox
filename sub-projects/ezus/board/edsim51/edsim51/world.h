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
} world_data_t;

typedef struct world_rules_s {
	__code void (* const space_init)(__data world_data_t *dp) __reentrant;
	__code void (* const time_init)(__data world_data_t *dp) __reentrant;
	__code void (* const thread_init)(__data world_data_t *dp) __reentrant;
	__code void (* const thread_schedule)(__data world_data_t *dp) __reentrant;
	__code void (* const startup)(__code struct world_rules_s *rp,
	                              __data struct world_data_s *dp) __reentrant;
} world_rules_t;

extern __code void w_space_init(__data world_data_t *dp) __using 2;
extern __code void w_time_init(__data world_data_t *dp) __using 2;
extern __code void w_thread_init(__data world_data_t *dp) __using 2;
extern __code void w_thread_schedule(__data world_data_t *dp) __using 2;
extern __code void w_startup(__code world_rules_t *rp,
                             __data world_data_t *dp) __using 2;

#endif
