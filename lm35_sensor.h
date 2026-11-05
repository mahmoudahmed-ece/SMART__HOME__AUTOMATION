/******************************************************************************
 * File: lm35_sensor.h
 *
 * Purpose:
 *     Public interface and configuration of the LM35 temperature driver.
 *
 * Hardware connection:
 *     LM35 output -> ADC Channel 7 (PA7)
 ******************************************************************************/

/* Include guard: prevents this header from being processed twice in one file. */
#ifndef LM35_SENSOR_H_
#define LM35_SENSOR_H_

#include "std_types.h"        /* uint8 type. */
#include "adc.h"              /* ADC channel names and ADC functions. */
#include "sensors_config.h"   /* ADC reference voltage value. */

/*******************************************************************************
 *                        LM35 Sensor Configuration                           *
 *******************************************************************************/

/* ADC channel connected to LM35 output */
#define SENSOR_CHANNEL_ID          ADC_CHANNEL_7

/*
 * LM35 sensitivity = 10 mV / degree Celsius.
 * At 150 degrees Celsius, output voltage = 1500 mV.
 */
#define SENSOR_MAX_VOLT_VALUE      1500UL
#define SENSOR_MAX_TEMPERATURE     150U

/*******************************************************************************
 *                            Function Prototype                              *
 *******************************************************************************/

/*
 * Function: LM35_getTemperature
 * Description:
 *     Reads the LM35 analog output and returns temperature in degrees C.
 */
uint8 LM35_getTemperature(void);

#endif /* LM35_SENSOR_H_ */
