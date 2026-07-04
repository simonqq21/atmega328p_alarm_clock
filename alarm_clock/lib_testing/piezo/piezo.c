#include "passive_buzzer.h"
#include "config.h"

void piezo_init(void)
{
    // set up PWM peripheral on PB1 (OC1A)
    // COM1A = 0b01
    // WGM1 = 0b0100
    // CS1 = variable
    // OCR1A = variable
    //
    TCCR1A |= _BV(COM1A0);
    TCCR1B |= _BV(WGM12);
    // 600 Hz
    /*
    focna = fclk / (2*n*(ocrna + 1))
    focna * 2*n*(ocrna + 1) = fclk
    ocrna + 1 = fclk / (focna * 2*n)
    ocrna = fclk / (focna * 2*n) - 1
    ocrna = 16000000 / (2*64*600) - 1
    ocrna = 207 for 600 Hz, prescaler of 8
    */
    TCCR1B |= _BV(CS11);
    // starting frequency and ending frequency
}