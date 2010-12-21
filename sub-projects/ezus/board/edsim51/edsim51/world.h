#ifndef _WORLD_H_
#define _WORLD_H_

#include <stdint.h>
#include <8051.h>

#define W_P1_0	P1_0
#define W_P1_1	P1_1

#define W_MAX_THREADS	8

extern uint8_t w_space_init(void);
extern uint8_t w_time_init(void);
extern void w_start_world(void (* const thread_list[W_MAX_THREADS])(void));

#endif
