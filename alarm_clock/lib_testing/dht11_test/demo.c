#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <stdlib.h>

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "../AVR-DHT/Files/DHT.h"
// #include "../DHT_Sensor_AVR_Library/DHT.h"
#include "../avrSerial/serial.h"

double humidity, temperature;
char buf[30];
char temp_str[6];
char humidity_str[6];
uint16_t temperature_int = 0;
uint16_t humidity_int = 0;
uint8_t raw_data[4];

int main(void)
{
    serialInit(0, BAUD(9600, F_CPU));
    sei();
    // DHT_Setup();
    snprintf(buf, 20, "DHT11 example\n");
    serialWriteString(0, buf);
    temperature = 30.0;
    humidity = 50.0;
    dtostrf(temperature, 6, 2, temp_str);
    dtostrf(humidity, 6, 2, humidity_str);
    snprintf(buf, 30, "%s, %s\n", temp_str, humidity_str);
    serialWriteString(0, buf);

    unsigned char *error = "Error";

    while (1)
    {
        // // call DHT sensor function defined in DHT.c
        // if (dht_GetTempUtil(&temperature_int, &humidity_int) != -1)
        // {
        //     snprintf(buf, 20, "%d, %d\n", temperature_int, humidity_int);
        //     serialWriteString(0, buf);
        // }

        // else
        // {
        //     serialWriteString(0, "error\n");
        // }

        // _delay_ms(1500);

        // Read from sensor
        DHT_Read(&temperature, &humidity);
        // DHT_ReadRaw(raw_data);
        // snprintf(buf, 30, "raw: %d, %d, %d, %d\n", raw_data[0], raw_data[1], raw_data[2], raw_data[3]);
        // serialWriteString(0, buf);

        // Check status
        snprintf(buf, 20, "status = %d\n", DHT_GetStatus());
        serialWriteString(0, buf);
        switch (DHT_GetStatus())
        {
        case (DHT_Ok):
        case (DHT_Error_Humidity):
            dtostrf(temperature, 6, 2, temp_str);
            dtostrf(humidity, 6, 2, humidity_str);
            snprintf(buf, 20, "%s, %s\n", temp_str, humidity_str);
            serialWriteString(0, buf);
            // Print temperature
            // print(temperature[0]);

            // Print humidity
            // print(humidity[0]);
            break;
        case (DHT_Error_Checksum):
            snprintf(buf, 20, "DHT11 err checksum\n");
            serialWriteString(0, buf);
            break;
        case (DHT_Error_Timeout):
            // Do something else
            snprintf(buf, 20, "DHT11 err timeout\n");
            serialWriteString(0, buf);
            break;
        }

        _delay_ms(3000);
    }
    return 0;
}