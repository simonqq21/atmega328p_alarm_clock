#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// #include <avr/io.h>
// #include <avr/sleep.h>
// #include <avr/wdt.h>
#include "src/millis_micros.h"
#include "src/gpio.h"
// #include <avr/interrupt.h>
// #include <util/atomic.h>

/*
IO pins
 */
#define MILLIS_LED_PORT PORTD
#define MILLIS_LED_PIN PD4
#define MILLIS_LED_DDR DDRD
#define MICROS_LED_PORT PORTD
#define MICROS_LED_PIN PD5
#define MICROS_LED_DDR DDRD
#define COMMON_LED_PORT PORTB
#define COMMON_LED_PIN PB0
#define COMMON_LED_DDR DDRB

/*
previous timestamp vars
 */
volatile uint32_t prevMillis, prevMicros;

Pin millis_led, micros_led, common_led;

ISR(TIMER0_COMPA_vect)
{
    millis_timer_ISR_loop();
}

void ioinit(void)
{
    /*
    GPIO
    */
    millis_led.port = &MILLIS_LED_PORT;
    millis_led.ddr = &MILLIS_LED_DDR;
    millis_led.pin_num = MILLIS_LED_PIN;
    micros_led.port = &MICROS_LED_PORT;
    micros_led.ddr = &MICROS_LED_DDR;
    micros_led.pin_num = MICROS_LED_PIN;
    common_led.port = &COMMON_LED_PORT;
    common_led.ddr = &COMMON_LED_DDR;
    common_led.pin_num = COMMON_LED_PIN;

    gpio_set_pin_output(&millis_led);
    gpio_set_pin_output(&micros_led);
    gpio_set_pin_output(&common_led);
    gpio_set_pin_low(&common_led);
    gpio_set_pin_high(&millis_led);
    gpio_set_pin_high(&micros_led);
    // _delay_ms(1000);
    // gpio_set_pin_low(&micros_led);
}

int main()
{
    ioinit();
    millis_timer_init();

    while (1)
    {
        uint32_t t_millis = get_millis();
        uint32_t t_micros = get_micros();

        //
        if (t_millis - prevMillis >= 500)
        {
            prevMillis = t_millis;
            gpio_toggle_pin_output(&millis_led);
        }

        if (t_micros - prevMicros >= 500000)
        {
            prevMicros = t_micros;
            gpio_toggle_pin_output(&micros_led);
        }
    }
}