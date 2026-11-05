/******************************************************************************
 * File: sensors_config.h
 *
 * Purpose:
 *     Shared voltage configuration for the analog sensor drivers
 *     (LDR and LM35).
 *
 * IMPORTANT:
 *     ADC_REF_VOLT_VALUE must match the reference selected in ADC_init().
 *     The application uses the internal 2.56 V reference, so the value
 *     here is 2560 mV.
 ******************************************************************************/

/* Include guard: prevents this header from being processed twice in one file. */
#ifndef SENSORS_CONFIG_H_
#define SENSORS_CONFIG_H_

/*
 * ADC reference voltage in millivolts (mV).
 * The sensor drivers use it to convert an ADC reading (0..1023)
 * back into a voltage: Voltage(mV) = ADC_value * 2560 / 1023.
 */
#define ADC_REF_VOLT_VALUE    2560UL

#endif /* SENSORS_CONFIG_H_ */
