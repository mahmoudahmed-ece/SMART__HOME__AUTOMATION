/******************************************************************************
 * File: lm35_sensor.c
 *
 * Purpose:
 *     Implementation of the LM35 temperature sensor driver.
 ******************************************************************************/

#include "lm35_sensor.h"   /* LM35 configuration, ADC functions and data types. */

/*
 * Function: LM35_getTemperature
 *
 * Steps:
 *     1. Start ADC conversion on Channel 7.
 *     2. Wait until conversion is complete.
 *     3. Retrieve the ADC result.
 *     4. Convert the ADC result into degrees Celsius.
 *
 * LM35 characteristic:
 *     10 mV output for every 1 degree Celsius.
 */

uint8 LM35_getTemperature(void)
{
    /* Local variables, both start at 0 so the function returns 0 if the read fails. */
    uint16 adc_value = 0;     /* Raw ADC reading (0..1023). */
    uint8 temperature = 0;    /* Final temperature in degrees Celsius. */

    /* Step 1: Start conversion on the LM35 channel */
    ADC_startConversion(SENSOR_CHANNEL_ID);

    /* Step 2: Wait until conversion is complete */
    while (ADC_isConversionComplete() == 0)
    {
        /* Polling: wait here until the result is ready */
    }

    /* Step 3: Retrieve the completed ADC result (returns 1 on success) */
    if (ADC_getResult(&adc_value) == 1)
    {
        /*
         * Step 4: Convert the reading to degrees Celsius.
         *
         * ADC voltage:
         *     Vout(mV) = ADC_value * Vref(mV) / ADC_MAXIMUM_VALUE
         *
         * LM35 temperature:
         *     Temperature(C) = Vout(mV) / 10
         *
         * Using the configured maximum values:
         *     Temperature =
         *       ADC_value * SENSOR_MAX_TEMPERATURE * ADC_REF_VOLT_VALUE
         *       ---------------------------------------------------------
         *       ADC_MAXIMUM_VALUE * SENSOR_MAX_VOLT_VALUE
         *
         * All voltage values are in millivolts.
         * The cast to uint32 keeps the large multiplication from overflowing.
         */
        temperature = (uint8)(
            ((uint32)adc_value *
             SENSOR_MAX_TEMPERATURE *
             ADC_REF_VOLT_VALUE) /
            (ADC_MAXIMUM_VALUE * SENSOR_MAX_VOLT_VALUE)
        );
    }

    /* Step 5: Return the temperature in degrees Celsius. */
    return temperature;
}
