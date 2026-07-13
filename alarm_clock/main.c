// ************************************************************************************************
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include "src/config.h"
#include "src/millis_micros.h"
#include "src/gpio.h"
#include "src/button.h"
// #include "src/piezo.h"
#include "src/seven_segment.h"
#include "src/rtc.h"
// #include "src/serial.h"
#include "src/states.h"
/*
src/rtc.c src/twi.c src/twi-lowlevel.c
*/

#define MIN_YEAR 1900
#define MAX_YEAR 1999

// buttons
#define BTNS_PORT PORTC
#define BTNS_DDR DDRC
#define BTNS_PIN PINC
#define BTN_1_PIN_NUM PC1 // down
#define BTN_2_PIN_NUM PC2 // reset
#define BTN_3_PIN_NUM PC3 // up

uint32_t t_millis;
Button minus_button, adjust_button, plus_button;
struct tm *clock_time;

uint32_t prev_millis_read_rtc;

// interrupts
ISR(TIMER0_COMPA_vect)
{
    millis_timer_ISR_loop();
    // piezo_loop_ISR();
    seven_segment_loop_isr();
}

// ISR(TIMER2_COMPA_vect)
// {
// }

// // initialize Time2 to trigger a CTC every 4ms
// void timer2_init(void)
// {
//     // WGM = 010, CTC mode
//     // COM2A = 00, OC2A disconnected
//     // CS2 = 100, prescaler 1/256, 16us per increment
//     // OCR2A = 250 - 1 = 249, 250 * 16us = 4ms
//     // enable OCIE2A, output compare match A interrupt handle
//     TCCR2A = 0;
//     TCCR2B = 0;
//     TCCR2A |= _BV(WGM21);
//     TCCR2B |= _BV(CS22) | _BV(CS21);
//     TIMSK2 |= _BV(OCIE2A);
//     OCR2A = 249;
//     sei();
// }

// state updated variable used to run code when switching to a new state
volatile uint8_t state_start;
volatile uint8_t main_state_swapped;
volatile uint8_t display_updated;
volatile alarm_clock_main_state_t main_state;

/**
 * set the state_start and display_updated flags false when a new main FSM state
 * is entered.
 */
void new_state_start()
{
    state_start = false;
    display_updated = false;
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
    if (main_state == MAIN_STATE_TESTING)
    {
        main_state = MAIN_STATE_HOUR_MIN;
    }
    else
    {
        main_state++;
    }
    main_state_swapped = true;
    new_state_start();
}

/**
 * button callback to decrement through all main alarm clock FSM states.
 */
void decrement_main_state(void)
{
    if (main_state == MAIN_STATE_HOUR_MIN)
    {
        main_state = MAIN_STATE_TESTING;
    }
    else
    {
        main_state--;
    }
    main_state_swapped = true;
    new_state_start();
}

/**
 * Variables for switching the substates of each main FSM state
 */
volatile hour_min_state_t hour_min_state;
volatile alarm_state_t alarm_state;
volatile month_day_state_t month_day_state;
volatile year_state_t year_state;
volatile temperature_state_t temperature_state;
volatile humidity_state_t humidity_state;
volatile testing_state_t testing_state;

/* swap through the different states in the hour minute state */
void swap_hour_min_state(void)
{
    /* update RTC time when exiting the hour and minute adjustment states */
    switch (hour_min_state)
    {
    case HOUR_MIN_STATE_ADJUST_HOUR:

    case HOUR_MIN_STATE_ADJUST_MIN:
        clock_time->sec = 0;
        rtc_set_time(clock_time);
        break;
    default:
        break;
    }
    hour_min_state++;
    if (hour_min_state == HOUR_MIN_STATE_END)
    {
        hour_min_state = HOUR_MIN_STATE_DISPLAY;
    }
    new_state_start();
}

/* swap through the different states in the alarm state */
void swap_alarm_state(void)
{
    alarm_state++;
    if (alarm_state == ALARM_STATE_END)
    {
        alarm_state = ALARM_STATE_DISPLAY;
    }
    new_state_start();
}

/* swap through the different states in the month day state */
void swap_month_day_state(void)
{
    /* update RTC time when exiting the month and day adjustment states */
    switch (month_day_state)
    {
    case MONTH_DAY_STATE_ADJUST_MONTH:
    case MONTH_DAY_STATE_ADJUST_DAY:
        rtc_set_time(clock_time);
        break;
    default:
        break;
    }
    month_day_state++;
    if (month_day_state == MONTH_DAY_STATE_END)
    {
        month_day_state = MONTH_DAY_STATE_DISPLAY;
    }
    new_state_start();
}

/* swap through the different states in the year state */
void swap_year_state(void)
{
    /* update RTC time when exiting the year adjustment states */
    switch (year_state)
    {
    case YEAR_STATE_ADJUST_YEAR:
        rtc_set_time(clock_time);
        break;
    default:
        break;
    }
    year_state++;
    if (year_state == YEAR_STATE_END)
    {
        year_state = YEAR_STATE_DISPLAY;
    }
    new_state_start();
}

/* swap through the different states in the temperature state */
void swap_temperature_state(void)
{
    temperature_state++;
    if (temperature_state == TEMPERATURE_STATE_END)
    {
        temperature_state = TEMPERATURE_STATE_DISPLAY_TEMPERATURE;
    }
    new_state_start();
}

/* swap through the different states in the humidity state */
void swap_humidity_state(void)
{
    humidity_state++;
    if (humidity_state == HUMIDITY_STATE_END)
    {
        humidity_state = HUMIDITY_STATE_DISPLAY_HUMIDITY;
    }
    new_state_start();
}

/* swap through the different states in the display testing state */
void swap_testing_state(void)
{
    testing_state++;
    if (testing_state == TESTING_STATE_END)
    {
        testing_state = TESTING_STATE_ALL_OFF;
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

// ************************************************************************************************

int main()
{
    // initialize timers
    millis_timer_init();
    // timer2_init
    // initialize buttons
    minus_button.button_pin.ddr = &BTNS_DDR;
    minus_button.button_pin.port = &BTNS_PORT;
    minus_button.button_pin.pin = &BTNS_PIN;
    minus_button.button_pin.pin_num = BTN_1_PIN_NUM;
    adjust_button.button_pin.ddr = &BTNS_DDR;
    adjust_button.button_pin.port = &BTNS_PORT;
    adjust_button.button_pin.pin = &BTNS_PIN;
    adjust_button.button_pin.pin_num = BTN_2_PIN_NUM;
    plus_button.button_pin.ddr = &BTNS_DDR;
    plus_button.button_pin.port = &BTNS_PORT;
    plus_button.button_pin.pin = &BTNS_PIN;
    plus_button.button_pin.pin_num = BTN_3_PIN_NUM;
    button_setup(&minus_button);
    button_setup(&adjust_button);
    button_setup(&plus_button);
    /*
    button callbacks
    left button is minus button
    center button is adjust button
    right button is plus button


    when in any mode,
    short press minus button goes to previous mode
    short press plus button goes to next mode

    in time mode,

    */
    button_attach_single_click(&minus_button, decrement_main_state);
    button_attach_single_click(&plus_button, increment_main_state);

    // initialize seven segment
    seven_segment_init();
    seven_segment_clear_all();

    // initialize RTC
    twi_init_master();
    rtc_init();
    // initialize piezo
    // piezo_init(10);
    main_state_swapped = true;
    // ************************************************************************************************

    while (1)
    {
        /*
        main FSM
        */
        t_millis = get_millis();
        switch (main_state)
        {
        case MAIN_STATE_HOUR_MIN:
            // run once
            if (state_start == false)
            {
                state_start = true;

                // run only when changing main state
                if (main_state_swapped == true)
                {
                    main_state_swapped = false;

                    // test
                    // clock_time->hour = 88;
                    // clock_time->min = 88;

                    hour_min_state = HOUR_MIN_STATE_DISPLAY;
                }
                // set button callbacks
                switch (hour_min_state)
                {
                /*
                +- buttons switch between main states
                adjust button long press would go into adjust hour mode
                */
                case HOUR_MIN_STATE_DISPLAY:
                    button_attach_single_click(&minus_button, decrement_main_state);
                    button_attach_single_click(&plus_button, increment_main_state);
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
            switch (hour_min_state)
            {
                /*
                the current time is displayed with a blinking colon.
                get time from the RTC every 250ms
                */
            case HOUR_MIN_STATE_DISPLAY:
                seven_segment_flash_colon(1);
                if (t_millis - prev_millis_read_rtc > 250)
                {
                    prev_millis_read_rtc = t_millis;
                    clock_time = rtc_get_time();
                }
                break;
                /*
                the current time is displayed with a steady colon, but the hour portion is blinking.

                */
            case HOUR_MIN_STATE_ADJUST_HOUR:
                seven_segment_flash_digits_hours(1);
                break;
                /*
                the current time is displayed with a steady colon, but the minute portion is blinking.

                 */
            case HOUR_MIN_STATE_ADJUST_MIN:
                seven_segment_flash_digits_minutes(1);
                break;
            default:
                break;
            }
            break;
        case MAIN_STATE_ALARM:
            // run once
            if (state_start == false)
            {
                state_start = true;
                // run only when changing main state
                if (main_state_swapped == true)
                {
                    main_state_swapped = false;
                }
                // set button callbacks
                switch (alarm_state)
                {
                case ALARM_STATE_DISPLAY:
                    break;
                case ALARM_STATE_ADJUST_HOUR:
                    break;
                case ALARM_STATE_ADJUST_MIN:
                    break;
                default:
                    break;
                }
            }
            // run forever
            switch (alarm_state)
            {
            case ALARM_STATE_DISPLAY:
                break;
            case ALARM_STATE_ADJUST_HOUR:

                break;
            case ALARM_STATE_ADJUST_MIN:

                break;
            default:
                break;
            }
            break;
        case MAIN_STATE_MONTH_DAY:
            // run once
            if (state_start == false)
            {
                state_start = true;
                // run only when changing main state
                if (main_state_swapped == true)
                {

                    main_state_swapped = false;

                    // test
                    // clock_time->mon = 88;
                    // clock_time->mday = 88;

                    hour_min_state = MONTH_DAY_STATE_DISPLAY;
                }
                // set button callbacks
                switch (month_day_state)
                {
                    /*
                    +- buttons switch between main states
                    adjust button long press would go into adjust month mode
                     */
                case MONTH_DAY_STATE_DISPLAY:
                    button_attach_single_click(&minus_button, decrement_main_state);
                    button_attach_single_click(&plus_button, increment_main_state);
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
            switch (month_day_state)
            {
            case MONTH_DAY_STATE_DISPLAY:
                if (t_millis - prev_millis_read_rtc > 1000)
                {
                    prev_millis_read_rtc = t_millis;
                    clock_time = rtc_get_time();
                }
                break;
            case MONTH_DAY_STATE_ADJUST_MONTH:
                seven_segment_flash_digits_hours(1);
                break;
            case MONTH_DAY_STATE_ADJUST_DAY:
                seven_segment_flash_digits_minutes(1);
                break;
            default:
                break;
            }
            break;
        case MAIN_STATE_YEAR:
            // run once
            if (state_start == false)
            {
                state_start = true;
                // run only when changing main state
                if (main_state_swapped == true)
                {
                    main_state_swapped = false;
                    // test
                    // clock_time->year = 1926;
                    // rtc_set_time(clock_time);
                    year_state = YEAR_STATE_DISPLAY;
                }
                // set button callbacks
                switch (year_state)
                {
                /*
                +- buttons switch between main states
                adjust button long press would go into adjust hour mode
                */
                case YEAR_STATE_DISPLAY:
                    button_attach_single_click(&minus_button, decrement_main_state);
                    button_attach_single_click(&plus_button, increment_main_state);
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
            switch (year_state)
            {
            case YEAR_STATE_DISPLAY:

                if (t_millis - prev_millis_read_rtc > 1000)
                {
                    prev_millis_read_rtc = t_millis;
                    clock_time = rtc_get_time();
                    // if (clock_time->year < MIN_YEAR)
                    // {
                    //     clock_time->year = MIN_YEAR;
                    //     rtc_set_time(clock_time);
                    // }
                    // if (clock_time->year > MAX_YEAR - 1)
                    // {
                    //     clock_time->year = MAX_YEAR - 1;
                    //     rtc_set_time(clock_time);
                    // }
                }
                break;
            case YEAR_STATE_ADJUST_YEAR:
                seven_segment_flash_all_digits(1);
                break;
            default:
                break;
            }
            break;
        case MAIN_STATE_TEMPERATURE:
            // run once
            if (state_start == false)
            {
                state_start = true;
                // run only when changing main state
                if (main_state_swapped == true)
                {

                    main_state_swapped = false;
                }
                // set button callbacks
                switch (temperature_state)
                {
                case TEMPERATURE_STATE_DISPLAY_TEMPERATURE:
                    break;

                default:
                    break;
                }
            }
            // run forever
            switch (temperature_state)
            {
            case TEMPERATURE_STATE_DISPLAY_TEMPERATURE:
                break;
            default:
                break;
            }
            break;
        case MAIN_STATE_HUMIDITY:
            // run once
            if (state_start == false)
            {
                state_start = true;
                // run only when changing main state
                if (main_state_swapped == true)
                {

                    main_state_swapped = false;
                }
                // set button callbacks
                switch (alarm_state)
                {
                case ALARM_STATE_DISPLAY:
                    break;
                case ALARM_STATE_ADJUST_HOUR:
                    break;
                case ALARM_STATE_ADJUST_MIN:
                    break;
                default:
                    break;
                }
            }
            // run forever
            switch (humidity_state)
            {
            case HUMIDITY_STATE_DISPLAY_HUMIDITY:
                break;
            default:
                break;
            }
            break;
        // 7 segment test on and off
        case MAIN_STATE_TESTING:
            // run once
            if (state_start == false)
            {
                state_start = true;
                // run only when changing main state
                if (main_state_swapped == true)
                {
                    main_state_swapped = false;
                    testing_state = TESTING_STATE_ALL_ON;
                }
                // attach adjust button
                button_attach_single_click(&adjust_button, swap_testing_state);
            }
            // run forever
            switch (testing_state)
            {
            // 7 segment all off
            case TESTING_STATE_ALL_OFF:
                seven_segment_clear_all();
                display_updated = true;
                break;
            // 7 segment all on
            case TESTING_STATE_ALL_ON:
                seven_segment_set_all();
                display_updated = true;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }

        // button tick loops
        button_loop(&minus_button);
        button_loop(&adjust_button);
        button_loop(&plus_button);
        // seven segment flashing loop
        seven_segment_flashing_loop(t_millis);
    }
}