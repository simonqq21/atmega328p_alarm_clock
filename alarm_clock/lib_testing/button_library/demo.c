#include "src/button.h"
#include "src/gpio.h"
#include "src/millis_micros.h"

Pin segment_pins[8];
Pin common_pin;
Button btns[3];
// Pin btn_pin;
/*
IO pins
 */
#define LEDS_PORT PORTD
#define LEDS_DDR DDRD
#define LED_1_PIN_NUM PD0
#define LED_2_PIN_NUM PD1
#define LED_3_PIN_NUM PD2

#define COMMON_LED_PORT PORTB
#define COMMON_LED_PIN PB0
#define COMMON_LED_DDR DDRB

#define BTNS_PORT PORTC
#define BTNS_DDR DDRC
#define BTNS_PIN PINC
#define BTN_1_PIN_NUM PC1 // down
#define BTN_2_PIN_NUM PC2 // reset
#define BTN_3_PIN_NUM PC3 // up

#define seven_seg_port PORTB

const uint8_t digit_values[16] = {
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
};
int8_t cur_digit;

ISR(TIMER0_COMPA_vect)
{
    millis_timer_ISR_loop();
}

void ioinit()
{
    for (int i = 0; i < 8; i++)
    {
        segment_pins[i].ddr = &LEDS_DDR;
        segment_pins[i].port = &LEDS_PORT;
        segment_pins[i].pin_num = i;
        gpio_set_pin_output(&segment_pins[i]);
    }

    common_pin.ddr = &COMMON_LED_DDR;
    common_pin.port = &COMMON_LED_PORT;
    common_pin.pin_num = COMMON_LED_PIN;

    btns[0].button_pin.ddr = &BTNS_DDR;
    btns[0].button_pin.port = &BTNS_PORT;
    btns[0].button_pin.pin = &BTNS_PIN;
    btns[0].button_pin.pin_num = BTN_1_PIN_NUM;
    btns[1].button_pin.ddr = &BTNS_DDR;
    btns[1].button_pin.port = &BTNS_PORT;
    btns[1].button_pin.pin = &BTNS_PIN;
    btns[1].button_pin.pin_num = BTN_2_PIN_NUM;
    btns[2].button_pin.ddr = &BTNS_DDR;
    btns[2].button_pin.port = &BTNS_PORT;
    btns[2].button_pin.pin = &BTNS_PIN;
    btns[2].button_pin.pin_num = BTN_3_PIN_NUM;

    gpio_set_pin_output(&segment_pins[0]);
    gpio_set_pin_output(&segment_pins[1]);
    gpio_set_pin_output(&segment_pins[2]);
    gpio_set_pin_output(&common_pin);
    button_setup(&btns[0]);
    button_setup(&btns[1]);
    button_setup(&btns[2]);
    // gpio_set_pin_input(&btn_pin);
    // gpio_set_pin_high(&btn_pin);

    gpio_set_pin_low(&common_pin);
}

void btn1_1click_func()
{
    // gpio_toggle_pin_output(&segment_pins[0]);
    cur_digit--;
    if (cur_digit < 0)
    {
        cur_digit = 15;
    }
}

void btn2_1click_func()
{
    // gpio_toggle_pin_output(&segment_pins[1]);
    cur_digit = 0;
}

void btn3_1click_func()
{
    // gpio_toggle_pin_output(&segment_pins[2]);
    cur_digit++;
    if (cur_digit > 15)
    {
        cur_digit = 0;
    }
}

void f1()
{
    gpio_set_pin_high(&common_pin);
}

void f2()
{
    gpio_set_pin_low(&common_pin);
}

uint8_t reading;

int main(void)
{
    ioinit();
    millis_timer_init();
    button_attach_single_click(&btns[0], btn1_1click_func);
    button_attach_single_click(&btns[1], btn2_1click_func);
    button_attach_single_click(&btns[2], btn3_1click_func);
    button_attach_double_click(&btns[0], btn3_1click_func);
    button_attach_long_press(&btns[0], btn2_1click_func);

    while (1)
    {
        // reading = gpio_read_pin(&btn_pin);
        // // reading = 1;
        // if (reading)
        // {
        //     gpio_set_pin_high(&common_pin);
        // }
        // else
        // {
        //     gpio_set_pin_low(&common_pin);
        // }
        button_loop(&btns[0]);
        button_loop(&btns[1]);
        button_loop(&btns[2]);
        // button_debounce(&btns[0], f1, f2);
        //)
        PORTD = ~digit_values[cur_digit];
    }
}
