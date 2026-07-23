#include "dht22.h"

Pin DHT22_pin;
uint8_t dht22_raw_values[5];

void dht22_init(void)
{
    DHT22_pin.ddr = &DHT22_DDR;
    DHT22_pin.port = &DHT22_PORT;
    DHT22_pin.pin = &DHT22_PIN;
    DHT22_pin.pin_num = DHT22_PIN_NUM;
}

/**
 * @brief read temperature and humidity values from the DHT22 sensor
 *
 */
dht22_status_t dht22_measure_values(dht_values_t *dht_values)
{
    uint8_t laststate = 1;
    uint8_t counter = 0; // counts the time duration of one 1 or 0 state
    uint8_t j = 0;       // counts the number of bits read

    /*
    initiate communication with DHT22
    */
    gpio_set_pin_output(&DHT22_pin);
    gpio_set_pin_low(&DHT22_pin);
    _delay_ms(18);
    gpio_set_pin_input(&DHT22_pin);

    /*
    Pin state change detection loop
    Wait for sensor to respond with a pull down
    grab the 40 bytes that the DHT22 returns
    */
    // for each pin state change
    for (int i = 0; i < MAXSTATECHANGES; i++)
    {
        counter = 0;
        // wait for pin to change state
        while (laststate == gpio_read_pin(&DHT22_pin))
        {
            counter++;
            if (counter >= 250)
            {
                break;
            }
            _delay_us(1);
        }
        laststate = gpio_read_pin(&DHT22_pin);
        if (counter >= 250)
        {
            if (i < 80)
                return DHT22_TIMEOUT_ERROR;
            break;
        }
        /*
        Ignore the first two state changes.
        After the MCU pulls the pin low for at least 18 ms and returns it high,
        the DHT22 will respond by pulling the pin low for 80 us and then pulling
        it high for 80 us.

        When the DHT22 outputs a '0', it pulls the pin low for 50 us
        then pulls it high for 26-28 us.
        When the DHT22 outputs a '1', it pulls the pin low for 50 us
        then pulls it high for 70 us.
        */
        if (i >= 4)
        {
            // if the pin is currently low
            if (laststate == 0)
            {
                // shift left
                dht22_raw_values[j / 8] <<= 1;

                // check if bit is 0 or 1
                if (counter >= 9) // 12
                {
                    dht22_raw_values[j / 8] |= 1;
                }
                j++;
            }
        }
    }

    /*
    check the checksum and convert the raw data into temperature and humidity
    data
*/
    if (j >= 40 && dht22_raw_values[4] == ((dht22_raw_values[0] + dht22_raw_values[1] + dht22_raw_values[2] + dht22_raw_values[3]) & 0xff))
    {
        dht_values->temperature = (float)(dht22_raw_values[2] & 0x7f) * 256 + (float)dht22_raw_values[3];
        dht_values->humidity = (float)dht22_raw_values[0] * 256 + (float)dht22_raw_values[1];
        dht_values->temperature /= 10;
        dht_values->humidity /= 10;
        if (dht22_raw_values[2] & 0x80)
        {
            dht_values->temperature *= -1;
        }
        return DHT22_OK;
    }
    else
        return DHT22_CHECKSUM_ERROR;
}