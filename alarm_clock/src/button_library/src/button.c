#include "button.h"

void button_setup(ButtonBank *btn)
{
    // set DDR input
    *btn->ddr &= ~btn->button_mask;
    // enable pullup
    *btn->port |= btn->button_mask;
}

void button_ISR_tick(ButtonBank *btn)
{
    buttons_fsm(btn, button_debounce(btn));
}

void button_set_debounce_ms(ButtonBank *btn, uint16_t ms)
{
    btn->debounce_ms = ms;
}

void button_set_click_ms(ButtonBank *btn, uint16_t ms)
{
    btn->click_ms = ms;
}

void button_set_press_ms(ButtonBank *btn, uint16_t ms)
{
    btn->press_ms = ms;
}

void button_attach_single_click(ButtonBank *btn,
                                uint8_t index,
                                button_action_t action)
{
    btn->single_click_actions[index] = action;
}

void button_attach_double_click(ButtonBank *btn,
                                uint8_t index,
                                button_action_t action)
{
    btn->double_click_actions[index] = action;
}

void button_attach_long_press(ButtonBank *btn,
                              uint8_t index,
                              button_action_t action)
{
    btn->long_press_actions[index] = action;
}

uint8_t button_debounce(ButtonBank *btn)
{
    uint8_t delta, changes, sample;
    btn->now = millis();
    /**
     * read sample from PORT
     */
    sample = *btn->pin & btn->button_mask;

    /**
     *     Set delta to changes from last sample
     *     sample is the raw input value
     *     debouncedBState is the stable, "debounced" value
     */
    delta = sample ^ btn->debouncedBState;

    /**
     * Increment two byte counter
     */
    btn->debounceB1 = btn->debounceB1 ^ btn->debounceB0;
    btn->debounceB0 = ~btn->debounceB0;

    /**
     *     if delta[i] is 0, there is no difference between sample[i] and debouncedBState[i]
     *     if delta[i] is 1, there is a difference between the raw sample[i] and stable,
     *     debounceB0[i] is 0 if delta is 0
     *     debounceB0[i] is unchanged if delta is 1
     *
     *     If no difference between current reading and debounced state, reset 2 byte counter.
     *     If there is a difference, 2 byte counter was incremented previously.
     */
    btn->debounceB0 &= delta;
    btn->debounceB1 &= delta;

    /**
     *     check if the 2 bit counter has overflowed (counted up to 4) and there is a change (delta)
     * if debounceB0 = 0 and debounceB1 = 0 and delta = 1, changes is 1.
     *
     */
    changes = ~(~delta | btn->debounceB0 | btn->debounceB1);

    /**
     *     update the debounced state;
     *     flip the debouncedBState[i] if changes[i] is 1
     */
    btn->debouncedBState ^= changes;

    // return changes;
    return btn->debouncedBState;
}

void buttons_fsm(ButtonBank *btn, uint8_t debouncedBState)
{
    for (int i = 0; i < 8; i++)
    {
        if (_BV(i) & btn->button_mask)
        {
            button_fsm(btn, debouncedBState, i);
        }
    }
}

void button_fsm(ButtonBank *btn, uint8_t debouncedBState, uint8_t index)
{
    switch (btn->_state)
    {
    /**
     *
     */
    case BUTTON_STATE_INIT:

        break;
    /**
     *
     */
    case BUTTON_STATE_DOWN:

        break;
    /**
     *
     */
    case BUTTON_STATE_UP:

        break;
    /**
     *
     */
    case BUTTON_STATE_COUNT:

        break;
    /**
     *
     */
    case BUTTON_STATE_PRESS:

        break;
    /**
     *
     */
    case BUTTON_STATE_PRESSEND:

        break;
    /**
     *
     */
    default:
        break;
    }
}
//