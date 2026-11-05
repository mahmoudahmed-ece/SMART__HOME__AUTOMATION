/******************************************************************************
 * File: pwm.c
 *
 * Purpose:
 *     Implementation of the PWM driver using Timer0 in Fast PWM mode.
 *
 * How Fast PWM works here:
 *     TCNT0 counts from 0 to 255 and then starts again.
 *     The OC0 output goes HIGH at 0 and LOW when TCNT0 reaches OCR0,
 *     so a larger OCR0 keeps the signal HIGH longer (higher duty cycle).
 ******************************************************************************/
#include "pwm.h"             /* Function prototype. */
#include "avr/io.h"          /* Timer0 register names: TCNT0, OCR0, TCCR0. */
#include "common_macros.h"   /* SET_BIT macro. */
/***********************************************************
 *                Functions Definitions                     *
 ***********************************************************/

/*
 * Start Timer0 PWM with the requested duty cycle (0..100 %).
 */
void PWM_Timer0_Start(uint8 duty_cycle){
	/* Step 1: Reset the timer counter so the new PWM period starts from 0. */
	TCNT0 = 0; // Initialize timer with 0 //

	/*
	 * Step 2: Convert the duty cycle from percent (0..100) to a compare
	 * value (0..255). Example: 50 % -> (50 * 255) / 100 = 127.
	 */
	OCR0 = (duty_cycle * 255) / 100; // set Compare value

	/*
	 * Step 3: Configure pins as outputs.
	 * PB3 is the Timer0 PWM output pin (OC0) on the ATmega32, so it must be
	 * an output for the PWM signal to reach the motor driver enable pin.
	 */
	SET_BIT(DDRB,PB3); // PB3/OC0 as output PIN (PWM signal) //
	SET_BIT(DDRC,PC5); // PC5 as output PIN (A)//
	SET_BIT(DDRC,PC6); // PC5 as output PIN (B)//

	/* Set Configuration to Timer0 */
	/*
	 * Step 4: Configure Timer0 in one write to TCCR0:
	 *1- WGM01/WGM00 -> For Fast PWM Mode of Timer0
	 *2- COM01 -> For non-inverted Mode
	 *3- CS01 -> For clock selector (FCPU / 8)
	 * Writing CS01 starts the timer, so the PWM signal begins here.
	 */
	TCCR0 = (1 << WGM00) | (1 << WGM01) | (1 << COM01) | (1 << CS01);
}

