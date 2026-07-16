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
#include "src/piezo.h"
#include "src/seven_segment.h"
#include "src/rtc.h"
// #include "src/serial.h"
#include "src/DHT.h"
#include "src/states.h"

extern const uint8_t digit_values[17];

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
uint32_t prev_millis_read_rtc;

Button minus_button, adjust_button, plus_button;
struct tm *clock_time;
uint8_t read_rtc;

// DHT11
typedef struct
{
    double humidity, temperature;
} sensor_values_t;
sensor_values_t sensor_values;

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
} alarm_memory_t;
alarm_memory_t alarm_memory;
button_action_t prev_adjust_btn_callback, prev_plus_btn_callback, prev_minus_btn_callback;
uint8_t prev_btn_callbacks_saved = 0;

uint8_t ALARM_OFF_DISPLAY_VALUE[4];

// interrupts
ISR(TIMER0_COMPA_vect)
{
    millis_timer_ISR_loop();
    piezo_loop_ISR();
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
fsm_variables_t fsm_variables;

void load_alarm_settings(void);
void update_alarm_settings(void);

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

/*
alarm functions
*/

void trim_alarm_values(void)
{
    if (alarm_settings.alarm_hour > 23)
    {
        alarm_settings.alarm_hour = 0;
    }
    if (alarm_settings.alarm_min > 59)
    {
        alarm_settings.alarm_min = 0;
    }
}

void compute_alarm_time(void)
{
    alarm_memory.start_hour = alarm_settings.alarm_hour;
    alarm_memory.start_min = alarm_settings.alarm_min;
    alarm_memory.start_mins_since_midnight = alarm_memory.start_hour * 60 + alarm_memory.start_min;
    alarm_memory.duration_mins = ALARM_DURATION_MINS;
    alarm_memory.end_mins_since_midnight = alarm_memory.start_mins_since_midnight + alarm_memory.duration_mins;

    alarm_memory.end_hour = alarm_memory.end_mins_since_midnight / 60;
    if (alarm_memory.end_hour > 23)
    {
        alarm_memory.end_hour = 0;
    }
    alarm_memory.end_min = alarm_memory.end_mins_since_midnight % 60;
}

void load_alarm_settings(void)
{
    eeprom_read_block((void *)&alarm_settings, (const void *)EEPROM_ALARM_SETTINGS_ADDR, sizeof(alarm_settings_t));
    trim_alarm_values();
    compute_alarm_time();
}

void update_alarm_settings(void)
{
    trim_alarm_values();
    eeprom_update_block((const void *)&alarm_settings, (void *)EEPROM_ALARM_SETTINGS_ADDR, sizeof(alarm_settings_t));
    compute_alarm_time();
}

void dismiss_alarm(void);

/**
 *
 */
void set_alarm_dismiss_button_callbacks()
{
    prev_adjust_btn_callback = button_get_single_click_action(&adjust_button);
    prev_plus_btn_callback = button_get_single_click_action(&plus_button);
    prev_minus_btn_callback = button_get_single_click_action(&minus_button);
    prev_btn_callbacks_saved = 1;
    button_attach_single_click(&adjust_button, dismiss_alarm);
    button_attach_single_click(&plus_button, dismiss_alarm);
    button_attach_single_click(&minus_button, dismiss_alarm);
}

/**
 * @brief restore all button callbacks
 */
void restore_button_callbacks(void)
{
    if (prev_btn_callbacks_saved)
    {
        prev_btn_callbacks_saved = 0;
        button_attach_single_click(&adjust_button, prev_adjust_btn_callback);
        button_attach_single_click(&plus_button, prev_plus_btn_callback);
        button_attach_single_click(&minus_button, prev_minus_btn_callback);
    }
}

/**
 * @brief dismiss alarm button callback
 *
 * dismiss alarm and restore the previous adjust button callback
 */
void dismiss_alarm(void)
{
    alarm_memory.dismissed = 1;

    restore_button_callbacks();
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

// ************************************************************************************************

int main()
{
    // alarm_settings = (alarm_settings_t *)malloc(sizeof(alarm_settings_t));
    // alarm_memory = (alarm_memory_t *)malloc(sizeof(alarm_memory_t));
    // sensor_values = (sensor_values_t *)malloc(sizeof(sensor_values_t));
    // clock_time = (struct tm *)malloc(sizeof(struct tm));
    // fsm_variables = (fsm_variables_t *)malloc(sizeof(fsm_variables_t));

    ALARM_OFF_DISPLAY_VALUE[0] = 0;
    ALARM_OFF_DISPLAY_VALUE[1] = digit_values[0x0];
    ALARM_OFF_DISPLAY_VALUE[2] = digit_values[0xf];
    ALARM_OFF_DISPLAY_VALUE[3] = digit_values[0xf];

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
    piezo_init(60);
    fsm_variables.main_state_swapped = true;

    load_alarm_settings();

    /*
    load tune into memory
    */
    // the maiden's prayer
    uint16_t note_ms = 250;
    Note notes[] = {
        {.ocr1a_value = OCR1A_NOTE_AS5, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_DS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_G6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_DS7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_G7, .duration = note_ms * 5},
        {.ocr1a_value = OCR1A_NOTE_F7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_F7, .duration = note_ms * 2},
        {.ocr1a_value = OCR1A_NOTE_DS7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_DS7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_D7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_C7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_C7, .duration = note_ms * 6}, // 13

        {.ocr1a_value = OCR1A_NOTE_REST, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS5, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_D6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_F6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_GS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_D7, .duration = note_ms * 5},
        {.ocr1a_value = OCR1A_NOTE_C7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_C7, .duration = note_ms * 2},
        {.ocr1a_value = OCR1A_NOTE_AS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_GS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_G6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_G6, .duration = note_ms * 6}, // 14

        {.ocr1a_value = OCR1A_NOTE_REST, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS5, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_DS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_G6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_DS7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_G7, .duration = note_ms * 5},
        {.ocr1a_value = OCR1A_NOTE_F7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_F7, .duration = note_ms * 2},
        {.ocr1a_value = OCR1A_NOTE_DS7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_DS7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_D7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_C7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_C7, .duration = note_ms * 6}, // 14

        {.ocr1a_value = OCR1A_NOTE_REST, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_D6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_F6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_GS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_D7, .duration = note_ms * 1}, // 6

        {.ocr1a_value = OCR1A_NOTE_REST, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_DS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_G6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_DS7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_G7, .duration = note_ms * 1}, // 6

        {.ocr1a_value = OCR1A_NOTE_AS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_A6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_G7, .duration = note_ms * 2},
        {.ocr1a_value = OCR1A_NOTE_F7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_DS7, .duration = note_ms * 6}, // 6

        {.ocr1a_value = OCR1A_NOTE_REST, .duration = note_ms * 10}, // 1

    };

    for (int i = 0; i < 60 + 1; i++)
    {
        piezo_add_note(&notes[i]);
    }

    /*
    Alarm test code
    set the RTC to a certain time and set alarm value to a certain time
     */
    // clock_time->sec = 55;
    // clock_time->min = 59;
    // clock_time->hour = 5;
    // rtc_set_time(clock_time);

    // rtc_set_time_s(5, 59, 55);
    // alarm_settings.alarm_hour = 6;
    // alarm_settings.alarm_min = 0;
    // update_alarm_settings();

    // ************************************************************************************************

    while (1)
    {
        /*
        main FSM
        */
        t_millis = get_millis();
        read_rtc = 1;
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
                    button_attach_single_click(&adjust_button, NULL);
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
                break;
                /*
                the current time is displayed with a steady colon, but the minute portion is blinking.

                 */
            case HOUR_MIN_STATE_ADJUST_MIN:
                seven_segment_flash_digits_minutes(1);
                read_rtc = 0;
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
                    button_attach_single_click(&adjust_button, NULL);
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
                break;
            case MONTH_DAY_STATE_ADJUST_DAY:
                seven_segment_flash_digits_minutes(1);
                read_rtc = 0;
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
                    button_attach_single_click(&adjust_button, NULL);
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
                    sensor_values.temperature = -88.8;
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
                    button_attach_single_click(&adjust_button, NULL);
                    button_attach_long_press(&adjust_button, NULL);
                    break;
                default:
                    break;
                }
            }
            // run forever
            // Read from sensor
            DHT_Read(&sensor_values.temperature, &sensor_values.humidity);
            switch (fsm_variables.temperature_state)
            {
            case TEMPERATURE_STATE_DISPLAY_TEMPERATURE:
            default:
                seven_segment_show_temperature(sensor_values.temperature);
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
                    sensor_values.humidity = 88.8;
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
                    button_attach_single_click(&adjust_button, NULL);
                    button_attach_long_press(&adjust_button, NULL);
                    break;
                default:
                    break;
                }
            }
            // run forever
            DHT_Read(&sensor_values.temperature, &sensor_values.humidity);
            switch (fsm_variables.humidity_state)
            {
            case HUMIDITY_STATE_DISPLAY_HUMIDITY:
            default:
                seven_segment_show_humidity(sensor_values.humidity);
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

        // alarm checking loop
        // check if alarm is enabled
        if (alarm_settings.enabled && read_rtc)
        {
            // check if the time is within the alarm time
            alarm_memory.cur_mins_since_midnight = clock_time->hour * 60 + clock_time->min;
            // if the alarm start and end times are within the same day
            bool same_day_alarm_triggered = (alarm_memory.start_mins_since_midnight < alarm_memory.end_mins_since_midnight) &&
                                            (alarm_memory.cur_mins_since_midnight >= alarm_memory.start_mins_since_midnight &&
                                             alarm_memory.cur_mins_since_midnight < alarm_memory.end_mins_since_midnight);
            // if the alarm start and end times are on different days.
            bool different_day_alarm_triggered = (alarm_memory.start_mins_since_midnight > alarm_memory.end_mins_since_midnight) &&
                                                 (alarm_memory.cur_mins_since_midnight >= alarm_memory.start_mins_since_midnight ||
                                                  alarm_memory.cur_mins_since_midnight < alarm_memory.end_mins_since_midnight);

            if (same_day_alarm_triggered || different_day_alarm_triggered)
            {
                alarm_memory.within_alarm_period = 1;
                if (alarm_memory.prev_triggered == 0)
                {
                    alarm_memory.prev_triggered = 1;
                    alarm_memory.dismissed = 0;
                    // set button callbacks
                    set_alarm_dismiss_button_callbacks();
                }
            }
            else
            {
                if (alarm_memory.prev_triggered == 1)
                {
                    alarm_memory.prev_triggered = 0;
                    restore_button_callbacks();
                }
                alarm_memory.within_alarm_period = 0;
            }

            // check if the time is within the alarm time and n minutes after the alarm time
            /*
            21:00 - 21:05
            21:58 - 22:03
            23:58 - 00:03

            convert hour min times into minutes since midnight
            total_mins = hours * 60 + mins
            total_mins += timedelta
            end_hours = timedelta / 60 % 24;
            end_minutes = timedelta % 60;

            */

            /*
            if alarm is outside alarm period or has been dismissed, stop playing the alarm.
            else if the time is within the alarm time period, and if the alarm has not been manually dismissed,
            start playing the alarm notes.
            */
            if (!alarm_memory.within_alarm_period || alarm_memory.dismissed)
            {
                piezo_stop_sequence();
            }
            else if (alarm_memory.within_alarm_period && !alarm_memory.dismissed && !piezo_is_playing())
            {
                piezo_play_sequence();
            }
        }

        // read RTC
        if (t_millis - prev_millis_read_rtc > 500 && read_rtc)
        {
            prev_millis_read_rtc = t_millis;
            clock_time = rtc_get_time();
        }
        // button tick loops
        button_loop(&minus_button);
        button_loop(&adjust_button);
        button_loop(&plus_button);
        // seven segment flashing loop
        seven_segment_flashing_loop(t_millis);
    }
}