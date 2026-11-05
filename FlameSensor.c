/******************************************************************************
 * File: FlameSensor.c
 *
 * Purpose:
 *     Implementation of the digital flame sensor driver.
 *     The sensor gives a digital signal, so it only needs the GPIO driver
 *     (no ADC). The application stays in alert mode until the sensor no
 *     longer detects fire.
 ******************************************************************************/

#include "FlameSensor.h"   /* Sensor port/pin configuration and prototypes. */
#include "gpio.h"          /* GPIO functions used to configure and read the pin. */
//The system stays in alert mode until the flame sensor no longer detects fire

/* Step 1: Configure the sensor pin as an input so the MCU can read it. */
void FlameSensor_init(void){
	GPIO_setupPinDirection(FlameSensor_PORT_ID,FlameSensor_PIN_ID,PIN_INPUT);
}

/* Step 2: Read the current level of the sensor pin and return it (0 or 1). */
uint8 FlameSensor_getValue(void){
	return GPIO_readPin(FlameSensor_PORT_ID,FlameSensor_PIN_ID);
}
