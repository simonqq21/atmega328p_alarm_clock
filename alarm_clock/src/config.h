#ifndef CONFIG_H
#define CONFIG_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "avr/io.h"
#include "avr/interrupt.h"
#include "avr/eeprom.h"
#include "util/atomic.h"

// #define SEGMENTS_DDR DDRD
// #define SEGMENTS_PORT PORTD
#define SR_DDR DDRD
#define SR_PORT PORTD
#define SER_PIN 2
#define SRCLK_PIN 3
#define LCLK_PIN 4

#define DIGITS_DDR DDRB
#define DIGITS_PORT PORTB
#define DIGITS_START_INDEX 0
#define COLON_DDR DDRB
#define COLON_PORT PORTB
#define COLON_PIN_NUM 5

// LED strip
#define WS2812_DDR DDRD
#define WS2812_PORT PORTD
#define WS2812_PIN_NUM 5

// buttons
#define BTNS_PORT PORTC
#define BTNS_DDR DDRC
#define BTNS_PIN PINC
#define BTN_1_PIN_NUM PC1 // down
#define BTN_2_PIN_NUM PC2 // reset
#define BTN_3_PIN_NUM PC3 // up

#define MIN_YEAR 1900
#define MAX_YEAR 1999

// DHT11
typedef struct
{
    double humidity, temperature;
} sensor_values_t;

#endif