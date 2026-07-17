#include "notes.h"
/*
    load tune into memory
    */
void load_alarm_notes(void)
{
    // the maiden's prayer
    uint16_t note_ms = 250;
    Note notes[] = {
        {.ocr1a_value = OCR1A_NOTE_AS5, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_DS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_G6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_DS7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_G7, .duration = note_ms * 5},
        {.ocr1a_value = OCR1A_NOTE_F7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_F7, .duration = note_ms * 2},
        {.ocr1a_value = OCR1A_NOTE_DS7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_DS7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_D7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_C7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_C7, .duration = note_ms * 6}, // 13

        {.ocr1a_value = OCR1A_NOTE_REST, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS5, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_D6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_F6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_GS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_D7, .duration = note_ms * 5},
        {.ocr1a_value = OCR1A_NOTE_C7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_C7, .duration = note_ms * 2},
        {.ocr1a_value = OCR1A_NOTE_AS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_GS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_G6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_G6, .duration = note_ms * 6}, // 14

        {.ocr1a_value = OCR1A_NOTE_REST, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS5, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_DS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_G6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_DS7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_G7, .duration = note_ms * 5},
        {.ocr1a_value = OCR1A_NOTE_F7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_F7, .duration = note_ms * 2},
        {.ocr1a_value = OCR1A_NOTE_DS7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_DS7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_D7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_C7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_C7, .duration = note_ms * 6}, // 14

        {.ocr1a_value = OCR1A_NOTE_REST, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_D6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_F6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_GS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_D7, .duration = note_ms * 1}, // 6

        {.ocr1a_value = OCR1A_NOTE_REST, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_DS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_G6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_DS7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_G7, .duration = note_ms * 1}, // 6

        {.ocr1a_value = OCR1A_NOTE_AS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_A6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_AS6, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_G7, .duration = note_ms * 2},
        {.ocr1a_value = OCR1A_NOTE_F7, .duration = note_ms * 1},
        {.ocr1a_value = OCR1A_NOTE_DS7, .duration = note_ms * 6}, // 6

        {.ocr1a_value = OCR1A_NOTE_REST, .duration = note_ms * 10}, // 1

    };
    for (int i = 0; i < 60 + 1; i++)
    {
        piezo_add_note(&notes[i]);
    }
}