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
#define TM1637_DIO PD6
#define TM1637_CLK PD7

/*
test segment pattern
*/
#define TEST_SEGMENT_PATTERN 0b10111111

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

// ################################################################
// TWI timing
#define TM1637_QUARTER_CLOCK_PERIOD 1000 // In microseconds (us)
#define TM1637_HALF_CLOCK_PERIOD TM1637_QUARTER_CLOCK_PERIOD * 2
#define TM1637_QUARTER_CYCLE_DELAY _delay_us(TM1637_QUARTER_CLOCK_PERIOD)
#define TM1637_HALF_CYCLE_DELAY _delay_us(TM1637_HALF_CLOCK_PERIOD)

// ################################################################
// TWI macros
/*
set pin output LOW
*/
static inline void gpio_set_pin_output_low(volatile uint8_t *ddrreg,
                                           volatile uint8_t *portreg,
                                           uint8_t pin)
{
    gpio_set_pin_output(ddrreg, pin);
    gpio_set_pin_low(portreg, pin);
}
/*
set pin input HIGH
*/
static inline void gpio_set_pin_input_high(volatile uint8_t *ddrreg,
                                           volatile uint8_t *portreg,
                                           uint8_t pin)
{
    gpio_set_pin_input(ddrreg, pin);
    gpio_set_pin_high(portreg, pin);
}

static inline void dio_pull_down(void)
{
    gpio_set_pin_output_low(&TM1637_DDR,
                            &TM1637_PORT,
                            TM1637_DIO);
}

static inline void dio_pull_up(void)
{
    gpio_set_pin_input_high(&TM1637_DDR,
                            &TM1637_PORT,
                            TM1637_DIO);
}

static inline void clk_pull_down(void)
{
    gpio_set_pin_output_low(&TM1637_DDR,
                            &TM1637_PORT,
                            TM1637_CLK);
}

static inline void clk_pull_up(void)
{
    gpio_set_pin_input_high(&TM1637_DDR,
                            &TM1637_PORT,
                            TM1637_CLK);
}

static inline void tm1637_quarter_delay(void)
{
    _delay_us(TM1637_QUARTER_CLOCK_PERIOD);
}

static inline void tm1637_half_delay(void)
{
    _delay_us(TM1637_HALF_CLOCK_PERIOD);
}

// ################################################################
// start, write byte, and stop macros
/*
start
    DIO pull down half cycle
    CLK pull down half cycle

write byte 8 cycles
    repeat for each bit in the byte
        CLK pull down
        DIO set bit
        wait half cycle
        CLK pull down
        wait half cycle
    CLK pull down half cycle // start of ACK
    CLK pull up half cycle // ACK ends after CLK pull up half cycle

stop
    CLK pull down half cycle
    CLK pull up half cycle
    DIO pull up half cycle

*/

/*
start
    DIO pull down half cycle
    CLK pull down half cycle
*/
static inline void tm1637_start()
{
    dio_pull_down();
    tm1637_half_delay();
    // clk_pull_down();
    // tm1637_delay();
}

/*
write byte 8 cycles
    repeat for each bit in the byte
        CLK pull down
        DIO set bit
        wait half cycle
        CLK pull up
        wait half cycle
    CLK pull down half cycle // start of ACK
    CLK pull up half cycle // ACK ends after CLK pull up half cycle
*/
static inline uint8_t tm1637_write_byte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        clk_pull_down();
        tm1637_quarter_delay();
        if (data & 1)
            dio_pull_up();
        else
            dio_pull_down();
        tm1637_quarter_delay();
        // tm1637_half_delay();
        clk_pull_up();
        tm1637_half_delay();
        data >>= 1;
    }
    clk_pull_down(); // ACK
    dio_pull_up();   // set DIO input pullup
    tm1637_half_delay();
    clk_pull_up();
    tm1637_half_delay();
    uint8_t ack = gpio_read_pin(&TM1637_PIN, TM1637_DIO);
    if (ack == 0)
    {
        // set DIO output and low
        dio_pull_down();
    }
    tm1637_half_delay();
    return ack;
}

/*
stop
    CLK pull down half cycle
    DIO pull down half cycle // to make sure that DIO will have rising edge when CLK is high
    CLK pull up half cycle
    DIO pull up half cycle
*/
static inline void tm1637_stop()
{
    clk_pull_down();
    tm1637_half_delay();
    dio_pull_down();
    tm1637_half_delay();
    clk_pull_up();
    tm1637_half_delay();
    dio_pull_up();
    tm1637_half_delay();
}

int main(void)
{
    uint8_t data;

    gpio_set_pin_output(&DDRB, PB5);
    gpio_set_pin_high(&PORTB, PB5);

    // gpio_set_pin_output(&DDRD, PD0);
    // gpio_set_pin_high(&PORTD, PD0);
    // gpio_set_pin_low(&PORTD, PD0);
    // gpio_set_pin_output(&DDRD, PD3);
    // gpio_set_pin_high(&PORTD, PD3);
    // _delay_ms(1000);
    // gpio_set_pin_low(&PORTD, PD3);

    // initialize TM1637
    clk_pull_up();
    dio_pull_up();
    _delay_ms(1000);

    // clk_pull_down();
    // dio_pull_down();
    // _delay_ms(1000);
    // clk_pull_up();
    // dio_pull_up();
    // _delay_ms(1000);

    // // test macro
    // for (int i = 0; i < 5; i++)
    // {
    //     dio_pull_up();
    //     _delay_ms(500);
    //     dio_pull_down();
    //     _delay_ms(500);
    // }

    // Send memory write command
    data = 0b01000000;
    tm1637_start();
    tm1637_write_byte(data);
    tm1637_stop();
    // _delay_ms(1000);

    // Set the initial address
    data = 0b11000000;
    tm1637_start();
    tm1637_write_byte(data);
    // _delay_ms(1000);

    // Transfer multiple words continuously
    data = 0b00000001;
    tm1637_write_byte(data);
    data = 0b10000010;
    tm1637_write_byte(data);
    data = 0b00000100;
    tm1637_write_byte(data);
    data = 0b00001000;
    tm1637_write_byte(data);
    tm1637_stop();
    // _delay_ms(1000);

    // Send display control command
    data = 0b10001001;
    tm1637_start();
    tm1637_write_byte(data);
    tm1637_stop();

    _delay_ms(1000);
    // Send display control command
    data = 0b10001111;
    tm1637_start();
    tm1637_write_byte(data);
    tm1637_stop();

    return 0;
}
