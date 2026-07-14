#ifndef _AVR_ATmega328P_
#define _AVR_ATmega328P_
#endif
#ifndef F_CPU
#define F_CPU 16000000UL
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

#define SEGMENTS_DDR DDRD
#define SEGMENTS_PORT PORTD
#define DIGITS_DDR DDRB
#define DIGITS_PORT PORTB
#define DIGITS_START_INDEX 0
#define COLON_DDR DDRB
#define COLON_PORT PORTB
#define COLON_PIN_NUM 5
