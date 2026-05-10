#include "gpio.h"

void gpio_set_pin_output(Pin *pin)
{
    *pin->ddr |= _BV(pin->pin_num);
}

void gpio_set_pin_input(Pin *pin)
{
    *pin->ddr &= ~_BV(pin->pin_num);
}

void gpio_toggle_pin_mode(Pin *pin)
{
    *pin->ddr ^= _BV(pin->pin_num);
}

uint8_t gpio_read_pin(Pin *pin)
{
    if (*pin->pin & _BV(pin->pin_num))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void gpio_set_pin_high(Pin *pin)
{
    *pin->port |= _BV(pin->pin_num);
}

void gpio_set_pin_low(Pin *pin)
{
    *pin->port &= ~_BV(pin->pin_num);
}

void gpio_toggle_pin_output(Pin *pin)
{
    *pin->port ^= _BV(pin->pin_num);
}