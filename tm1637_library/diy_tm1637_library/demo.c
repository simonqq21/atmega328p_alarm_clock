#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/*
DDR, PORT, PIN, and pins
*/
#define TM1637_DDR DDRD
#define TM1637_PORT PORTD
#define TM1637_PIN PIND
#define TM1637_DIO PD0
#define TM1637_CLK PD1

/*
test segment pattern
*/
#define TEST_SEGMENT_PATTERN 0b00111111

/*
set pin data direction macros
set, clear, toggle, read macros
DDR for pin data direction
PORT for writing value, HIGH/LOW for output, PULLUP/FLOATING for input
PIN for reading value
*/
#define SET_BIT(portreg, pin) ((portreg) |= _BV((pin)))
#define CLEAR_BIT(portreg, pin) ((portreg) &= ~_BV((pin)))
#define TOGGLE_BIT(portreg, pin) ((portreg) ^= _BV((pin)))
#define READ_BIT(pinreg, pin) ((pinreg) & _BV((pin)))

#define SET_PIN_INPUT(ddrreg, pin) ((ddrreg) &= ~_BV((pin)))
#define SET_PIN_DDR_OUT(ddrreg, pin) ((ddrreg) |= _BV((pin)))

// ################################################################
// GPIO function macros
/*
inline function to set pin as output
*/
static inline void gpio_set_pin_output(volatile uint8_t *ddrreg, uint8_t pin)
{
    *ddrreg |= _BV(pin);
}

/*
inline function to set pin as input
*/
static inline void gpio_set_pin_input(volatile uint8_t *ddrreg, uint8_t pin)
{
    *ddrreg &= ~_BV(pin);
}

/*
inline function to set pin high
*/
static inline void gpio_set_pin_high(volatile uint8_t *portreg, uint8_t pin)
{
    *portreg |= _BV(pin);
}

/*
inline function to set pin low
*/
static inline void gpio_set_pin_low(volatile uint8_t *portreg, uint8_t pin)
{
    *portreg &= ~_BV(pin);
}

/*
inline function to toggle pin value
*/
static inline void gpio_set_pin_toggle(volatile uint8_t *portreg, uint8_t pin)
{
    *portreg ^= _BV(pin);
}

/*
inline function to read pin value
*/
static inline uint8_t gpio_read_pin(volatile uint8_t *pinreg, uint8_t pin)
{
    if (*pinreg & _BV(pin))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int main(void)
{
    gpio_set_pin_output(&DDRB, PB5);
    gpio_set_pin_high(&PORTB, PB5);

    gpio_set_pin_output(&DDRD, PD0);
    gpio_set_pin_high(&PORTD, PD0);
    // gpio_set_pin_low(&PORTD, PD0);
    gpio_set_pin_output(&DDRD, PD3);
    gpio_set_pin_high(&PORTD, PD3);
    _delay_ms(1000);
    gpio_set_pin_low(&PORTD, PD3);

    // initialize TM1637
    gpio_set_pin_output(&TM1637_DDR, TM1637_CLK);
    gpio_set_pin_input(&TM1637_DDR, TM1637_DIO);
    gpio_set_pin_high(&TM1637_PORT, TM1637_CLK);
    gpio_set_pin_high(&TM1637_PORT, TM1637_DIO);

    // Send memory write command

    // Set the initial address

    // Transfer multiple words continuously

    // Send display control command

    return 0;
}
