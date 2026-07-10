#include "button.h"

void button_setup(Button *btn)
{
    /*
    default time values
    */
    gpio_set_pin_input(&btn->button_pin);
    gpio_set_pin_high(&btn->button_pin);
    btn->debounce_ms = 10;
    btn->click_period_timeout_ms = 60;
    btn->longpress_ms = 1000;
    btn->_state = BUTTON_STATE_INIT;
    btn->num_clicks = 0;
    btn->_debouncedBState = 1;
}

void button_ISR_tick(Button *btn)
{
    button_fsm(btn, button_debounce(btn));
}

void button_set_debounce_ms(Button *btn, uint16_t ms)
{
    btn->debounce_ms = ms;
}

void button_set_click_ms(Button *btn, uint16_t ms)
{
    btn->click_period_timeout_ms = ms;
}

void button_set_press_ms(Button *btn, uint16_t ms)
{
    btn->longpress_ms = ms;
}

void button_attach_single_click(Button *btn,
                                button_action_t action)
{
    btn->single_click_action = action;
}

void button_attach_double_click(Button *btn,
                                button_action_t action)
{
    btn->double_click_action = action;
}

void button_attach_long_press(Button *btn,
                              button_action_t action)
{
    btn->long_press_action = action;
}

// , button_action_t f1, button_action_t f2
uint8_t button_debounce(Button *btn)
{
    btn->now = get_millis();
    uint8_t rawState = gpio_read_pin(&btn->button_pin);
    if (btn->_lastDebouncedBState == rawState)
    {
        if ((btn->now - btn->_lastDebouncedTime) > btn->debounce_ms)
        {
            btn->_debouncedBState = rawState;
        }
    }
    else
    {
        btn->_lastDebouncedTime = btn->now;
        btn->_lastDebouncedBState = rawState;
    }
    return btn->_debouncedBState;
}

void _newState(Button *btn, enum button_state state)
{
    btn->_state = state;
    btn->_startTime = get_millis();
}

void button_reset(Button *btn)
{
    btn->_state = BUTTON_STATE_INIT;
    btn->num_clicks = 0;
    btn->_startTime = get_millis();
}

void button_fsm(Button *btn, uint8_t debouncedBState)
{
    uint32_t waitTime = btn->now - btn->_startTime;
    switch (btn->_state)
    {
    /**
     * BUTTON_STATE_INIT is when the button is idle
     */
    case BUTTON_STATE_INIT:
        /**
         * if in BUTTON_STATE_INIT and button debounced state is LOW,
         * save the time to _startTime
         * change state to BUTTON_STATE_DOWN
         */
        if (!debouncedBState)
        {
            btn->num_clicks = 0;
            _newState(btn, BUTTON_STATE_DOWN);
        }
        break;

    /**
     * BUTTON_STATE_DOWN is when the debounced button state is pressed,
     */
    case BUTTON_STATE_DOWN:
        /**
         * if in BUTTON_STATE_DOWN and button debounced state is HIGH,
         * change state to BUTTON_STATE_UP
         */
        if (debouncedBState)
        {
            _newState(btn, BUTTON_STATE_UP);
        }
        /**
         * if in BUTTON_STATE_DOWN and the waiting time exceeds the long press time,
         * run button long press function
         * change button state to BUTTON_STATE_PRESS
         */
        else if (waitTime > btn->longpress_ms)
        {
            if (btn->long_press_action)
                btn->long_press_action();
            _newState(btn, BUTTON_STATE_PRESS);
        }
        break;

    /**
     * BUTTON_STATE_UP is when the debounced button state is released
     */
    case BUTTON_STATE_UP:
        /**
         * If in BUTTON_STATE_UP, increment button click counter then
         * change button state to BUTTON_STATE_COUNT
         */
        btn->num_clicks++;
        _newState(btn, BUTTON_STATE_COUNT);
        break;

    /**
     * BUTTON_STATE_COUNT is when the number of button clicks is counted.
     */
    case BUTTON_STATE_COUNT:
        /**
         * if in BUTTON_STATE_COUNT and button is pressed down,
         * go back to BUTTON_STATE_DOWN and restart timer
         * else if in BUTTON_STATE_COUNT and the waiting time exceeded the
         * click time,
         * execute the appropriate callback based on the number of clicks.
         *
         */
        if (!debouncedBState)
        {
            _newState(btn, BUTTON_STATE_DOWN);
            btn->_startTime = btn->now;
        }
        else if (waitTime > btn->click_period_timeout_ms)
        {
            switch (btn->num_clicks)
            {
            case 1:
                if (btn->single_click_action)
                    btn->single_click_action();
                break;
            default:
                if (btn->double_click_action)
                    btn->double_click_action();
            }
            button_reset(btn);
        }
        break;

    /**
     * BUTTON_STATE_PRESS is after the long press has been acknowledged
     * and the long press callback function has been run.
     */
    case BUTTON_STATE_PRESS:
        /**
         * if button has been released, change state to BUTTON_STATE_PRESSEND.
         */
        if (debouncedBState)
        {
            _newState(btn, BUTTON_STATE_PRESSEND);
        }
        break;

    /**
     * BUTTON_STATE_PRESSEND is right after the button has been lifted from a long press.
     */
    case BUTTON_STATE_PRESSEND:
        /**
         * go back to BUTTON_STATE_IDLE
         */
        button_reset(btn);
        break;

    /**
     * unknown state, go back to BUTTON_STATE_INIT
     */
    default:
        button_reset(btn);
        break;
    }
}

// #include "button.h"

// void button_setup(ButtonBank *btn)
// {
//     // set DDR input
//     *btn->ddr &= ~btn->button_mask;
//     // enable pullup
//     *btn->port |= btn->button_mask;
// }

// void button_ISR_tick(ButtonBank *btn)
// {
//     buttons_fsm(btn, button_debounce(btn));
// }

// void button_set_debounce_ms(ButtonBank *btn, uint16_t ms)
// {
//     btn->debounce_ms = ms;
// }

// void button_set_click_ms(ButtonBank *btn, uint16_t ms)
// {
//     btn->click_ms = ms;
// }

// void button_set_press_ms(ButtonBank *btn, uint16_t ms)
// {
//     btn->longpress_ms = ms;
// }

// void button_attach_single_click(ButtonBank *btn,
//                                 uint8_t index,
//                                 button_action_t action)
// {
//     btn->single_click_actions[index] = action;
// }

// void button_attach_double_click(ButtonBank *btn,
//                                 uint8_t index,
//                                 button_action_t action)
// {
//     btn->double_click_actions[index] = action;
// }

// void button_attach_long_press(ButtonBank *btn,
//                               uint8_t index,
//                               button_action_t action)
// {
//     btn->long_press_actions[index] = action;
// }

// uint8_t button_debounce(ButtonBank *btn)
// {
//     uint8_t delta, changes, sample;
//     btn->now = millis();
//     /**
//      * read sample from PORT
//      */
//     sample = *btn->pin & btn->button_mask;

//     /**
//      *     Set delta to changes from last sample
//      *     sample is the raw input value
//      *     debouncedBState is the stable, "debounced" value
//      */
//     delta = sample ^ btn->debouncedBState;

//     /**
//      * Increment two byte counter
//      */
//     btn->debounceB1 = btn->debounceB1 ^ btn->debounceB0;
//     btn->debounceB0 = ~btn->debounceB0;

//     /**
//      *     if delta[i] is 0, there is no difference between sample[i] and debouncedBState[i]
//      *     if delta[i] is 1, there is a difference between the raw sample[i] and stable,
//      *     debounceB0[i] is 0 if delta is 0
//      *     debounceB0[i] is unchanged if delta is 1
//      *
//      *     If no difference between current reading and debounced state, reset 2 byte counter.
//      *     If there is a difference, 2 byte counter was incremented previously.
//      */
//     btn->debounceB0 &= delta;
//     btn->debounceB1 &= delta;

//     /**
//      *     check if the 2 bit counter has overflowed (counted up to 4) and there is a change (delta)
//      * if debounceB0 = 0 and debounceB1 = 0 and delta = 1, changes is 1.
//      *
//      */
//     changes = ~(~delta | btn->debounceB0 | btn->debounceB1);

//     /**
//      *     update the debounced state;
//      *     flip the debouncedBState[i] if changes[i] is 1
//      */
//     btn->debouncedBState ^= changes;

//     // return changes;
//     return btn->debouncedBState;
// }

// void buttons_fsm(ButtonBank *btn, uint8_t debouncedBState)
// {
//     for (int i = 0; i < 8; i++)
//     {
//         if (_BV(i) & btn->button_mask)
//         {
//             button_fsm(btn, debouncedBState, i);
//         }
//     }
// }

// void _newState(ButtonBank *btn, uint8_t index, enum button_state state)
// {
//     btn->_states[index] = state;
//     btn->_startTimes[index] = millis();
// }

// void button_fsm(ButtonBank *btn, uint8_t debouncedBState, uint8_t index)
// {
//     uint32_t waitTime = btn->now - btn->_startTimes[index];
//     switch (btn->_states[index])
//     {
//     /**
//      * BUTTON_STATE_INIT is when the button is idle
//      */
//     case BUTTON_STATE_INIT:
//         /**
//          * if in BUTTON_STATE_INIT and button debounced state is LOW,
//          * save the time to _startTime
//          * change state to BUTTON_STATE_DOWN
//          */
//         if (!(debouncedBState & _BV(index)))
//         {
//             btn->num_clicks[index] = 0;
//             _newState(btn, index, BUTTON_STATE_DOWN);
//         }
//         break;
//     /**
//      * BUTTON_STATE_DOWN is when the debounced button state is pressed,
//      */
//     case BUTTON_STATE_DOWN:
//         /**
//          * if in BUTTON_STATE_DOWN and button debounced state is HIGH,
//          * change state to BUTTON_STATE_UP
//          */
//         if (debouncedBState & _BV(index))
//         {
//             _newState(btn, index, BUTTON_STATE_UP);
//         }
//         /**
//          * if in BUTTON_STATE_DOWN and the waiting time exceeds the long press time,
//          * run button long press function
//          * change button state to BUTTON_STATE_PRESS
//          */
//         else if (waitTime > btn->longpress_ms)
//         {
//             if (btn->long_press_actions[index])
//                 btn->long_press_actions[index]();
//             _newState(btn, index, BUTTON_STATE_PRESS);
//         }
//         break;
//     /**
//      * BUTTON_STATE_UP is when the debounced button state is released
//      */
//     case BUTTON_STATE_UP:
//         /**
//          * If in BUTTON_STATE_UP, increment counter
//          */
//         break;
//     /**
//      * BUTTON_STATE_COUNT is when the number of button clicks is counted.
//      */
//     case BUTTON_STATE_COUNT:

//         break;
//     /**
//      * BUTTON_STATE_PRESS is after the long press has ben acknowledged
//      * and the long press callback function has been run.
//      */
//     case BUTTON_STATE_PRESS:

//         break;
//     /**
//      * BUTTON_STATE_PRESSEND is right after the button has been lifted from a long press.
//      */
//     case BUTTON_STATE_PRESSEND:

//         break;
//     /**
//      * unknown state, go back to BUTTON_STATE_INIT
//      */
//     default:
//         break;
//     }
// }
// //