#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "millis_micros.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"

#include "dht22.h"

#define BAUD_RATE 9600
#define MYUBRR F_CPU / 16 / BAUD_RATE - 1

#define LED_PIN PB5
#define LED_PORT PORTB
#define LED_DDR DDRB
#define DEBUG_LED_PIN PB1

Pin led_pin, debug_led_pin;

dht_values_t dht_values;

// uint8_t led_brightness;

volatile uint32_t t_millis;

char buff[60];
/*
note: do not define strings as char*, define strings as char []
so that the memory for the string will surely be allocated.
*/
char str[30];
float temperature;
char temperature_str[8], humidity_str[8];
int brightness;

ISR(BADISR_vect)
{
    // user code here
    gpio_set_pin_high(&debug_led_pin);
}

ISR(TIMER0_COMPA_vect)
{
    millis_timer_ISR_loop();
}

ISR(USART_RX_vect)
{
    getchar_ISR();
}

void ioinit(void)
{
    led_pin.ddr = &LED_DDR;
    led_pin.port = &LED_PORT;
    led_pin.pin_num = LED_PIN;
    gpio_set_pin_output(&led_pin);
    debug_led_pin.ddr = &LED_DDR;
    debug_led_pin.port = &LED_PORT;
    debug_led_pin.pin_num = DEBUG_LED_PIN;
    gpio_set_pin_output(&debug_led_pin);
    gpio_set_pin_low(&debug_led_pin);
}

int main()
{
    ioinit();
    init_uart(MYUBRR);
    // millis_timer_init();

    // Put data into buffer, sends the data
    // cli();
    // printchar('a');
    // printchar('b');

    // printchar('c');
    // printchar('d');
    // printchar('\n');
    // sei();

    sprintf(str, "DHT22 example code \n");
    printstr(str, strlen(str));

    // cli();
    dht22_init();
    _delay_ms(3000);
    // dht22_measure_values(&dht_values);

    while (1)
    {
        dht22_measure_values(&dht_values);

        dtostrf(dht_values.temperature, 5, 2, temperature_str);
        snprintf(str, 30, "temperature = %s\n", temperature_str);
        printstr(str, strlen(str));

        dtostrf(dht_values.humidity, 5, 2, humidity_str);
        snprintf(str, 30, "humidity = %s\n", humidity_str);
        printstr(str, strlen(str));

        gpio_set_pin_high(&led_pin);
        _delay_ms(200);
        gpio_set_pin_low(&led_pin);
        _delay_ms(3000);
    }
}
