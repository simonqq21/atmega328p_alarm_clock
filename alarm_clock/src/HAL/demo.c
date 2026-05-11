#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
#ifndef F_CPU
#define F_CPU 16000000
#endif
#define rate 400

#include "src/gpio.h"
#include <util/delay.h>

const uint8_t digit_values[10] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b01110111, // A
    0b01111100, // b
    0b00111001, // C
    0b01011110, // d
    0b01111001, // E
    0b01110001, // F
};

int main(void)
{
    Pin segment_pins[8];
    for (int i = 0; i < 8; i++)
    {
        segment_pins[i].ddr = &DDRD;
        segment_pins[i].port = &PORTD;
        segment_pins[i].pin = &PIND;
        segment_pins[i].pin_num = i;
        gpio_set_pin_output(&segment_pins[i]);
    }
    Pin digit_pins[4];
    for (int i = 0; i < 4; i++)
    {
        digit_pins[i].ddr = &DDRB;
        digit_pins[i].port = &PORTB;
        digit_pins[i].pin = &PINB;
        digit_pins[i].pin_num = i;
        gpio_set_pin_output(&digit_pins[i]);
    }

    while (1)
    {
        PORTB |= 0x0F;
        PORTB &= ~0x08;
        PORTD = ~digit_values[1];
        _delay_ms(rate);
        PORTB |= 0x0F;
        PORTB &= ~0x04;
        PORTD = ~digit_values[2];
        _delay_ms(rate);
        PORTB |= 0x0F;
        PORTB &= ~0x02;
        PORTD = ~digit_values[4];
        _delay_ms(rate);
        PORTB |= 0x0F;
        PORTB &= ~0x01;
        PORTD = ~digit_values[8];
        _delay_ms(rate);
    }
}