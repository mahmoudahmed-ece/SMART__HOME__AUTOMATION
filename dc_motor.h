/******************************************************************************
 * File: dc_motor.h
 *
 * Purpose:
 *     Public interface of the DC motor (fan) driver.
 *
 * Hardware connection:
 *     Motor driver direction input 1 -> PB0
 *     Motor driver direction input 2 -> PB1
 *     Speed is controlled by the Timer0 PWM signal (see pwm.c).
 ******************************************************************************/

/* Include guard: prevents this header from being processed twice in one file. */
#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"   /* uint8 type. */

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
/*
 * Motor states. Their numeric values are written directly to PB1:PB0:
 *   DC_Motor_stop          = 0 -> PB1 = 0, PB0 = 0
 *   DC_Motor_clockwise     = 1 -> PB1 = 0, PB0 = 1
 *   DC_Motor_Anticlockwise = 2 -> PB1 = 1, PB0 = 0
 */
typedef enum{
	DC_Motor_stop,DC_Motor_clockwise,DC_Motor_Anticlockwise
}DC_Motor_state;


/*
 * Configure the two direction pins as outputs and start with the motor stopped.
 */
void DcMotor_Init(void);

/*
 * Set the motor direction (stop / clockwise / anticlockwise) and its speed.
 * state : one of the DC_Motor_state values.
 * speed : duty cycle in percent (0..100), passed to the PWM driver.
 */
void DcMotor_Rotate(DC_Motor_state state,uint8 speed);

#endif
