#include "piezo.h"
#include "config.h"

// #include "gpio.h"
// Pin common_pin = {
//     .ddr = &DDRB,
//     .port = &PORTB,
//     .pin_num = 0,
// };
// Pin cathode_pin = {
//     .ddr = &DDRD,
//     .port = &PORTD,
//     .pin_num = 1,
// };

Note *note_sequence;
uint16_t note_sequence_len = 0;
volatile uint16_t last_note_idx = 0;
volatile uint16_t note_sequence_idx = 0;
volatile uint8_t is_playing;
volatile uint8_t note_paused = false;
volatile uint32_t piezo_millis = 0;
volatile uint32_t last_piezo_millis = 0;

// Note rest = {
//     .ocr1a_value = OCR1A_NOTE_REST,
//     .duration = 0,
// };

void piezo_init(uint8_t note_buf_len)
{
    // gpio_set_pin_output(&common_pin);
    // gpio_set_pin_output(&cathode_pin);
    // gpio_set_pin_low(&common_pin);

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
    DDRB |= _BV(PB1);
    // starting frequency and ending frequency

    // allocate note memory
    note_sequence_len = note_buf_len;
    note_sequence = malloc(note_sequence_len * sizeof(Note));
    note_sequence_idx = 0;
    last_note_idx = 0;
    is_playing = false;
}

void piezo_loop_ISR(uint32_t piezo_millis)
{
    // piezo_millis++;

    if (is_playing)
    {
        // gpio_set_pin_high(&cathode_pin);
        // after pause period
        if (note_paused && (piezo_millis - last_piezo_millis) > 20)
        {
            last_piezo_millis = piezo_millis;
            if (note_sequence_idx >= (last_note_idx - 1))
            {
                piezo_stop_sequence();
            }
            note_paused = false;
        }

        // pause the note for a very short duration after playing the note
        if ((piezo_millis - last_piezo_millis) > note_sequence[note_sequence_idx].duration - 20)
        {
            last_piezo_millis = piezo_millis;
            note_paused = true;
            piezo_off();

            note_sequence_idx++;
        }

        // play the current note in the sequence
        if (is_playing && !note_paused && OCR1A != note_sequence[note_sequence_idx].ocr1a_value)
        {
            piezo_play_cur_note();
        }
    }
    else
    {
        // gpio_set_pin_low(&cathode_pin);
        piezo_off();
    }
}

void piezo_clear_all_notes(void)
{
    last_note_idx = 0;
}

void piezo_add_note(Note *note)
{
    note_sequence[last_note_idx] = *note;
    last_note_idx++;
}

void piezo_play_cur_note(void)
{
    OCR1A = note_sequence[note_sequence_idx].ocr1a_value;
    if (OCR1A > OCR1A_NOTE_REST)
    {
        TCCR1A |= _BV(COM1A0);
    }
    else
    {
        TCCR1A &= ~_BV(COM1A0);
    }
}

void piezo_play_sequence(void)
{
    DDRB |= _BV(PB1);
    is_playing = true;
    note_sequence_idx = 0;
    last_piezo_millis = piezo_millis;
}

void piezo_off(void)
{
    OCR1A = OCR1A_NOTE_REST;
    TCCR1A &= ~_BV(COM1A0);
}

void piezo_stop_sequence(void)
{
    piezo_off();
    DDRB &= ~_BV(PB1);
    is_playing = false;
}

uint8_t piezo_is_playing(void)
{
    return is_playing;
}