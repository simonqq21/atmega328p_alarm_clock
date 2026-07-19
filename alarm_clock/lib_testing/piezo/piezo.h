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
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951

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
#define OCR1A_NOTE_CS6 901
#define OCR1A_NOTE_D6 850
#define OCR1A_NOTE_DS6 802
#define OCR1A_NOTE_E6 757
#define OCR1A_NOTE_F6 715
#define OCR1A_NOTE_FS6 675
#define OCR1A_NOTE_G6 637
#define OCR1A_NOTE_GS6 601
#define OCR1A_NOTE_A6 567
#define OCR1A_NOTE_AS6 535
#define OCR1A_NOTE_B6 505
#define OCR1A_NOTE_C7 477
#define OCR1A_NOTE_CS7 450
#define OCR1A_NOTE_D7 425
#define OCR1A_NOTE_DS7 401
#define OCR1A_NOTE_E7 378
#define OCR1A_NOTE_F7 357
#define OCR1A_NOTE_FS7 337
#define OCR1A_NOTE_G7 318
#define OCR1A_NOTE_GS7 300
#define OCR1A_NOTE_A7 283
#define OCR1A_NOTE_AS7 267
#define OCR1A_NOTE_B7 252
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
void piezo_loop_ISR(uint32_t piezo_millis);

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
 * @brief play a sequence of notes with a sequence of durations
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
 * @brief
 */
uint8_t piezo_is_playing(void);

#endif