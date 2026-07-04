#ifndef SEVEN_SEG_H
#define SEVEN_SEG_H
#ifndef _AVR_ATmega328P_
#define _AVR_ATmega328P_
#endif
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <stddef.h>
#include <avr/io.h>
#include "gpio.h"
#include <util/delay.h>
#include "millis_micros.h"

#define SEGMENTS_DDR DDRD
#define SEGMENTS_PORT PORTD
#define DIGITS_DDR DDRB
#define DIGITS_PORT PORTB
#define DIGITS_START_INDEX 0
#define COLON_DDR DDRB
#define COLON_PORT PORTB
#define COLON_PIN_NUM 5

typedef struct
{
    uint8_t display_digits[4];
    uint8_t colon;
    uint8_t cur_digit_index;
} display_data_t;

// function definitions
/* initialize display */
void seven_segment_init();
// ISR functions
// ******************************************************************
/* isr function to refresh the whole display */
void seven_segment_loop_isr(void);
// ******************************************************************

// low level functions
// ******************************************************************
/* clear all display segments */
void seven_segment_clear_all(void);
/* set all display segments */
void seven_segment_set_all(void);
/* write a byte to a digit */
void seven_segment_write_byte_to_digit(uint8_t byte, uint8_t digit);
/* write raw bytes to display segments */
void seven_segment_write_bytes(uint8_t bytes[4]);
/* write the colon */
void seven_segment_set_colon(uint8_t state);
/* enable the decimal point in a digit*/
void seven_segment_set_decimal_point(uint8_t digit);
// ******************************************************************

// write numbers to display
// ******************************************************************
/* write a hex digit to a digit */
void seven_segment_write_digit_val_to_digit(uint8_t digit_val, uint8_t digit);
/* write a sequence of hex digits to the display */
void seven_segment_write_digit_vals(uint8_t digit_indices[4]);
// ******************************************************************

// high level functions for different display modes
// ******************************************************************
/* show the hour and minute */
void seven_segment_show_hour_minute(uint8_t hour, uint8_t minute);
/* show the month and day */
void seven_segment_show_month_day(uint8_t month, uint8_t day);
/* show the year */
void seven_segment_show_year(uint16_t year);
/* show the temperature */
void seven_segment_show_temperature(float temperature_celsius);
/* show the humidity */
void seven_segment_show_humidity(float humidity);
// ******************************************************************

#endif