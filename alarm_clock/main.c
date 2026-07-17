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

#include "alarm.h"
#include "fsm.h"
#include "notes.h"

extern const uint8_t digit_values[17];

/*

*/

uint32_t t_millis;
uint32_t prev_millis_read_rtc;

Button minus_button, adjust_button, plus_button;
struct tm *clock_time;
uint8_t read_rtc;
sensor_values_t sensor_values;
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

fsm_variables_t fsm_variables;

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
    // piezo_stop_sequence();
    load_alarm_notes();

    piezo_stop_sequence();
    /*
    Alarm test code
    set the RTC to a certain time and set alarm value to a certain time
     */
    clock_time->sec = 88;
    clock_time->min = 88;
    clock_time->hour = 88;
    // rtc_set_time(clock_time);

    // rtc_set_time_s(5, 59, 55);
    // alarm_settings.alarm_hour = 6;
    // alarm_settings.alarm_min = 0;
    // update_alarm_settings();

    // ************************************************************************************************

    while (1)
    {

        t_millis = get_millis();
        read_rtc = 1;
        alarm_memory.adjusting_time = 0;
        /*
        main FSM
        */
        fsm_loop();

        // alarm checking loop
        // check if alarm is enabled
        alarm_loop();

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