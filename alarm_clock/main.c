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
// #include "src/rtc.h"
// #include "src/serial.h"

typedef enum
{
    STATE_HOUR_MIN,
    STATE_ALARM,
    STATE_MONTH_DAY,
    STATE_YEAR,
    STATE_TEMPERATURE,
    STATE_HUMIDITY,
    STATE_TESTING,
} alarm_clock_state_t;

typedef enum
{
    STATE_HOUR_MIN_DISPLAY,
    STATE_HOUR_MIN_ADJUST_HOUR,
    STATE_HOUR_MIN_ADJUST_MIN,
} state_hour_min_t;

typedef enum
{
    STATE_ALARM_DISPLAY, // display OFF if disabled, display alarm time if enabled
    STATE_ALARM_ADJUST_HOUR,
    STATE_ALARM_ADJUST_MIN,
    STATE
} state_alarm_t;

typedef enum
{
    STATE_MONTH_DAY_DISPLAY,
    STATE_MONTH_DAY_ADJUST_MONTH,
    STATE_MONTH_DAY_ADJUST_DAY,
} state_month_day_t;

typedef enum
{
    STATE_YEAR_DISPLAY,
    STATE_YEAR_ADJUST_YEAR,
} state_year_t;

typedef enum
{
    STATE_TEMPERATURE_DISPLAY_TEMPERATURE,
} state_temperature_t;

typedef enum
{
    STATE_HUMIDITY_DISPLAY_HUMIDITY,
} state_humidity_t;

typedef enum
{
    STATE_TESTING_ALL_ON,
    STATE_TESTING_ALL_OFF,
} state_testing_t;

// devices
#define BTNS_PORT PORTC
#define BTNS_DDR DDRC
#define BTNS_PIN PINC
#define BTN_1_PIN_NUM PC1 // down
#define BTN_2_PIN_NUM PC2 // reset
#define BTN_3_PIN_NUM PC3 // up
Button minus_button, adjust_button, plus_button;

// interrupts
ISR(TIMER0_COMPA_vect)
{
    millis_timer_ISR_loop();
    // piezo_loop_ISR();
    seven_segment_loop_isr();
}

ISR(TIMER2_COMPA_vect)
{
}

// trigger a CTC every 1ms
void timer2_init(void)
{
    // WGM = 010, CTC mode
    // COM2A = 00, OC2A disconnected
    // CS2 = 100, prescaler 1/256, 16us per increment
    // OCR2A = 250 - 1 = 249, 250 * 16us = 4ms
    // enable OCIE2A, output compare match A interrupt handle
    TCCR2A = 0;
    TCCR2B = 0;
    TCCR2A |= _BV(WGM21);
    TCCR2B |= _BV(CS22) | _BV(CS21);
    TIMSK2 |= _BV(OCIE2A);
    OCR2A = 249;
    sei();
}

/*
used to set the contents of the display just ONCE until the value changed.
*/
volatile uint8_t state_updated;

uint32_t t_millis, prev_millis1, prev_millis2;
uint8_t set_clear;
void flash_colon_loop(void)
{
    if (t_millis - prev_millis2 > 500)
    {
        prev_millis2 = t_millis;
        if (set_clear)
        {
            seven_segment_set_colon(1);
        }
        else
        {
            seven_segment_set_colon(0);
        }
        set_clear = !set_clear;
    }
}

volatile alarm_clock_state_t alarm_clock_state;
uint8_t state_testing_setup;
volatile state_testing_t state_testing;

void set_setup_false()
{
    switch (alarm_clock_state)
    {
    case STATE_TESTING:
        state_testing_setup = false;
        break;
    default:
        break;
    }
}

void increment_alarm_clock_state(void)
{
    if (alarm_clock_state == STATE_TESTING)
    {
        alarm_clock_state = STATE_HOUR_MIN;
    }
    else
    {
        alarm_clock_state++;
    }
    set_setup_false();
}

void decrement_alarm_clock_state(void)
{
    if (alarm_clock_state == STATE_HOUR_MIN)
    {
        alarm_clock_state = STATE_TESTING;
    }
    else
    {
        alarm_clock_state--;
    }
    set_setup_false();
}

void toggle_testing_state(void)
{
    if (state_testing == STATE_TESTING_ALL_ON)
    {
        state_testing = STATE_TESTING_ALL_OFF;
        state_updated = false;
    }
    else
    {
        state_testing = STATE_TESTING_ALL_ON;
        state_updated = false;
    }
}

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
    button_attach_single_click(&minus_button, decrement_alarm_clock_state);
    button_attach_single_click(&plus_button, increment_alarm_clock_state);

    // initialize seven segment
    seven_segment_init();
    seven_segment_clear_all();

    // initialize RTC
    // twi_init_master();
    // rtc_init();
    // initialize piezo
    // piezo_init(10);

    while (1)
    {

        switch (alarm_clock_state)
        {
        case STATE_HOUR_MIN:
            /* code */
            break;
        case STATE_ALARM:
            /* code */
            break;
        case STATE_MONTH_DAY:
            /* code */
            break;
        case STATE_YEAR:
            /* code */
            break;
        case STATE_TEMPERATURE:
            /* code */
            break;
        case STATE_HUMIDITY:
            /* code */
            break;
        case STATE_TESTING:

            if (state_testing_setup == false)
            {
                state_testing_setup = true;
                // attach adjust button
                button_attach_single_click(&adjust_button, toggle_testing_state);
                state_testing = STATE_TESTING_ALL_ON;
                state_updated = false;
            }
            switch (state_testing)
            {
            case STATE_TESTING_ALL_ON:
                if (state_updated == false)
                {
                    seven_segment_set_all();
                    state_updated = true;
                }
                break;
            case STATE_TESTING_ALL_OFF:
                if (state_updated == false)
                {
                    seven_segment_clear_all();
                    state_updated = true;
                }
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        button_ISR_tick(&minus_button);
        button_ISR_tick(&adjust_button);
        button_ISR_tick(&plus_button);
    }
}