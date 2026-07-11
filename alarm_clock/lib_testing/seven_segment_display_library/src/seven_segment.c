#include "seven_segment.h"

const uint8_t digit_values[17] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b01110111, // A
    0b01111100, // b
    0b00111001, // C
    0b01011110, // d
    0b01111001, // E
    0b01110001, // F
    0b01110100, // h (for humidity)
};

display_data_t display_data;
Pin segment_pins[8];
Pin digit_pins[4];
Pin colon_pin;
uint32_t prev_millis_flash;
uint8_t flash_colon, flash_digits_minutes, flash_digits_hours, flash_all_digits;

void seven_segment_init()
{
    for (int i = 0; i < 8; i++)
    {
        segment_pins[i].ddr = &SEGMENTS_DDR;
        segment_pins[i].port = &SEGMENTS_PORT;
        segment_pins[i].pin_num = i;
        gpio_set_pin_output(&segment_pins[i]);
    }
    int i1 = 0;
    for (int i = 0; i < 5; i++)
    {
        // digit_pins[i].ddr = &DIGITS_DDR;
        // digit_pins[i].port = &DIGITS_PORT;
        // // digit_pins[i].pin = &PINB;
        // digit_pins[i].pin_num = i;
        // gpio_set_pin_output(&digit_pins[i]);

        // skip PB1
        if (i != 1)
        {
            digit_pins[i1].ddr = &DIGITS_DDR;
            digit_pins[i1].port = &DIGITS_PORT;
            // digit_pins[i].pin = &PINB;
            digit_pins[i1].pin_num = i;
            gpio_set_pin_output(&digit_pins[i1]);
            i1++;
        }
    }

    colon_pin.ddr = &COLON_DDR;
    colon_pin.port = &COLON_PORT;
    colon_pin.pin_num = COLON_PIN_NUM;
    gpio_set_pin_output(&colon_pin);
}

void seven_segment_loop_isr(void)
{

    gpio_set_pin_val(&colon_pin, display_data.colon);
    // DIGITS_PORT |= 0x0F;
    DIGITS_PORT |= 0x1D; // skip PB1
    // DIGITS_PORT &= ~(_BV(3 - display_data.cur_digit_index));
    // if current digit index is enabled, display the digit.
    if (display_data.enable_digits[display_data.cur_digit_index])
    {
        DIGITS_PORT &= ~(_BV(digit_pins[3 - display_data.cur_digit_index].pin_num));
        SEGMENTS_PORT = ~display_data.display_digits[display_data.cur_digit_index];
    }
    display_data.cur_digit_index++;
    if (display_data.cur_digit_index > 3)
    {
        display_data.cur_digit_index = 0;
    }
}

/**
 * @brief
 *
 * @param
 * @return
 */
void seven_segment_clear_all(void)
{
    for (int i = 0; i < 4; i++)
    {
        display_data.display_digits[i] = 0;
        display_data.enable_digits[i] = 0;
    }
    display_data.colon = 0;
}

void seven_segment_set_all(void)
{
    for (int i = 0; i < 4; i++)
    {
        display_data.display_digits[i] = 255;
        display_data.enable_digits[i] = 1;
    }
    display_data.colon = 1;
}

void seven_segment_write_byte_to_digit(uint8_t byte, uint8_t digit)
{
    display_data.display_digits[digit] = byte;
    uint8_t is_flashing;
    switch (digit)
    {
    case 3:
    case 2:
        is_flashing = flash_all_digits | flash_digits_minutes;
        if (!is_flashing)
        {
            display_data.enable_digits[digit] = 1;
        }
        break;
    case 1:
    case 0:
        is_flashing = flash_all_digits | flash_digits_hours;
        if (!is_flashing)
        {
            display_data.enable_digits[digit] = 1;
        }
        break;
    }
}

void seven_segment_write_bytes(uint8_t bytes[4])
{
    for (int i = 0; i < 4; i++)
    {
        seven_segment_write_byte_to_digit(bytes[i], i);
    }
}

void seven_segment_write_digit_val_to_digit(uint8_t digit_index, uint8_t digit)
{
    seven_segment_write_byte_to_digit(digit_values[digit_index], digit);
}

void seven_segment_write_digit_vals(uint8_t digit_indices[4])
{
    for (int i = 0; i < 4; i++)
    {
        seven_segment_write_digit_val_to_digit(digit_indices[i], i);
    }
}

void seven_segment_set_colon(uint8_t state)
{
    display_data.colon = state;
}

void seven_segment_set_decimal_point(uint8_t digit)
{
    display_data.display_digits[digit] |= _BV(7);
}

/* flashing loop */
void seven_segment_flashing_loop(uint32_t t_millis)
{
    if (t_millis - prev_millis_flash > 500)
    {
        prev_millis_flash = t_millis;

        // flash colon
        if (flash_colon)
        {
            if (display_data.colon)
            {
                seven_segment_set_colon(1);
            }
            else
            {
                seven_segment_set_colon(0);
            }
            display_data.colon = !display_data.colon;
        }
        // flash digits 32
        if (flash_digits_minutes)
        {
            if (display_data.enable_digits[3])
            {
                display_data.enable_digits[3] = 0;
                display_data.enable_digits[2] = 0;
            }
            else
            {
                display_data.enable_digits[3] = 1;
                display_data.enable_digits[2] = 1;
            }
        }
        // flash digits 10
        if (flash_digits_hours)
        {
            if (display_data.enable_digits[1])
            {
                display_data.enable_digits[1] = 0;
                display_data.enable_digits[0] = 0;
            }
            else
            {
                display_data.enable_digits[1] = 1;
                display_data.enable_digits[0] = 1;
            }
        }
        // flash all digits
        if (flash_all_digits)
        {
            if (display_data.enable_digits[1])
            {
                display_data.enable_digits[3] = 0;
                display_data.enable_digits[2] = 0;
                display_data.enable_digits[1] = 0;
                display_data.enable_digits[0] = 0;
            }
            else
            {
                display_data.enable_digits[3] = 1;
                display_data.enable_digits[2] = 1;
                display_data.enable_digits[1] = 1;
                display_data.enable_digits[0] = 1;
            }
        }
    }
}
/* flash the colon */
void seven_segment_flash_colon(uint8_t bool_flash)
{
    flash_colon = bool_flash;
}

/* flash digits 3 and 2 */
void seven_segment_flash_digits_minutes(uint8_t bool_flash)
{
    flash_digits_minutes = bool_flash;
}
/* flash digits 1 and 0 */
void seven_segment_flash_digits_hours(uint8_t bool_flash)
{
    flash_digits_hours = bool_flash;
}
/* flash all digits */
void seven_segment_flash_all_digits(uint8_t bool_flash)
{
    flash_all_digits = bool_flash;
}

void seven_segment_show_hour_minute(uint8_t hour, uint8_t minute)
{
    // error checking
    if (hour < 0)
    {
        hour = 0;
    }
    else if (hour > 23)
    {
        hour = 23;
    }
    if (minute < 0)
    {
        minute = 0;
    }
    else if (minute > 59)
    {
        minute = 59;
    }

    uint8_t digit_indices[4];

    // get hour tens digit
    digit_indices[0] = hour / 10;
    // get hour ones digit
    digit_indices[1] = hour % 10;

    // get minute tens digit
    digit_indices[2] = minute / 10;
    // get minute ones digit
    digit_indices[3] = minute % 10;

    // write to display
    seven_segment_write_digit_vals(digit_indices);
}

void seven_segment_show_month_day(uint8_t month, uint8_t day)
{
    // error checking
    if (month < 1)
    {
        month = 1;
    }
    else if (month > 12)
    {
        month = 12;
    }
    if (day < 1)
    {
        day = 1;
    }
    else if (day > 31)
    {
        day = 31;
    }

    uint8_t digit_indices[4];

    // get hour tens digit
    digit_indices[0] = month / 10;
    // get hour ones digit
    digit_indices[1] = month % 10;

    // get minute tens digit
    digit_indices[2] = day / 10;
    // get minute ones digit
    digit_indices[3] = day % 10;

    // write to display
    seven_segment_write_digit_vals(digit_indices);
}

void seven_segment_show_year(uint16_t year)
{
    if (year < 0)
    {
        year = 0;
    }
    else if (year > 9999)
    {
        year = 9999;
    }
    uint8_t digit_indices[4];
    digit_indices[0] = year / 1000;
    digit_indices[1] = (year % 1000) / 100;
    digit_indices[2] = (year % 100) / 10;
    digit_indices[3] = year % 10;
    seven_segment_write_digit_vals(digit_indices);
}

void seven_segment_show_temperature(float temperature_celsius)
{
    int16_t temperature_int;
    // 27.0C
    if (temperature_celsius < -40.)
    {
        temperature_celsius = -40.;
    }
    else if (temperature_celsius > 125.)
    {
        temperature_celsius = 125.;
    }
    uint8_t digit_indices[4];
    // get tens digit
    if (temperature_celsius < 0.)
    {
        temperature_int = temperature_celsius * -10;
    }
    else
    {
        temperature_int = temperature_celsius * 10;
    }

    digit_indices[0] = temperature_int / 100;
    // get ones digit
    digit_indices[1] = (temperature_int % 100) / 10;

    // get tenths digit
    digit_indices[2] = (temperature_int % 10);
    // ones digit gets decimal point
    // last digit get C for celsius
    digit_indices[3] = 0xc;
    seven_segment_write_digit_vals(digit_indices);
    seven_segment_set_decimal_point(1);
    // if temperature sub zero, light up the 0th decimal point
    if (temperature_celsius < 0.)
    {
        seven_segment_set_decimal_point(0);
    }
}

void seven_segment_show_humidity(float humidity)
{
    // 60.0h
    if (humidity < 0.)
    {
        humidity = 0.;
    }
    else if (humidity > 100.)
    {
        humidity = 100.;
    }
    uint8_t digit_indices[4];
    // get tens digit
    int16_t humidity_int = humidity * 10;
    digit_indices[0] = humidity_int / 100;
    // get ones digit
    digit_indices[1] = (humidity_int % 100) / 10;

    // get tenths digit
    digit_indices[2] = (humidity_int % 10);
    // ones digit gets decimal point
    // last digit get C for celsius
    digit_indices[3] = 0x10;
    seven_segment_write_digit_vals(digit_indices);
    seven_segment_set_decimal_point(1);
}