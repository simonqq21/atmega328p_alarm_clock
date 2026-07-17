#ifndef FSM_H
#define FSM_H

#include "src/config.h"
#include "src/millis_micros.h"
#include "src/gpio.h"
#include "src/button.h"
#include "src/piezo.h"
#include "src/seven_segment.h"
#include "src/rtc.h"
// #include "src/serial.h"
#include "src/DHT.h"
#include "src/states.h"
#include "src/light_ws2812.h"

// state updated variable used to run code when switching to a new state
typedef struct
{
    volatile uint8_t state_start;
    volatile uint8_t main_state_swapped;
    volatile uint8_t display_updated;
    volatile alarm_clock_main_state_t main_state;
    // Variables for switching the substates of each main FSM state
    volatile hour_min_state_t hour_min_state;
    volatile alarm_state_t alarm_state;
    volatile month_day_state_t month_day_state;
    volatile year_state_t year_state;
    volatile temperature_state_t temperature_state;
    volatile humidity_state_t humidity_state;
    volatile testing_state_t testing_state;
} fsm_variables_t;

void increment_main_state(void);
void decrement_main_state(void);
void fsm_loop(void);
#endif