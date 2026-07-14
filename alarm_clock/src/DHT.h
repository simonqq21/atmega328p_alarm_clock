#ifndef DHT_H_INCLUDED
#define DHT_H_INCLUDED

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

/*
||
||  Filename:	 		DHT.h
||  Title: 			    DHTxx Driver
||  Author: 			Efthymios Koktsidis
||	Email:				efthymios.ks@gmail.com
||  Compiler:		 	AVR-GCC
||	Description:
||	This library can drive DHT11 and DHT22 sensors.
||
*/

//------ Headers ------//
#include <inttypes.h>
#include <util/delay.h>
#include <avr/io.h>

#include "IO_Macros.h"
#include "DHT_Settings.h"
//----------------------//

//----- Auxiliary data -------------------//
#define DHT11 1
#define DHT22 2
#define DHT_ReadInterval 1500

#define __DHT_Delay_Setup 2000

enum DHT_Status_t
{
	DHT_Ok,
	DHT_Error_Humidity,
	DHT_Error_Temperature,
	DHT_Error_Checksum,
	DHT_Error_Timeout
};
//-----------------------------------------//

//----- Prototypes---------------------------//
void DHT_Setup();
enum DHT_Status_t DHT_GetStatus();
enum DHT_Status_t DHT_ReadRaw(uint8_t Data[4]);
enum DHT_Status_t DHT_GetTemperature(double *Temperature);
enum DHT_Status_t DHT_GetHumidity(double *Humidity);
enum DHT_Status_t DHT_Read(double *Temperature, double *Humidity);
double DHT_CelsiusToFahrenheit(double Temperature);
double DHT_CelsiusToKelvin(double Temperature);
//-------------------------------------------//
#endif
