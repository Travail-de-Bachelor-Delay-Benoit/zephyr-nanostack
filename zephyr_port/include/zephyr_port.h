#ifndef _ZEPHYR_PORT_H_
#define _ZEPHYR_PORT_H_

#include <stdint.h>

void eventOS_scheduler_signal(void);

void platform_enter_critical();

void platform_exit_critical();

uint16_t platform_timer_get_remaining_slots(void);

void platform_timer_set_cb(void (*new_fp)(void));

void platform_timer_start(uint16_t slots);

#endif