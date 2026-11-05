/******************************************************************************
 * File: LDR.c
 *
 * Purpose:
 *     Implementation of the LDR (light sensor) driver.
 ******************************************************************************/

#include "LDR.h"   /* LDR configuration, ADC functions and data types. */

/*
 * Function: LDR_getLightIntensity
 *
 * Steps:
 *     1. Start ADC conversion on Channel 0.
 *     2. Wait until conversion is complete.
 *     3. Read the ADC result.
 *     4. Convert the reading into an approximate light percentage.
 *
 * Project requirement:
 *     The LDR output voltage is maximum in darkness.
 *     Therefore:
 *         High ADC reading -> Low light intensity
 *         Low ADC reading  -> High light intensity
 */

uint16 LDR_getLightIntensity(void)
{
    /* Local variables, all start at 0 so the function returns 0 if the read fails. */
    uint16 adc_value = 0;            /* Raw ADC reading (0..1023). */
    uint16 voltage_percentage = 0;   /* Reading as a percentage of full scale. */
    uint16 light_intensity = 0;      /* Final light percentage returned. */

    /* Step 1: Start conversion on the LDR channel */
    ADC_startConversion(LDR_SENSOR_CHANNEL_ID);

    /* Step 2: Wait for the ADC conversion to finish */
    while (ADC_isConversionComplete() == 0)
    {
        /* Polling: wait here until the result is ready */
    }

    /* Step 3: Retrieve the completed conversion result (returns 1 on success) */
    if (ADC_getResult(&adc_value) == 1)
    {
        /*
         * Step 4: Map the ADC reading (0..1023) to a percentage (0..100).
         * The value is cast to uint32 first so the multiplication
         * (up to 1023 * 100) does not overflow a 16-bit number.
         *
         * This assumes the LDR voltage-divider output spans approximately
         * 0 V to 2.56 V. Actual endpoints depend on the circuit.
         */
        voltage_percentage = (uint16)(
            ((uint32)adc_value * LDR_SENSOR_MAX_LIGHT_INTENSITY) /
            ADC_MAXIMUM_VALUE
        );

        /*
         * Step 5: Invert the result because the project specification states
         * that the LDR output voltage is highest in darkness.
         * Example: 80 % voltage -> 100 - 80 = 20 % light.
         */
        light_intensity =
            LDR_SENSOR_MAX_LIGHT_INTENSITY - voltage_percentage;
    }

    /* Step 6: Return the light intensity (0..100 %). */
    return light_intensity;
}
