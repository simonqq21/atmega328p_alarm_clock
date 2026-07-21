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
#include "gpio.h"
#include "millis_micros.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"

#define BAUD_RATE 9600
#define MYUBRR F_CPU / 16 / BAUD_RATE - 1

#define LED_PIN PB5
#define LED_PORT PORTB
#define LED_DDR DDRB
#define DEBUG_LED_PIN PB1

#define DHT22_DDR DDRD
#define DHT22_PORT PORTD
#define DHT22_PIN PIND
#define DHT22_PIN_NUM 6

typedef struct
{
    float temperature;
    float humidity;
} dht_values_t;

typedef enum
{
    DHT22_OK,
    DHT22_CHECKSUM_ERROR,
    DHT22_TIMEOUT_ERROR,
} dht22_status_t;

Pin led_pin, debug_led_pin;
Pin DHT22_pin;

uint8_t dht22_raw_values[5];
dht_values_t dht_values;

// uint8_t led_brightness;

volatile uint32_t t_millis;

uint8_t counters[90];

char buff[60];
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

void dht22_init(void)
{
    DHT22_pin.ddr = &DHT22_DDR;
    DHT22_pin.port = &DHT22_PORT;
    DHT22_pin.pin = &DHT22_PIN;
    DHT22_pin.pin_num = DHT22_PIN_NUM;
}

/**
 * @brief read temperature and humidity values from the DHT22 sensor
 *
 */
dht22_status_t dht22_measure_values(dht_values_t *dht_values)
{
    uint8_t laststate = 1;
    uint8_t counter = 0;
    uint8_t j = 0;

#define MAXSTATECHANGES 85

    /*
    initiate communication with DHT22
    */
    gpio_set_pin_output(&DHT22_pin);
    gpio_set_pin_low(&DHT22_pin);
    _delay_ms(18);
    gpio_set_pin_input(&DHT22_pin);

    // counter = 0;
    // while (1)
    // {
    //     while (gpio_read_pin(&DHT22_pin) > 0)
    //     {
    //     };
    //     // counter++;
    //     // snprintf(str, 30, "c=%d\n", counter);
    //     // printstr(str, strlen(str));
    //     printchar('t');
    //     while (gpio_read_pin(&DHT22_pin) == 0)
    //     {
    //     }
    // }

    /*
    Pin state change detection loop
    Wait for sensor to respond with a pull down
    grab the 40 bytes that the DHT22 returns
    */
    // for each pin state change
    for (int i = 0; i < MAXSTATECHANGES; i++)
    {
        counter = 0;
        counters[i] = 0;
        // wait for pin to change state
        while (laststate == gpio_read_pin(&DHT22_pin))
        {
            counter++;
            counters[i]++;
            if (counter >= 250)
            {
                break;
            }
            _delay_us(1);
        }
        laststate = gpio_read_pin(&DHT22_pin);
        if (counter >= 250)
        {
            break;
        }
        /*
        Ignore the first two state changes.
        After the MCU pulls the pin low for at least 18 ms and returns it high,
        the DHT22 will respond by pulling the pin low for 80 us and then pulling
        it high for 80 us.

        When the DHT22 outputs a '0', it pulls the pin low for 50 us
        then pulls it high for 26-28 us.
        When the DHT22 outputs a '1', it pulls the pin low for 50 us
        then pulls it high for 70 us.
        */
        //     // sprintf(str, "%d, %d\n", i, laststate);
        //     // printstr(str, strlen(str));

        if (i >= 4)
        {
            // if the pin is currently low
            if (laststate == 0)
            {
                // shift left
                dht22_raw_values[j / 8] <<= 1;

                // check if bit is 0 or 1
                if (counter >= 12)
                {
                    dht22_raw_values[j / 8] |= 1;
                }
                j++;
            }
        }
    }
    // sprintf(str, "j=%d\n", j);
    // printstr(str, strlen(str));

    for (int i = 0; i < 90; i++)
    {
        sprintf(str, "counter[%d] = %d\n", i, counters[i]);
        printstr(str, strlen(str));
    }

    /*
    check the checksum and convert the raw data into temperature and humidity
    data
    */
    // sprintf(str, "%d, %d, %d, %d, %d\n", dht22_raw_values[0], dht22_raw_values[1], dht22_raw_values[2], dht22_raw_values[3], dht22_raw_values[4]);
    // printstr(str, strlen(str));

    if (j >= 40 && dht22_raw_values[4] == ((dht22_raw_values[0] + dht22_raw_values[1] + dht22_raw_values[2] + dht22_raw_values[3]) & 0xff))
    {
        dht_values->temperature = (float)(dht22_raw_values[2] & 0x7f) * 256 + (float)dht22_raw_values[3];
        dht_values->humidity = (float)dht22_raw_values[0] * 256 + (float)dht22_raw_values[1];
        dht_values->temperature /= 10;
        dht_values->humidity /= 10;
        if (dht22_raw_values[2] & 0x80)
        {
            dht_values->temperature *= -1;
        }
        return DHT22_OK;
    }
    else
        return DHT22_CHECKSUM_ERROR;
    return DHT22_OK;
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

    sprintf(str, "helloworldA\n");
    printstr(str, strlen(str));

    cli();
    dht22_init();
    _delay_ms(3000);
    dht22_measure_values(&dht_values);

    while (1)
    {
        // dtostrf(30.3, 5, 2, temperature_str);
        // snprintf(str, 30, "temperature = %s, %d\n", temperature_str, *str);
        // printstr(str, strlen(str));

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
