/******************************************************************************
 * File: buzzer.c
 *
 * Purpose:
 *     Implementation of the buzzer driver.
 *     The buzzer is connected between two MCU pins: it sounds when there is
 *     a voltage difference between them (positive HIGH, negative LOW).
 ******************************************************************************/

#include "buzzer.h"   /* Buzzer port/pin configuration and prototypes. */
#include "gpio.h"     /* GPIO functions used to drive the buzzer pins. */

/*
 * Initialize the buzzer and leave it off.
 */
void Buzzer_init(void){
	/* Step 1: Set both buzzer pins as outputs so the MCU can drive them. */
	GPIO_setupPinDirection(BUZZER_portID,BUZZER_positive_PinID,PIN_OUTPUT);
	GPIO_setupPinDirection(BUZZER_portID,BUZZER_negative_PinID,PIN_OUTPUT);

	/* Step 2: Drive both pins LOW (GND), so no current flows and the buzzer is off. */
	GPIO_writePin(BUZZER_portID,BUZZER_positive_PinID,LOGIC_LOW);
	GPIO_writePin(BUZZER_portID,BUZZER_negative_PinID,LOGIC_LOW);
}

/*
 * Turn the buzzer on.
 */
void Buzzer_on(void){
	/* Positive terminal HIGH (+5 V) and negative terminal LOW (GND): current flows and the buzzer sounds. */
	GPIO_writePin(BUZZER_portID,BUZZER_positive_PinID,LOGIC_HIGH);
	GPIO_writePin(BUZZER_portID,BUZZER_negative_PinID,LOGIC_LOW);
}

/*
 * Turn the buzzer off.
 */
void Buzzer_off(void){
	/* Both terminals LOW: no voltage across the buzzer, so it is silent. */
	GPIO_writePin(BUZZER_portID,BUZZER_positive_PinID,LOGIC_LOW);
	GPIO_writePin(BUZZER_portID,BUZZER_negative_PinID,LOGIC_LOW);
}
