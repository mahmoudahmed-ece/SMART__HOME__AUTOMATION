/******************************************************************************
 * File: FlameSensor.h
 *
 * Purpose:
 *     Public interface and pin configuration of the digital flame sensor.
 *
 * Hardware connection:
 *     Flame sensor digital output (OUT) -> PD2
 ******************************************************************************/

/* Include guard: prevents this header from being processed twice in one file. */
#ifndef FLAMESENSOR_H_
#define FLAMESENSOR_H_

/* Port and pin connected to the flame sensor output. */
#define FlameSensor_PORT_ID PORTD_ID
#define FlameSensor_PIN_ID PIN2_ID

#include "std_types.h"   /* uint8 type. */

/* Configure the flame sensor pin as an input. */
void FlameSensor_init(void);

/* Read the sensor output: returns LOGIC_HIGH or LOGIC_LOW. */
uint8 FlameSensor_getValue(void);

#endif /* FLAMESENSOR_H_ */
