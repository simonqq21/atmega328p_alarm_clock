#ifndef DHT22_H
#define DHT22_H

#include "config.h"
#include "gpio.h"
#include <util/delay.h>

#define MAXSTATECHANGES 85

#define DHT22_DDR DDRC
#define DHT22_PORT PORTC
#define DHT22_PIN PINC
#define DHT22_PIN_NUM 0

// #define DHT22_DDR DDRD
// #define DHT22_PORT PORTD
// #define DHT22_PIN PIND
// #define DHT22_PIN_NUM 7

typedef enum
{
    DHT22_OK,
    DHT22_CHECKSUM_ERROR,
    DHT22_TIMEOUT_ERROR,
} dht22_status_t;

typedef struct
{
    float temperature;
    float humidity;
} dht_values_t;

void dht22_init(void);
dht22_status_t dht22_measure_values(dht_values_t *dht_values);

#endif