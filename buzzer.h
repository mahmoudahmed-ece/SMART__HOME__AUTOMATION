/******************************************************************************
 * File: buzzer.h
 *
 * Purpose:
 *     Public interface and pin configuration of the buzzer driver.
 *
 * Hardware connection:
 *     Buzzer positive terminal -> PD6
 *     Buzzer negative terminal -> PD7
 ******************************************************************************/
/* Include guard: prevents this header from being processed twice in one file. */
#ifndef BUZZER_H_
#define BUZZER_H_
#include "std_types.h"   /* Project data types. */


/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
/* Port and the two pins connected to the buzzer terminals. */
#define BUZZER_portID           PORTD_ID
#define BUZZER_positive_PinID   PIN6_ID
#define BUZZER_negative_PinID   PIN7_ID

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Configure both buzzer pins as outputs and start with the buzzer off.
 */
void Buzzer_init(void);

/*
 * Turn the buzzer on (positive pin HIGH, negative pin LOW).
 */
void Buzzer_on(void);

/*
 * Turn the buzzer off (both pins LOW, so no voltage across it).
 */
void Buzzer_off(void);


#endif
