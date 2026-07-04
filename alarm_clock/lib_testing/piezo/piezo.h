#ifndef PASSIVE_BUZZER_H
#define PASSIVE_BUZZER_H

#include "config.h"

// defined note frequencies
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

typedef struct
{
    uint16_t frequency;
    uint16_t duration;
} Note;

/**
 * @brief initialize PWM for piezo buzzer
 * This initializes Timer1 for tone generation.
 */
void piezo_init(void);

/**
 * @brief play a single note for a certain duration
 *
 */
void piezo_play_note(Note note);

/**
 * @brief play a sequence of notes with a sequence of durations
 */
#endif