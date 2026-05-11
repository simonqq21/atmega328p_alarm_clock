#ifndef TIMELIB_H
#define TIMELIB_H

#include <avr/interrupt.h>
#include <util/atomic.h>

// typedef enum
// {
//     TIMER_PERIOD_1US,
//     TIMER_PERIOD_1MS,
// } timer_period_enum;

extern volatile uint32_t millis;

void init_millis_timer(void);
uint32_t get_millis(void);
uint32_t get_micros(void);

#endif // TIMELIB_H