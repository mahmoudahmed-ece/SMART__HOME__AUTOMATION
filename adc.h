/*
 * File: adc.h
 * Purpose:
 *     Public interface for the ATmega32 ADC driver.
 *
 * This header exposes:
 *     - ADC constants, such as the maximum result and input channel numbers.
 *     - Configuration types for reference voltage, prescaler, and operating mode.
 *     - Public ADC function declarations used by main.c or another application file.
 *
 * The register-level implementation is kept in adc.c so application code does not
 * need to manipulate ADMUX or ADCSRA directly.
 */

#ifndef ADC_H_
#define ADC_H_

#include "std_types.h" /* Project-specific integer types such as uint8 and uint16. */

/*******************************************************************************
 *                              Definitions                                   *
 *******************************************************************************/

/*
 * ADC_MAXIMUM_VALUE:
 *     Largest numeric value produced by the ATmega32's 10-bit ADC.
 *     10-bit unsigned result range: 0 to (2^10 - 1) = 1023.
 */
#define ADC_MAXIMUM_VALUE       1023U

/*
 * ADC_CHANNEL_x:
 *     Named channel numbers for the ATmega32's eight single-ended ADC inputs.
 *     Passing one of these constants to ADC_startConversion() selects that input.
 */
#define ADC_CHANNEL_0            0U
#define ADC_CHANNEL_1            1U
#define ADC_CHANNEL_2            2U
#define ADC_CHANNEL_3            3U
#define ADC_CHANNEL_4            4U
#define ADC_CHANNEL_5            5U
#define ADC_CHANNEL_6            6U
#define ADC_CHANNEL_7            7U

/*******************************************************************************
 *                              Types Declaration                             *
 *******************************************************************************/

/*
 * ADC_ReferenceType:
 *     Selects the voltage used as the ADC's measurement reference.
 *
 * ADC_REF_AREF:
 *     Use the voltage supplied to the AREF pin.
 *
 * ADC_REF_AVCC:
 *     Use AVCC as the reference. Follow the hardware datasheet recommendations
 *     for AVCC supply and the capacitor connected to AREF.
 *
 * ADC_REF_INTERNAL_2_56V:
 *     Select the ATmega32 internal nominal 2.56 V reference.
 *
 * The measured input voltage must stay within the valid range for the selected
 * reference and the device's electrical limits.
 */
typedef enum
{
    ADC_REF_AREF,
    ADC_REF_AVCC,
    ADC_REF_INTERNAL_2_56V
} ADC_ReferenceType;

/*
 * ADC_PrescalerType:
 *     Selects the division factor between the CPU clock and ADC clock.
 *
 *     ADC clock = F_CPU / selected prescaler.
 *
 * This enum selects a value; it does not calculate or change F_CPU itself.
 * Choose the prescaler based on the actual CPU clock and the required ADC accuracy.
 */
typedef enum
{
    ADC_PRESCALER_2,
    ADC_PRESCALER_4,
    ADC_PRESCALER_8,
    ADC_PRESCALER_16,
    ADC_PRESCALER_32,
    ADC_PRESCALER_64,
    ADC_PRESCALER_128
} ADC_PrescalerType;

/*
 * ADC_ModeType:
 *     Chooses how the application detects conversion completion.
 *
 * ADC_POLLING:
 *     The application checks ADC_isConversionComplete(), which reads the
 *     hardware completion flag when needed.
 *
 * ADC_INTERRUPT:
 *     The ADC completion ISR stores the result and sets a software flag.
 *     Global interrupts must be enabled by the application using sei().
 */
typedef enum
{
    ADC_POLLING,
    ADC_INTERRUPT
} ADC_ModeType;

/*
 * ADC_ConfigType:
 *     Holds the settings used by ADC_init().
 *
 * reference:
 *     Voltage reference choice (AREF, AVCC, or internal 2.56 V).
 *
 * prescaler:
 *     ADC clock division factor.
 *
 * mode:
 *     Polling or Interrupt operation.
 *
 * Example for a 16 MHz CPU:
 *     ADC_REF_AVCC, ADC_PRESCALER_128, ADC_INTERRUPT
 *     gives an ADC clock of 16 MHz / 128 = 125 kHz.
 */
typedef struct
{
    ADC_ReferenceType reference;
    ADC_PrescalerType prescaler;
    ADC_ModeType mode;
} ADC_ConfigType;

/*******************************************************************************
 *                             Function Prototypes                            *
 *******************************************************************************/

/*
 * ADC_init:
 *     Configures the reference, prescaler, operating mode, and ADC enable bit.
 *     Pass the address of an ADC_ConfigType structure.
 */
void ADC_init(const ADC_ConfigType * Config_Ptr);

/*
 * ADC_startConversion:
 *     Starts one conversion on ADC0..ADC7.
 *     Returns immediately; it does not wait for the conversion to complete.
 */
void ADC_startConversion(uint8 channel_num);

/*
 * ADC_isConversionComplete:
 *     Checks whether a conversion result is ready.
 *     Returns 1 when ready, or 0 when the conversion is not complete.
 */
uint8 ADC_isConversionComplete(void);

/*
 * ADC_getResult:
 *     Copies a ready result to the uint16 variable pointed to by Result_Ptr.
 *     Returns 1 on success and 0 if no result is ready or the pointer is NULL.
 */
uint8 ADC_getResult(uint16 * Result_Ptr);

#endif /* ADC_H_ */
