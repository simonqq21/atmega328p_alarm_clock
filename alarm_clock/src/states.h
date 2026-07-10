#ifndef STATES_H
#define STATES_H

typedef enum
{
    MAIN_STATE_HOUR_MIN,
    MAIN_STATE_ALARM,
    MAIN_STATE_MONTH_DAY,
    MAIN_STATE_YEAR,
    MAIN_STATE_TEMPERATURE,
    MAIN_STATE_HUMIDITY,
    MAIN_STATE_TESTING,
    MAIN_STATE_END
} alarm_clock_main_state_t;

typedef enum
{
    HOUR_MIN_STATE_DISPLAY,
    HOUR_MIN_STATE_ADJUST_HOUR,
    HOUR_MIN_STATE_ADJUST_MIN,
    HOUR_MIN_STATE_END,
} hour_min_state_t;

typedef enum
{
    ALARM_STATE_DISPLAY, // display OFF if disabled, display alarm time if enabled
    ALARM_STATE_ADJUST_HOUR,
    ALARM_STATE_ADJUST_MIN,
    ALARM_STATE_END,
} alarm_state_t;

typedef enum
{
    MONTH_DAY_STATE_DISPLAY,
    MONTH_DAY_STATE_ADJUST_MONTH,
    MONTH_DAY_STATE_ADJUST_DAY,
    MONTH_DAY_STATE_END,
} month_day_state_t;

typedef enum
{
    YEAR_STATE_DISPLAY,
    YEAR_STATE_ADJUST_YEAR,
    YEAR_STATE_END,
} year_state_t;

typedef enum
{
    TEMPERATURE_STATE_DISPLAY_TEMPERATURE,
    TEMPERATURE_STATE_END,
} temperature_state_t;

typedef enum
{
    HUMIDITY_STATE_DISPLAY_HUMIDITY,
    HUMIDITY_STATE_END,
} humidity_state_t;

typedef enum
{
    TESTING_STATE_ALL_OFF,
    TESTING_STATE_ALL_ON,
    TESTING_STATE_END,
} testing_state_t;

#endif