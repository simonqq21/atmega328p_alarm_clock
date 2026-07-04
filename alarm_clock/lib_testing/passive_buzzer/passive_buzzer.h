#ifndef PASSIVE_BUZZER_H
#define PASSIVE_BUZZER_H

#include "config.h"

typedef struct
{
    uint16_t frequency;
    uint16_t duration;
} Note;

/**
 * @brief initialize PWM for piezo buzzer
 * This initializes Timer1 for tone generation.
 */
void passive_buzzer_init(void);
/**
 * @brief play a single note for a certain duration
 *
 */

/**
 * @brief play a sequence of notes with a sequence of durations
 */
#endif