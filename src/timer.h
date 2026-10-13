#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

/* mtime on QEMU virt runs at 10 MHz */
#define TIMER_FREQ_HZ   10000000u

void timer_init(uint32_t interval);
void timer_handler(void);
uint32_t timer_get_ticks(void);

#endif
