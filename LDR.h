/******************************************************************************
 * File: LDR.h
 *
 * Purpose:
 *     Public interface and configuration of the LDR (light sensor) driver.
 *
 * Hardware connection:
 *     LDR voltage-divider output -> ADC Channel 0 (PA0)
 ******************************************************************************/

/* Include guard: prevents this header from being processed twice in one file. */
#ifndef LDR_H_
#define LDR_H_

#include "std_types.h"        /* uint16 type. */
#include "adc.h"              /* ADC channel names and ADC functions. */
#include "sensors_config.h"   /* ADC reference voltage value. */

/*******************************************************************************
 *                         LDR Sensor Configuration                           *
 *******************************************************************************/

/* ADC channel connected to the LDR circuit */
#define LDR_SENSOR_CHANNEL_ID          ADC_CHANNEL_0

/*
 * Maximum LDR output voltage according to the project requirements.
 * This occurs at the lowest light intensity (darkness).
 * 2.56 V = 2560 mV.
 */
#define LDR_SENSOR_MAX_VOLT_VALUE      2560UL

/* Output scale: 0% to 100% */
#define LDR_SENSOR_MAX_LIGHT_INTENSITY  100U

/*******************************************************************************
 *                            Function Prototype                              *
 *******************************************************************************/

/*
 * Function: LDR_getLightIntensity
 * Description:
 *     Reads the LDR voltage through the ADC and returns an approximate
 *     light-intensity percentage.
 *
 * Return:
 *     uint16 value from 0 to 100.
 *
 * Note:
 *     This percentage is a linear estimate based on the specified
 *     voltage-divider behavior. It is not a calibrated Lux measurement.
 */
uint16 LDR_getLightIntensity(void);

#endif /* LDR_H_ */
