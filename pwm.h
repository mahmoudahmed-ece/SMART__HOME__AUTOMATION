/******************************************************************************
 * File: pwm.h
 *
 * Purpose:
 *     Public interface of the PWM driver (Timer0, Fast PWM mode).
 *     Used by the DC motor driver to control the fan speed.
 ******************************************************************************/

/* Include guard: prevents this header from being processed twice in one file. */
#ifndef PWM_H_
#define PWM_H_
#include "std_types.h"   /* uint8 type. */
/**********************************************************
 *                Functions Prototypes                     *
 **********************************************************/


/*
 * Start Timer0 in Fast PWM, non-inverting mode with the given duty cycle.
 *   1- Fast PWM mode, non-inverting output on OC0.
 *   2- Timer clock = F_CPU / 8.
 *   3- Compare value (OCR0) calculated from the duty cycle.
 *   4- PWM frequency = F_CPU / (8 * 256):
 *        at 1 MHz  -> about 488 Hz
 *        at 16 MHz -> about 7.8 kHz
 * duty_cycle : percentage from 0 to 100, passed by the DC motor driver.
 */
void PWM_Timer0_Start(uint8 duty_cycle);


#endif /* PWM_H_ */
