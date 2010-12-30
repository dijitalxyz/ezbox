#ifndef _UNIVERSE_H_
#define _UNIVERSE_H_

#include "world.h"

#define U_WORLD_MAX	1
#define U_WORLD_NUM	1

#if (U_WORLD_MAX < U_WORLD_NUM)
#error "U_WORLD_NUM should be less than or equal to U_WORLD_MAX"
#endif

#define U_SP_BOTTOM	W_SP_BOTTOM
#define U_TIMER0_COUNT	W_TIMER0_COUNT

#if 0
typedef struct universe_s {
	world_rules_t world_rules_list[U_WORLD_NUM];
	world_data_t world_data_list[U_WORLD_NUM];
} universe_t;
#endif

extern void universe(void) __using 1;

#endif
