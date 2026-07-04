#ifndef BUTTON_H
#define BUTTON_H

#include "config.h"

/*
button_ISR_tick is placed inside the ISR
button_debounce and fsm called by button_ISR_tick

*/
#include "gpio.h"
#include "millis_micros.h"

// callback function typedef
typedef void (*button_action_t)(void);

/*
button states FSM
*/
enum button_state
{
    BUTTON_STATE_INIT,
    BUTTON_STATE_DOWN,
    BUTTON_STATE_UP,
    BUTTON_STATE_COUNT,
    BUTTON_STATE_PRESS,
    BUTTON_STATE_PRESSEND,
};

typedef struct
{
    // hardware
    Pin button_pin;

    // debouncing values
    uint16_t debounce_ms;
    uint16_t click_period_timeout_ms;
    uint16_t longpress_ms;

    // function callbacks
    button_action_t single_click_action;
    button_action_t double_click_action;
    button_action_t long_press_action;

    // debouncing variables
    uint8_t _lastDebouncedBState, _debouncedBState;
    uint32_t now, _lastDebouncedTime, _startTime;

    // Button FSM state and number of clicks
    enum button_state _state;
    uint8_t num_clicks;

} Button;

/**
 * @brief button bank setup
 *
 * @param btn pointer to Button
 */
void button_setup(Button *btn);

/**
 * @brief button ISR tick function. Place inside the periodic timer ISR.
 *
 * @param btn pointer to Button
 */
void button_ISR_tick(Button *btn);

/**
 * @brief button set debounce ms
 *
 * @param btn pointer to Button
 * @param ms milliseconds to wait for debouncing
 */
void button_set_debounce_ms(Button *btn, uint16_t ms);

/**
 * @brief button set click ms
 *
 * @param btn pointer to Button
 * @param click_period_timeout_ms milliseconds timeout after button release before
 *  counting button clicks
 */
void button_set_click_ms(Button *btn, uint16_t click_period_timeout_ms);

/**
 * @brief button set press ms
 *
 * @param btn pointer to Button
 * @param longpress_ms milliseconds for pressing the button to register as a long press
 *
 */
void button_set_press_ms(Button *btn, uint16_t longpress_ms);

/**
 * @brief button attach single click function
 *
 * @param btn pointer to Button
 * @param action function pointer to the single click function
 */
void button_attach_single_click(Button *btn,
                                button_action_t action);

/**
 * @brief button attach double click function
 *
 * @param btn pointer to Button
 *@param action function pointer to the double click function
 */
void button_attach_double_click(Button *btn,
                                button_action_t action);

/**
 * @brief button attach long press function
 *
 * @param btn pointer to Button
 * @param action function pointer to the long press function
 */
void button_attach_long_press(Button *btn,
                              button_action_t action);

/**
 * @brief button debounce function run inside the button ISR tick
 *
 * @param btn pointer to Button
 */
// uint8_t button_debounce(Button *btn);
uint8_t button_debounce(Button *btn);

/**
 * @brief button FSM function run inside the button ISR tick
 *
 * @param btn pointer to Button
 * @param debouncedBState return value from button_debounce function.
 */
void button_fsm(Button *btn, uint8_t debouncedBState);

#endif // BUTTON_H

// #ifndef BUTTON_H
// #define BUTTON_H

// #ifndef F_CPU
// #define F_CPU 16000000UL
// #endif
// #ifndef __AVR_ATmega328P__
// #define __AVR_ATmega328P__
// #endif
// /*
// button_ISR_tick is placed inside the ISR
// button_debounce and fsm called by button_ISR_tick

// */
// #include "../../HAL/src/gpio.h"
// typedef void (*button_action_t)(void);

// enum button_state
// {
//     BUTTON_STATE_INIT,
//     BUTTON_STATE_DOWN,
//     BUTTON_STATE_UP,
//     BUTTON_STATE_COUNT,
//     BUTTON_STATE_PRESS,
//     BUTTON_STATE_PRESSEND,
//     // BUTTON_IDLE,
//     // BUTTON_SINGLE_CLICK,
//     // BUTTON_DOUBLE_CLICK,
//     // BUTTON_LONG_PRESS,
// };

// typedef struct
// {
//     // hardware
//     volatile uint8_t *ddr;
//     volatile uint8_t *port;
//     volatile uint8_t *pin;
//     uint8_t button_mask;
//     // debouncing values
//     uint16_t debounce_ms;
//     uint16_t click_period_timeout_ms;
//     uint16_t longpress_ms;

//     // function callback arrays
//     button_action_t single_click_actions[8];
//     button_action_t double_click_actions[8];
//     button_action_t long_press_actions[8];

//     // debouncing variables
//     uint8_t debounceB0, debounceB1, debouncedBState;
//     uint32_t now, _startTimes[8];
//     enum button_state _states[8];
//     uint8_t num_clicks[8];

// } ButtonBank;

// /**
//  * @brief button bank setup
//  */
// void button_setup(ButtonBank *btn);

// /**
//  * @brief button ISR tick function
//  */
// void button_ISR_tick(ButtonBank *btn);

// /**
//  * @brief button set debounce ms
//  */
// void button_set_debounce_ms(ButtonBank *btn, uint16_t ms);

// /**
//  * @brief button set click ms
//  */
// void button_set_click_ms(ButtonBank *btn, uint16_t ms);

// /**
//  * @brief button set press ms
//  */
// void button_set_press_ms(ButtonBank *btn, uint16_t ms);

// /**
//  * @brief button attach single click function
//  */
// void button_attach_single_click(ButtonBank *btn,
//                                 uint8_t index,
//                                 button_action_t action);

// /**
//  * @brief button attach double click function
//  */
// void button_attach_double_click(ButtonBank *btn,
//                                 uint8_t index,
//                                 button_action_t action);

// /**
//  * @brief button attach long press function
//  */
// void button_attach_long_press(ButtonBank *btn,
//                               uint8_t index,
//                               button_action_t action);

// /**
//  * @brief button debounce function
//  */
// uint8_t button_debounce(ButtonBank *btn);

// /**
//  * @brief button FSM function
//  */
// void buttons_fsm(ButtonBank *btn, uint8_t debouncedBState);

// #endif // BUTTON_H
