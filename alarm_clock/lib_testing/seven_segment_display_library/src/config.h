#ifndef _AVR_ATmega328P_
#define _AVR_ATmega328P_
#endif
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "avr/io.h"
#include "stdint.h"
#include "avr/interrupt.h"
#include "util/atomic.h"

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
