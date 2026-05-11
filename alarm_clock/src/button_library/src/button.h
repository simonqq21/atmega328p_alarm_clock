#ifndef BUTTON_H
#define BUTTON_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
/*
button_ISR_tick is placed inside the ISR
button_debounce and fsm called by button_ISR_tick

*/
#include "../../HAL/src/gpio.h"
typedef void (*button_action_t)(void);

enum button_state
{
    BUTTON_IDLE,
    BUTTON_SINGLE_CLICK,
    BUTTON_DOUBLE_CLICK,
    BUTTON_LONG_PRESS,
};

typedef struct
{
    // hardware
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    volatile uint8_t *pin;
    uint8_t button_mask;
    // debouncing values
    uint16_t debounce_ms;
    uint16_t click_ms;
    uint16_t press_ms;

    // function callback arrays
    button_action_t single_click_actions[8];
    button_action_t double_click_actions[8];
    button_action_t long_press_actions[8];

} ButtonBank;

/**
 * @brief button bank setup
 */
void button_setup(ButtonBank *btn);

/**
 * @brief button ISR tick function
 */
void button_ISR_tick(ButtonBank *btn);

/**
 * @brief button set debounce ms
 */
void button_set_debounce_ms(ButtonBank *btn, uint16_t ms);

/**
 * @brief button set click ms
 */
void button_set_click_ms(ButtonBank *btn, uint16_t ms);

/**
 * @brief button set press ms
 */
void button_set_press_ms(ButtonBank *btn, uint16_t ms);

/**
 * @brief button attach single click function
 */
void button_attach_single_click(ButtonBank *btn, uint8_t index, button_action_t action);

/**
 * @brief button attach double click function
 */
void button_attach_double_click(ButtonBank *btn, uint8_t index, button_action_t action);

/**
 * @brief button attach long press function
 */
void button_attach_long_press(ButtonBank *btn, uint8_t index, button_action_t action);

/**
 * @brief button debounce function
 */
void button_debounce(ButtonBank *btn);

/**
 * @brief button FSM function
 */
void button_fsm(ButtonBank *btn);

#endif // BUTTON_H
