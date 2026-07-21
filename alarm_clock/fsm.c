#include "fsm.h"
#include "alarm.h"

extern fsm_variables_t fsm_variables;
extern button_action_t prev_adjust_btn_callback, prev_plus_btn_callback, prev_minus_btn_callback;
extern struct tm *clock_time;
extern Button minus_button, adjust_button, plus_button;
extern alarm_settings_t alarm_settings;
extern alarm_memory_t alarm_memory;
extern uint8_t read_rtc;
extern dht_values_t dht_values;
extern uint8_t ALARM_OFF_DISPLAY_VALUE[4];
extern struct cRGB colors[8];
extern struct cRGB led[8];
extern uint32_t t_millis, prev_millis_read_rtc, prev_millis_read_dht;

/**
 * set the state_start and display_updated flags false when a new main FSM state
 * is entered.
 */
void new_state_start()
{
    update_alarm_settings();
    fsm_variables.state_start = false;
    fsm_variables.display_updated = false;
    seven_segment_flash_colon(0);
    seven_segment_flash_digits_hours(0);
    seven_segment_flash_digits_minutes(0);
    seven_segment_flash_all_digits(0);
}

/**
 * button callback to increment through all main alarm clock FSM states.
 */
void increment_main_state(void)
{
    if (fsm_variables.main_state == MAIN_STATE_TESTING)
    {
        fsm_variables.main_state = MAIN_STATE_HOUR_MIN;
    }
    else
    {
        fsm_variables.main_state++;
    }
    fsm_variables.main_state_swapped = true;
    new_state_start();
}

/**
 * button callback to decrement through all main alarm clock FSM states.
 */
void decrement_main_state(void)
{
    if (fsm_variables.main_state == MAIN_STATE_HOUR_MIN)
    {
        fsm_variables.main_state = MAIN_STATE_TESTING;
    }
    else
    {
        fsm_variables.main_state--;
    }
    fsm_variables.main_state_swapped = true;
    new_state_start();
}

/* swap through the different states in the hour minute state */
void swap_hour_min_state(void)
{
    /* update RTC time when exiting the hour and minute adjustment states */
    switch (fsm_variables.hour_min_state)
    {
    case HOUR_MIN_STATE_ADJUST_HOUR:
    case HOUR_MIN_STATE_ADJUST_MIN:
        clock_time->sec = 0;
        rtc_set_time(clock_time);
        break;
    default:
        break;
    }
    fsm_variables.hour_min_state++;
    if (fsm_variables.hour_min_state == HOUR_MIN_STATE_END)
    {
        fsm_variables.hour_min_state = HOUR_MIN_STATE_DISPLAY;
    }
    new_state_start();
}

/* swap through the different states in the alarm state */
void swap_alarm_state(void)
{
    update_alarm_settings();
    fsm_variables.alarm_state++;
    if (fsm_variables.alarm_state == ALARM_STATE_END)
    {
        fsm_variables.alarm_state = ALARM_STATE_DISPLAY;
    }
    new_state_start();
}

/* swap through the different states in the month day state */
void swap_month_day_state(void)
{
    /* update RTC time when exiting the month and day adjustment states */
    switch (fsm_variables.month_day_state)
    {
    case MONTH_DAY_STATE_ADJUST_MONTH:
    case MONTH_DAY_STATE_ADJUST_DAY:
        rtc_set_time(clock_time);
        break;
    default:
        break;
    }
    fsm_variables.month_day_state++;
    if (fsm_variables.month_day_state == MONTH_DAY_STATE_END)
    {
        fsm_variables.month_day_state = MONTH_DAY_STATE_DISPLAY;
    }
    new_state_start();
}

/* swap through the different states in the year state */
void swap_year_state(void)
{
    /* update RTC time when exiting the year adjustment states */
    switch (fsm_variables.year_state)
    {
    case YEAR_STATE_ADJUST_YEAR:
        rtc_set_time(clock_time);
        break;
    default:
        break;
    }
    fsm_variables.year_state++;
    if (fsm_variables.year_state == YEAR_STATE_END)
    {
        fsm_variables.year_state = YEAR_STATE_DISPLAY;
    }
    new_state_start();
}

/* swap through the different states in the temperature state */
void swap_temperature_state(void)
{
    fsm_variables.temperature_state++;
    if (fsm_variables.temperature_state == TEMPERATURE_STATE_END)
    {
        fsm_variables.temperature_state = TEMPERATURE_STATE_DISPLAY_TEMPERATURE;
    }
    new_state_start();
}

/* swap through the different states in the humidity state */
void swap_humidity_state(void)
{
    fsm_variables.humidity_state++;
    if (fsm_variables.humidity_state == HUMIDITY_STATE_END)
    {
        fsm_variables.humidity_state = HUMIDITY_STATE_DISPLAY_HUMIDITY;
    }
    new_state_start();
}

/* swap through the different states in the display testing state */
void swap_testing_state(void)
{
    fsm_variables.testing_state++;
    if (fsm_variables.testing_state == TESTING_STATE_END)
    {
        fsm_variables.testing_state = TESTING_STATE_ALL_OFF;
    }
    new_state_start();
}

// ************************************************************************************************
void increment_hour(void)
{
    if (clock_time->hour < 23)
    {
        clock_time->hour++;
    }
    else
    {
        clock_time->hour = 0;
    }
}

void decrement_hour(void)
{
    if (clock_time->hour > 0)
    {
        clock_time->hour--;
    }
    else
    {
        clock_time->hour = 23;
    }
}

void increment_minute(void)
{
    if (clock_time->min < 59)
    {
        clock_time->min++;
    }
    else
    {
        clock_time->min = 0;
    }
}

void decrement_minute(void)
{
    if (clock_time->min > 0)
    {
        clock_time->min--;
    }
    else
    {
        clock_time->min = 59;
    }
}

void toggle_alarm_on_off(void)
{
    alarm_settings.enabled = !alarm_settings.enabled;
}

void increment_alarm_hour(void)
{
    if (alarm_settings.alarm_hour < 23)
    {
        alarm_settings.alarm_hour++;
    }
    else
    {
        alarm_settings.alarm_hour = 0;
    }
}

void decrement_alarm_hour(void)
{
    if (alarm_settings.alarm_hour > 0)
    {
        alarm_settings.alarm_hour--;
    }
    else
    {
        alarm_settings.alarm_hour = 23;
    }
}

void increment_alarm_min(void)
{
    if (alarm_settings.alarm_min < 59)
    {
        alarm_settings.alarm_min++;
    }
    else
    {
        alarm_settings.alarm_min = 0;
    }
}

void decrement_alarm_min(void)
{
    if (alarm_settings.alarm_min > 0)
    {
        alarm_settings.alarm_min--;
    }
    else
    {
        alarm_settings.alarm_min = 59;
    }
}

void increment_month(void)
{
    if (clock_time->mon < 12)
    {
        clock_time->mon++;
    }
    else
    {
        clock_time->mon = 1;
    }
}

void decrement_month(void)
{
    if (clock_time->mon > 1)
    {
        clock_time->mon--;
    }
    else
    {
        clock_time->mon = 12;
    }
}

void increment_day(void)
{
    if (clock_time->mday < 31)
    {
        clock_time->mday++;
    }
    else
    {
        clock_time->mday = 1;
    }
}

void decrement_day(void)
{
    if (clock_time->mday > 1)
    {
        clock_time->mday--;
    }
    else
    {
        clock_time->mday = 31;
    }
}

void increment_year(void)
{
    if (clock_time->year < MAX_YEAR)
    {
        clock_time->year++;
    }
    else
    {
        clock_time->year = MIN_YEAR;
    }
}

void decrement_year(void)
{
    if (clock_time->year > MIN_YEAR)
    {
        clock_time->year--;
    }
    else
    {
        clock_time->year = MAX_YEAR;
    }
}

void toggle_mood_light(void)
{
    static uint8_t state = 0;
    state++;
    if (state == 9)
    {
        state = 0;
    }
    // if (state)
    // {
    //     colors[0].r = 0;
    //     colors[0].g = 255;
    //     colors[0].b = 0;
    // }
    // else
    // {
    //     colors[0].r = 0;
    //     colors[0].g = 0;
    //     colors[0].b = 0;
    // }

    for (int i = 0; i < 8; i++)
    {
        led[i].r = colors[state].r;
        led[i].g = colors[state].g;
        led[i].b = colors[state].b;
    }
    ws2812_sendarray((uint8_t *)led, 8 * 3);
}

void fsm_loop(void)
{

    switch (fsm_variables.main_state)
    {
    case MAIN_STATE_HOUR_MIN:
        // run once
        if (fsm_variables.state_start == false)
        {
            fsm_variables.state_start = true;

            // run only when changing main state
            if (fsm_variables.main_state_swapped == true)
            {
                fsm_variables.main_state_swapped = false;

                // test
                // clock_time->hour = 88;
                // clock_time->min = 88;

                fsm_variables.hour_min_state = HOUR_MIN_STATE_DISPLAY;
            }
            // set button callbacks
            switch (fsm_variables.hour_min_state)
            {
            /*
            +- buttons switch between main states
            adjust button long press would go into adjust hour mode
            */
            case HOUR_MIN_STATE_DISPLAY:
                button_attach_single_click(&minus_button, decrement_main_state);
                button_attach_single_click(&plus_button, increment_main_state);
                button_attach_single_click(&adjust_button, toggle_mood_light);
                button_attach_long_press(&adjust_button, swap_hour_min_state);
                break;
            /*
            +- buttons adjust the hour
            adjust button short press goes into adjust minute mode
            */
            case HOUR_MIN_STATE_ADJUST_HOUR:
                button_attach_single_click(&minus_button, decrement_hour);
                button_attach_single_click(&plus_button, increment_hour);
                button_attach_single_click(&adjust_button, swap_hour_min_state);
                break;
            /*
            +- buttons adjust the minute
            adjust button short press goes back to hour min display mode
             */
            case HOUR_MIN_STATE_ADJUST_MIN:
                button_attach_single_click(&minus_button, decrement_minute);
                button_attach_single_click(&plus_button, increment_minute);
                button_attach_single_click(&adjust_button, swap_hour_min_state);
                break;
            default:
                break;
            }
        }
        // run forever
        seven_segment_show_hour_minute(clock_time->hour, clock_time->min);
        switch (fsm_variables.hour_min_state)
        {
            /*
            the current time is displayed with a blinking colon.
            get time from the RTC every 250ms
            */
        case HOUR_MIN_STATE_DISPLAY:
            seven_segment_flash_colon(1);
            break;
            /*
            the current time is displayed with a steady colon, but the hour portion is blinking.

            */
        case HOUR_MIN_STATE_ADJUST_HOUR:
            seven_segment_flash_digits_hours(1);
            read_rtc = 0;
            alarm_memory.adjusting_time = 1;
            break;
            /*
            the current time is displayed with a steady colon, but the minute portion is blinking.

             */
        case HOUR_MIN_STATE_ADJUST_MIN:
            seven_segment_flash_digits_minutes(1);
            read_rtc = 0;
            alarm_memory.adjusting_time = 1;
            break;
        default:
            break;
        }
        break;

    case MAIN_STATE_ALARM:
        // run once
        if (fsm_variables.state_start == false)
        {
            fsm_variables.state_start = true;
            // run only when changing main state
            if (fsm_variables.main_state_swapped == true)
            {
                load_alarm_settings();
                fsm_variables.main_state_swapped = false;
                // fsm_variables.alarm_state = ALARM_STATE_DISPLAY;
            }
            // set button callbacks
            switch (fsm_variables.alarm_state)
            {
            /*
            +- buttons switch between main states
            adjust button short press toggles the alarm on/off.
            adjust button long press would go into adjust alarm hour mode
            */
            case ALARM_STATE_DISPLAY:
                button_attach_single_click(&minus_button, decrement_main_state);
                button_attach_single_click(&plus_button, increment_main_state);
                button_attach_single_click(&adjust_button, toggle_alarm_on_off);
                button_attach_long_press(&adjust_button, swap_alarm_state);
                break;
            /*
            +- buttons adjust the alarm hour
            adjust button short press goes into adjust alarm minute mode
            */
            case ALARM_STATE_ADJUST_HOUR:
                button_attach_single_click(&minus_button, decrement_alarm_hour);
                button_attach_single_click(&plus_button, increment_alarm_hour);
                button_attach_single_click(&adjust_button, swap_alarm_state);
                break;
            /*
            +- buttons adjust the alarm minute
            adjust button short press goes back to alarm hour min display mode
             */
            case ALARM_STATE_ADJUST_MIN:
                button_attach_single_click(&minus_button, decrement_alarm_min);
                button_attach_single_click(&plus_button, increment_alarm_min);
                button_attach_single_click(&adjust_button, swap_alarm_state);
                break;
            default:
                break;
            }
        }
        // run forever
        switch (fsm_variables.alarm_state)
        {
        case ALARM_STATE_DISPLAY:
            if (alarm_settings.enabled == 0)
            {
                seven_segment_write_bytes(ALARM_OFF_DISPLAY_VALUE);
            }
            else
            {
                seven_segment_show_hour_minute(alarm_settings.alarm_hour, alarm_settings.alarm_min);
            }
            // seven_segment_show_hour_minute(alarm_settings.enabled, 0);
            break;
        case ALARM_STATE_ADJUST_HOUR:
            seven_segment_show_hour_minute(alarm_settings.alarm_hour, alarm_settings.alarm_min);
            seven_segment_flash_digits_hours(1);
            break;
        case ALARM_STATE_ADJUST_MIN:
            seven_segment_show_hour_minute(alarm_settings.alarm_hour, alarm_settings.alarm_min);
            seven_segment_flash_digits_minutes(1);
            break;
        default:
            break;
        }
        break;
    case MAIN_STATE_MONTH_DAY:
        // run once
        if (fsm_variables.state_start == false)
        {
            fsm_variables.state_start = true;
            // run only when changing main state
            if (fsm_variables.main_state_swapped == true)
            {

                fsm_variables.main_state_swapped = false;

                // test
                // clock_time->mon = 88;
                // clock_time->mday = 88;

                fsm_variables.hour_min_state = MONTH_DAY_STATE_DISPLAY;
            }
            // set button callbacks
            switch (fsm_variables.month_day_state)
            {
                /*
                +- buttons switch between main states
                adjust button long press would go into adjust month mode
                 */
            case MONTH_DAY_STATE_DISPLAY:
                button_attach_single_click(&minus_button, decrement_main_state);
                button_attach_single_click(&plus_button, increment_main_state);
                button_attach_single_click(&adjust_button, toggle_mood_light);
                button_attach_long_press(&adjust_button, swap_month_day_state);
                break;
                /*
            +- buttons adjust the month
            adjust button short press goes into adjust day mode
            */
            case MONTH_DAY_STATE_ADJUST_MONTH:
                button_attach_single_click(&minus_button, decrement_month);
                button_attach_single_click(&plus_button, increment_month);
                button_attach_single_click(&adjust_button, swap_month_day_state);
                break;
                /*
            +- buttons adjust the day
            adjust button short press goes back to month day display mode
            */
            case MONTH_DAY_STATE_ADJUST_DAY:
                button_attach_single_click(&minus_button, decrement_day);
                button_attach_single_click(&plus_button, increment_day);
                button_attach_single_click(&adjust_button, swap_month_day_state);
                break;
            default:
                break;
            }
        }
        // run forever
        seven_segment_show_month_day(clock_time->mon, clock_time->mday);
        switch (fsm_variables.month_day_state)
        {
        case MONTH_DAY_STATE_DISPLAY:
            break;
        case MONTH_DAY_STATE_ADJUST_MONTH:
            seven_segment_flash_digits_hours(1);
            read_rtc = 0;
            alarm_memory.adjusting_time = 1;
            break;
        case MONTH_DAY_STATE_ADJUST_DAY:
            seven_segment_flash_digits_minutes(1);
            read_rtc = 0;
            alarm_memory.adjusting_time = 1;
            break;
        default:
            break;
        }
        break;
    case MAIN_STATE_YEAR:
        // run once
        if (fsm_variables.state_start == false)
        {
            fsm_variables.state_start = true;
            // run only when changing main state
            if (fsm_variables.main_state_swapped == true)
            {
                fsm_variables.main_state_swapped = false;
                // test
                // clock_time->year = 1926;
                clock_time = rtc_get_time();
                if (clock_time->year < MIN_YEAR)
                {
                    clock_time->year = MIN_YEAR;
                    rtc_set_time(clock_time);
                }
                if (clock_time->year > MAX_YEAR)
                {
                    clock_time->year = MAX_YEAR;
                    rtc_set_time(clock_time);
                }
                fsm_variables.year_state = YEAR_STATE_DISPLAY;
            }
            // set button callbacks
            switch (fsm_variables.year_state)
            {
            /*
            +- buttons switch between main states
            adjust button long press would go into adjust hour mode
            */
            case YEAR_STATE_DISPLAY:
                button_attach_single_click(&minus_button, decrement_main_state);
                button_attach_single_click(&plus_button, increment_main_state);
                button_attach_single_click(&adjust_button, toggle_mood_light);
                button_attach_long_press(&adjust_button, swap_year_state);
                break;
            /*
            +- buttons adjust the hour
            adjust button short press goes into adjust minute mode
            */
            case YEAR_STATE_ADJUST_YEAR:
                button_attach_single_click(&minus_button, decrement_year);
                button_attach_single_click(&plus_button, increment_year);
                button_attach_single_click(&adjust_button, swap_year_state);
                break;
            default:
                break;
            }
        }
        // run forever
        seven_segment_show_year(100 + clock_time->year);
        switch (fsm_variables.year_state)
        {
        case YEAR_STATE_DISPLAY:
            break;
        case YEAR_STATE_ADJUST_YEAR:
            seven_segment_flash_all_digits(1);
            read_rtc = 0;
            alarm_memory.adjusting_time = 1;
            break;
        default:
            break;
        }
        break;
    case MAIN_STATE_TEMPERATURE:
        // run once
        if (fsm_variables.state_start == false)
        {
            fsm_variables.state_start = true;
            // run only when changing main state
            if (fsm_variables.main_state_swapped == true)
            {
                dht_values.temperature = -88.8;
                fsm_variables.main_state_swapped = false;
            }
            // set button callbacks
            switch (fsm_variables.temperature_state)
            {
                /*
                +- buttons switch between main states
                */
            case TEMPERATURE_STATE_DISPLAY_TEMPERATURE:
                button_attach_single_click(&minus_button, decrement_main_state);
                button_attach_single_click(&plus_button, increment_main_state);
                button_attach_single_click(&adjust_button, toggle_mood_light);
                button_attach_long_press(&adjust_button, NULL);
                break;
            default:
                break;
            }
        }
        // run forever
        // Read from sensor
        if (t_millis - prev_millis_read_dht > 3000)
        {
            prev_millis_read_dht = t_millis;
            cli();
            dht22_measure_values(&dht_values);
            sei();
        }
        switch (fsm_variables.temperature_state)
        {
        case TEMPERATURE_STATE_DISPLAY_TEMPERATURE:
        default:
            seven_segment_show_temperature(dht_values.temperature);
        }
        break;
    case MAIN_STATE_HUMIDITY:
        // run once
        if (fsm_variables.state_start == false)
        {
            fsm_variables.state_start = true;
            // run only when changing main state
            if (fsm_variables.main_state_swapped == true)
            {
                dht_values.humidity = 88.8;
                fsm_variables.main_state_swapped = false;
            }
            // set button callbacks
            switch (fsm_variables.humidity_state)
            {
            /*
            +- buttons switch between main states
            */
            case HUMIDITY_STATE_DISPLAY_HUMIDITY:
                button_attach_single_click(&minus_button, decrement_main_state);
                button_attach_single_click(&plus_button, increment_main_state);
                button_attach_single_click(&adjust_button, toggle_mood_light);
                button_attach_long_press(&adjust_button, NULL);
                break;
            default:
                break;
            }
        }
        // run forever
        if (t_millis - prev_millis_read_dht > 3000)
        {
            prev_millis_read_dht = t_millis;
            cli();
            dht22_measure_values(&dht_values);
            sei();
        }
        switch (fsm_variables.humidity_state)
        {
        case HUMIDITY_STATE_DISPLAY_HUMIDITY:
        default:
            seven_segment_show_humidity(dht_values.humidity);
        }
        break;
    // 7 segment test on and off
    case MAIN_STATE_TESTING:
        // run once
        if (fsm_variables.state_start == false)
        {
            fsm_variables.state_start = true;
            // run only when changing main state
            if (fsm_variables.main_state_swapped == true)
            {
                fsm_variables.main_state_swapped = false;
                fsm_variables.testing_state = TESTING_STATE_ALL_ON;
            }
            button_attach_single_click(&minus_button, decrement_main_state);
            button_attach_single_click(&plus_button, increment_main_state);
            // attach adjust button
            button_attach_single_click(&adjust_button, swap_testing_state);
        }
        // run forever
        switch (fsm_variables.testing_state)
        {
        // 7 segment all off
        case TESTING_STATE_ALL_OFF:
            seven_segment_clear_all();
            fsm_variables.display_updated = true;
            break;
        // 7 segment all on
        case TESTING_STATE_ALL_ON:
            seven_segment_set_all();
            fsm_variables.display_updated = true;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}