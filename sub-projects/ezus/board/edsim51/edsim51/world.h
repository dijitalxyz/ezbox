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

extern uint8_t w_space_init(void);
extern uint8_t w_time_init(void);
extern void w_schedule_threads(void);
/* extern void w_world_start(void (* const thread_list[W_THREAD_NUM])(void)); */
extern void w_world_start(void);

#endif
