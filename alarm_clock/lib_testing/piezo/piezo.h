#ifndef PIEZO_H
#define PIEZO_H

#include "config.h"

// defined note frequencies
#define NOTE_REST 0
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047

// defined note OCRNA values
// if OCR1A value is 0, disable the piezo output because that
// is a rest.
#define OCR1A_NOTE_REST 0
#define OCR1A_NOTE_C5 1911
#define OCR1A_NOTE_CS5 1804
#define OCR1A_NOTE_D5 1703
#define OCR1A_NOTE_DS5 1607
#define OCR1A_NOTE_E5 1516
#define OCR1A_NOTE_F5 1432
#define OCR1A_NOTE_FS5 1350
#define OCR1A_NOTE_G5 1275
#define OCR1A_NOTE_GS5 1202
#define OCR1A_NOTE_A5 1135
#define OCR1A_NOTE_AS5 1072
#define OCR1A_NOTE_B5 1011
#define OCR1A_NOTE_C6 954

typedef struct
{
    uint16_t ocr1a_value;
    uint16_t duration;
} Note;

/**
 * @brief initialize PWM for piezo buzzer
 * This initializes Timer1 for tone generation.
 */
void piezo_init(uint8_t note_buf_len);

/**
 * @brief piezo playing loop runs only when prompted to play.
 */
void piezo_loop_ISR(void);

/**
 * @brief play a single note for a certain duration
 *
 */
void piezo_play_cur_note(void);

/**
 * @brief
 *
 */
void piezo_clear_all_notes(void);

/**
 * @brief
 *
 */
void piezo_add_note(Note *note);

void piezo_play_cur_note(void);

/**
 * @brief
 *
 */
void piezo_play_sequence(void);

void piezo_off(void);

/**
 * @brief
 *
 */
void piezo_stop_sequence(void);

/**
 * @brief play a sequence of notes with a sequence of durations
 */
#endif