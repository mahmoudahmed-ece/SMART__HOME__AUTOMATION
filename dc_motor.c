/******************************************************************************
 * File: dc_motor.c
 *
 * Purpose:
 *     Implementation of the DC motor (fan) driver.
 *     Direction is set with two GPIO pins (PB0, PB1) connected to the motor
 *     driver (e.g. L293D), and speed is set with the Timer0 PWM duty cycle.
 ******************************************************************************/

#include "dc_motor.h" /* header file of DC motor*/
#include "common_macros.h"
#include "gpio.h" /* header file of GPIO*/
#include "avr/io.h" /* To use the DC motor register*/
#include "pwm.h" /* header file of PWM*/
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Initialize the motor direction pins and stop the motor.
 */

void DcMotor_Init(void){
	/* Step 1: Configure the two direction pins as outputs. */
	GPIO_setupPinDirection(PORTB_ID,PIN0_ID,PIN_OUTPUT); /* set PB0 as Output PIN */
	GPIO_setupPinDirection(PORTB_ID,PIN1_ID,PIN_OUTPUT); /* set PB1 as Output PIN*/
	//initialize the motor to be stopped
	/* Step 2: Both pins LOW means the motor is stopped. */
	GPIO_writePin(PORTB_ID,PIN0_ID,LOGIC_LOW);
	GPIO_writePin(PORTB_ID,PIN1_ID,LOGIC_LOW);
}
/*
 * Rotate the motor clockwise, anticlockwise, or stop it, and set its speed.
 * state : stop / clockwise / anticlockwise
 * speed : duty cycle in percent (0..100)
 */
void DcMotor_Rotate(DC_Motor_state state,uint8 speed){
	/*
	 * Step 1: Set the direction.
	 * (PORTB & 0xFC) keeps PB2..PB7 unchanged and clears PB0 and PB1.
	 * (state & 0x03) keeps only the two lowest bits of the state value.
	 * OR-ing them writes the state into PB1:PB0 without touching other pins.
	 */
	PORTB = (PORTB & 0xFC) | (state & 0x03); // Set Direction of DCMotor (CW / A-CW / Stop) based on state argument //
	/* Step 2: Start PWM with the requested duty cycle to set the speed. */
	PWM_Timer0_Start(speed); // Set Duty cycle to PWM Driver Based on Speed Argument //

}
