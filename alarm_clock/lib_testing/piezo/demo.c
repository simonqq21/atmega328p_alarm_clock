#ifndef _AVR_ATmega328P_
#define _AVR_ATmega328P_
#endif
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "piezo.h"
#include <util/delay.h>
#include "millis_micros.h"
#include "gpio.h"
#include <util/atomic.h>
#include "config.h"

Pin common_pin = {
    .ddr = &DDRB,
    .port = &PORTB,
    .pin_num = 0,
};
Pin cathode_pin = {
    .ddr = &DDRD,
    .port = &PORTD,
    .pin_num = 1,
};

ISR(TIMER2_COMPA_vect)
{
    piezo_loop_ISR();
}

// trigger a CTC every 1ms
void timer2_init(void)
{
    // WGM = 010, CTC mode
    // COM2A = 00, OC2A disconnected
    // CS2 = 100, prescaler 1/64, 4us per increment
    // OCR2A = 250 - 1 = 249, 250 * 4us = 1ms
    // enable OCIE2A, output compare match A interrupt handle
    TCCR2A = 0;
    TCCR2B = 0;
    TCCR2A |= _BV(WGM21);
    TCCR2B |= _BV(CS22);
    TIMSK2 |= _BV(OCIE2A);
    OCR2A = 249;
    sei();
}

int main()
{
    timer2_init();
    // gpio_set_pin_output(&common_pin);
    // gpio_set_pin_output(&cathode_pin);
    // gpio_set_pin_low(&common_pin);

    piezo_init(10);
    // Note note = {.ocr1a_value = 8000, .duration = 100};
    // piezo_add_note(&note);
    // note.ocr1a_value = OCR1A_NOTE_REST;
    // piezo_clear_all_notes();
    // piezo_add_note(&note);
    // _delay_ms(200);

    // Note notes[] = {
    //     {.ocr1a_value = OCR1A_NOTE_C5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_C5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_G5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_G5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_A5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_A5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_G5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_REST, .duration = 500},
    // };

    // // 8 notes
    // for (int i = 0; i < 9; i++)
    // {
    //     piezo_add_note(&notes[i]);
    // }

    // // twinkle2 little star
    // Note notes[] = {
    //     {.ocr1a_value = OCR1A_NOTE_C5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_C5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_G5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_G5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_A5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_A5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_G5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_REST, .duration = 500},

    //     {.ocr1a_value = OCR1A_NOTE_F5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_F5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_E5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_E5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_D5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_D5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_C5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_REST, .duration = 500},

    //     {.ocr1a_value = OCR1A_NOTE_G5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_G5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_F5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_F5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_E5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_E5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_D5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_REST, .duration = 500},

    //     {.ocr1a_value = OCR1A_NOTE_G5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_G5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_F5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_F5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_E5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_E5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_D5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_REST, .duration = 500},

    //     {.ocr1a_value = OCR1A_NOTE_C5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_C5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_G5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_G5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_A5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_A5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_G5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_REST, .duration = 500},

    //     {.ocr1a_value = OCR1A_NOTE_F5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_F5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_E5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_E5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_D5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_D5, .duration = 500},
    //     {.ocr1a_value = OCR1A_NOTE_C5, .duration = 100},
    //     {.ocr1a_value = OCR1A_NOTE_C6, .duration = 1000},
    //     {.ocr1a_value = OCR1A_NOTE_REST, .duration = 500},
    // };

    // // 49 notes
    // for (int i = 0; i < 50; i++)
    // {
    //     piezo_add_note(&notes[i]);
    // }

    uint16_t note_ms = 120;

    // never gonna give you up
    Note notes[] = {
        {.ocr1a_value = OCR1A_NOTE_AS5, .duration = note_ms},
        {.ocr1a_value = OCR1A_NOTE_CS6, .duration = note_ms},
        {.ocr1a_value = OCR1A_NOTE_CS6, .duration = note_ms},
        {.ocr1a_value = OCR1A_NOTE_AS5, .duration = note_ms},
        {.ocr1a_value = OCR1A_NOTE_F6, .duration = note_ms * 3},
        {.ocr1a_value = OCR1A_NOTE_F6, .duration = note_ms * 3},
        {.ocr1a_value = OCR1A_NOTE_DS6, .duration = note_ms * 4},
        {.ocr1a_value = OCR1A_NOTE_REST, .duration = note_ms * 2},

        {.ocr1a_value = OCR1A_NOTE_GS5, .duration = note_ms},
        {.ocr1a_value = OCR1A_NOTE_C6, .duration = note_ms},
        {.ocr1a_value = OCR1A_NOTE_C6, .duration = note_ms},
        {.ocr1a_value = OCR1A_NOTE_GS5, .duration = note_ms},
        {.ocr1a_value = OCR1A_NOTE_DS6, .duration = note_ms * 3},
        {.ocr1a_value = OCR1A_NOTE_DS6, .duration = note_ms * 3},
        {.ocr1a_value = OCR1A_NOTE_CS6, .duration = note_ms * 3},
        {.ocr1a_value = OCR1A_NOTE_C6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS5, .duration = note_ms * 2}, // 17

        {.ocr1a_value = OCR1A_NOTE_AS5, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_CS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_CS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS5, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_CS6, .duration = note_ms * 4},
        {.ocr1a_value = OCR1A_NOTE_DS6, .duration = note_ms * 2},
        {.ocr1a_value = OCR1A_NOTE_C6, .duration = note_ms * 4},
        {.ocr1a_value = OCR1A_NOTE_AS5, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_GS5, .duration = note_ms * 4},
        {.ocr1a_value = OCR1A_NOTE_GS5, .duration = note_ms * 2},
        {.ocr1a_value = OCR1A_NOTE_DS6, .duration = note_ms * 4},
        {.ocr1a_value = OCR1A_NOTE_CS6, .duration = note_ms * 8}, // 29

        {.ocr1a_value = OCR1A_NOTE_AS5, .duration = note_ms},
        {.ocr1a_value = OCR1A_NOTE_CS6, .duration = note_ms},
        {.ocr1a_value = OCR1A_NOTE_CS6, .duration = note_ms},
        {.ocr1a_value = OCR1A_NOTE_AS5, .duration = note_ms},
        {.ocr1a_value = OCR1A_NOTE_F6, .duration = note_ms * 3},
        {.ocr1a_value = OCR1A_NOTE_F6, .duration = note_ms * 3},
        {.ocr1a_value = OCR1A_NOTE_DS6, .duration = note_ms * 4},
        {.ocr1a_value = OCR1A_NOTE_REST, .duration = note_ms * 2}, // 37

        {.ocr1a_value = OCR1A_NOTE_GS5, .duration = note_ms},
        {.ocr1a_value = OCR1A_NOTE_C6, .duration = note_ms},
        {.ocr1a_value = OCR1A_NOTE_C6, .duration = note_ms},
        {.ocr1a_value = OCR1A_NOTE_GS5, .duration = note_ms},
        {.ocr1a_value = OCR1A_NOTE_GS6, .duration = note_ms * 4},
        {.ocr1a_value = OCR1A_NOTE_C6, .duration = note_ms * 2},
        {.ocr1a_value = OCR1A_NOTE_CS6, .duration = note_ms * 4},
        {.ocr1a_value = OCR1A_NOTE_C6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS5, .duration = note_ms * 2}, // 46

        {.ocr1a_value = OCR1A_NOTE_AS5, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_CS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_CS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS5, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_CS6, .duration = note_ms * 4},
        {.ocr1a_value = OCR1A_NOTE_DS6, .duration = note_ms * 2},
        {.ocr1a_value = OCR1A_NOTE_C6, .duration = note_ms * 4},
        {.ocr1a_value = OCR1A_NOTE_AS5, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_GS5, .duration = note_ms * 4},
        {.ocr1a_value = OCR1A_NOTE_GS5, .duration = note_ms * 2},
        {.ocr1a_value = OCR1A_NOTE_DS6, .duration = note_ms * 4},
        {.ocr1a_value = OCR1A_NOTE_CS6, .duration = note_ms * 8},
        {.ocr1a_value = OCR1A_NOTE_REST, .duration = note_ms * 6}, // 59

    };
    // 59 notes
    for (int i = 0; i < 59 + 1; i++)
    {
        piezo_add_note(&notes[i]);
    }

    // the maiden's prayer

    piezo_play_sequence();

    while (1)
    {
        // gpio_toggle_pin_output(&cathode_pin);
        // _delay_ms(500);

        // if (millis1 - last_millis1 > 500)
        // {
        //     last_millis1 = millis1;
        //     gpio_toggle_pin_output(&cathode_pin);
        // }
        // TCCR1B |= _BV(CS11);
        // _delay_ms(1000);
        // TCCR1B &= ~_BV(CS11);
        // _delay_ms(1000);
    }
}