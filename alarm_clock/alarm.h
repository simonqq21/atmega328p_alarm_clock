#ifndef ALARM_H
#define ALARM_H

#include "src/config.h"
#include "src/millis_micros.h"
#include "src/gpio.h"
#include "src/button.h"
#include "src/piezo.h"
#include "src/seven_segment.h"
#include "src/rtc.h"
// #include "src/serial.h"
#include "src/states.h"

#define EEPROM_ALARM_SETTINGS_ADDR (0x0)
#define ALARM_DURATION_MINS (5)
// alarm settings struct
typedef struct
{
    uint8_t enabled;
    uint8_t alarm_hour;
    uint8_t alarm_min;
} alarm_settings_t;
alarm_settings_t alarm_settings;
typedef struct
{
    uint8_t start_hour;
    uint8_t start_min;
    uint16_t start_mins_since_midnight;
    uint8_t duration_mins;
    uint8_t end_hour;
    uint8_t end_min;
    uint16_t end_mins_since_midnight;
    uint16_t cur_mins_since_midnight;
    uint8_t within_alarm_period;
    uint8_t prev_triggered;
    uint8_t dismissed;
    uint8_t adjusting_time;
} alarm_memory_t;
void load_alarm_settings(void);
void update_alarm_settings(void);
void trim_alarm_values(void);
void compute_alarm_time(void);

void dismiss_alarm(void);
void set_alarm_dismiss_button_callbacks();
void restore_button_callbacks(void);

void alarm_loop(void);

#endif