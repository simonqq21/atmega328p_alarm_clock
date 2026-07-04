#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include "src/seven_segment.h"
#include <util/delay.h>

extern uint8_t digit_values[];

uint32_t prev_millis;
uint32_t t_millis, prev_millis1, prev_millis2;

int hour, minute;
int month, day;
int year;
float temperature_celsius, humidity;

ISR(TIMER0_COMPA_vect)
{
    millis_timer_ISR_loop();
    // seven_segment_loop_isr();
}

// ISR(TIMER0_COMPB_vect)
// {
//     seven_segment_loop_isr();
// }

ISR(TIMER2_COMPA_vect)
{
    seven_segment_loop_isr();
}

// trigger a CTC every 1ms
void timer2_init(void)
{
    // WGM = 010, CTC mode
    // COM2A = 00, OC2A disconnected
    // CS2 = 100, prescaler 1/64, 4us per increment
    // OCR2A = 250 - 1 = 249, 250 * 4us = 1ms
    // enable OCIE2A, output compare match A interrupt handle
    TCCR2A |= _BV(WGM21);
    TCCR2B |= _BV(CS22);
    // TCCR2B |= 0b011 << 0;
    TIMSK2 |= _BV(OCIE2A);
    OCR2A = 249;
}

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

int main()
{
    millis_timer_init();
    seven_segment_init();
    seven_segment_clear_all();
    // DDRB |= _BV(1);

    timer2_init();

    // uint8_t byte_arr[] = {
    //     digit_values[10],
    //     digit_values[11],
    //     digit_values[12],
    //     digit_values[13],
    // };
    // seven_segment_write_bytes(byte_arr);

    // uint8_t byte_arr[] = {
    //     0xa,
    //     0xb,
    //     0xc,
    //     0xd,
    // };
    // seven_segment_write_digit_vals(byte_arr);

    // _delay_ms(2000);

    while (1)
    {
        // PORTB |= _BV(1);
        // _delay_ms(500);
        // PORTB &= ~_BV(1);
        // _delay_ms(500);
        t_millis = get_millis();
        // flash_colon_loop();
        // display time with flashing colon for 10 seconds, changing
        // the time at 5 seconds.

        prev_millis1 = t_millis;
        hour = 19;
        minute = 59;
        seven_segment_show_hour_minute(hour, minute);
        prev_millis2 = t_millis;
        while (t_millis - prev_millis1 < 5000)
        {
            flash_colon_loop();
            t_millis = get_millis();
        }

        hour = 20;
        minute = 00;
        seven_segment_show_hour_minute(hour, minute);
        prev_millis1 = t_millis;
        while (t_millis - prev_millis1 < 5000)
        {

            flash_colon_loop();
            t_millis = get_millis();
        }

        // display month and day for 5 seconds
        month = 7;
        day = 2;
        seven_segment_show_month_day(month, day);
        // for (int i = 0; i < 4; i++)
        // {
        //     seven_segment_set_decimal_point(i);
        // }
        seven_segment_set_decimal_point(3);
        seven_segment_set_decimal_point(1);
        prev_millis1 = t_millis;
        while (t_millis - prev_millis1 < 5000)
        {
            t_millis = get_millis();
        }

        // display year for 5 seconds
        year = 2026;
        seven_segment_show_year(year);
        seven_segment_set_decimal_point(3);
        prev_millis1 = t_millis;
        while (t_millis - prev_millis1 < 5000)
        {
            t_millis = get_millis();
        }

        // display temperature for 5 seconds
        temperature_celsius = 26.7;
        seven_segment_show_temperature(temperature_celsius);
        prev_millis1 = t_millis;
        while (t_millis - prev_millis1 < 5000)
        {
            t_millis = get_millis();
        }
        temperature_celsius = -32.1;
        seven_segment_show_temperature(temperature_celsius);
        prev_millis1 = t_millis;
        while (t_millis - prev_millis1 < 5000)
        {
            t_millis = get_millis();
        }

        // display humidity for 5 seconds
        humidity = 67.8;
        seven_segment_show_humidity(humidity);
        prev_millis1 = t_millis;
        while (t_millis - prev_millis1 < 5000)
        {
            t_millis = get_millis();
        }

        //  turn on all LEDs for 5 seconds
        seven_segment_set_all();
        prev_millis1 = t_millis;
        while (t_millis - prev_millis1 < 5000)
        {
            t_millis = get_millis();
        }
        // turn off all LEDs for 5 seconds
        seven_segment_clear_all();
        prev_millis1 = t_millis;
        while (t_millis - prev_millis1 < 5000)
        {
            t_millis = get_millis();
        }
    }
}