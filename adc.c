/*
 * File: adc.c
 * Purpose:
 *     Implementation of the ADC (Analog-to-Digital Converter) driver for ATmega32.
 *
 * Main responsibilities:
 *     1. Configure the ADC voltage reference.
 *     2. Configure the ADC clock prescaler.
 *     3. Support Polling mode and Interrupt mode.
 *     4. Start a conversion on one of the ADC0..ADC7 input channels.
 *     5. Provide the completed 10-bit result to the application.
 *
 * Important:
 *     This driver does not calculate the prescaler from F_CPU.
 *     The application chooses the prescaler through ADC_ConfigType.
 *     The ADC clock is therefore: ADC clock = CPU clock / selected prescaler.
 */

#include "avr/io.h"              /* ADC registers and bit names, such as ADMUX, ADCSRA, ADEN. */
#include <avr/interrupt.h>       /* ISR() macro and ADC interrupt vector declaration. */
#include "adc.h"                 /* ADC types, constants, and public function prototypes. */
#include "common_macros.h"       /* Shared bit-manipulation macros used by the project. */

/*
 * g_adcResult:
 *     Stores the most recently completed ADC conversion result.
 *     ATmega32 ADC resolution is 10 bits, so valid results range from 0 to 1023.
 *
 *     volatile is used because this variable may be written inside the ADC ISR
 *     and read by normal application code.
 */
static volatile uint16 g_adcResult = 0;

/*
 * g_conversionComplete:
 *     Software flag:
 *         0 = there is no unread completed conversion result.
 *         1 = a conversion result has been stored and can be retrieved.
 *
 *     volatile is required because the flag can be changed by an interrupt.
 */
static volatile uint8 g_conversionComplete = 0;

/*
 * g_adcMode:
 *     Remembers the operating mode selected during ADC_init().
 *     ADC_POLLING means the application checks the completion flag/status bit.
 *     ADC_INTERRUPT means the ADC completion ISR stores the result.
 */
static ADC_ModeType g_adcMode = ADC_POLLING;

/*
 * ADC_vect - ADC Conversion Complete Interrupt Service Routine:
 *     The CPU enters this ISR when an ADC conversion finishes and the ADC
 *     interrupt is enabled globally and locally.
 *
 *     ADC reads the ADC data registers and provides the 10-bit conversion result.
 *     Store that result in g_adcResult, then set the software completion flag.
 *
 *     Keep the ISR short. Application decisions, calculations, and display updates
 *     should normally be handled outside the ISR.
 */
ISR(ADC_vect)
{
    g_adcResult = ADC;
    g_conversionComplete = 1;
}

/*
 * ADC_init:
 *     Configures and enables the ADC peripheral using the application settings.
 *
 * Config_Ptr:
 *     Pointer to a configuration structure containing:
 *         - reference: voltage reference selection.
 *         - prescaler: division factor for the ADC clock.
 *         - mode: Polling or Interrupt.
 *
 * Register overview:
 *     ADMUX:
 *         Selects the ADC voltage reference and input channel.
 *
 *     ADCSRA:
 *         Controls ADC enable, conversion start, conversion status,
 *         interrupt enable, and ADC clock prescaler.
 *
 * F_CPU note:
 *     This function does not use F_CPU in a calculation. The prescaler is chosen
 *     directly from Config_Ptr->prescaler. The application must choose a suitable
 *     prescaler for the actual CPU clock frequency.
 */
void ADC_init(const ADC_ConfigType *Config_Ptr)
{
    /*
     * ADMUX - ADC Multiplexer Selection Register:
     *     Clear the register before configuring it.
     *     This starts with reference bits REFS1:0 = 00 (AREF pin as reference)
     *     and channel-selection bits MUX4:0 = 00000 (ADC0).
     *
     *     This driver configures the reference below. The input channel is selected
     *     later in ADC_startConversion().
     */
    ADMUX = 0;

    /*
     * Voltage reference selection uses ADMUX bits REFS1 and REFS0:
     *
     *     REFS1:REFS0 = 00 -> external voltage on AREF pin.
     *     REFS1:REFS0 = 01 -> AVCC reference, with a capacitor on AREF recommended.
     *     REFS1:REFS0 = 11 -> internal 2.56 V reference on ATmega32.
     *
     * ADC_REF_AREF needs no extra bit to be set because ADMUX was cleared above.
     */
    if (Config_Ptr->reference == ADC_REF_AVCC)
        ADMUX |= (1 << REFS0);
    else if (Config_Ptr->reference == ADC_REF_INTERNAL_2_56V)
        ADMUX |= (1 << REFS1) | (1 << REFS0);

    /*
     * ADCSRA - ADC Control and Status Register A:
     *     ADEN = 1 enables the ADC peripheral.
     *
     *     This assignment also clears the other ADCSRA bits at initialization,
     *     including the prescaler bits and ADIE. The required settings are added
     *     in the following code.
     */
    ADCSRA = (1 << ADEN);

    /*
     * ADC prescaler selection uses ADPS2:ADPS0 in ADCSRA.
     *
     * The prescaler divides the CPU clock to create the ADC clock:
     *     ADC_clock = F_CPU / prescaler
     *
     * The selected prescaler is supplied by the application. For example,
     * at F_CPU = 16 MHz, prescaler 128 gives an ADC clock of 125 kHz.
     *
     * Typical ATmega32 guidance for full 10-bit resolution is an ADC clock
     * around 50-200 kHz. Check the device datasheet for the exact operating limits.
     *
     * ADPS2:ADPS0 mapping:
     *     000 -> 2
     *     001 -> 2
     *     010 -> 4
     *     011 -> 8
     *     100 -> 16
     *     101 -> 32
     *     110 -> 64
     *     111 -> 128
     *
     * The enum ADC_PRESCALER_2 uses the 001 encoding here; this is a valid
     * divide-by-2 selection on ATmega32.
     */
    if (Config_Ptr->prescaler == ADC_PRESCALER_2)
        ADCSRA |= (1 << ADPS0);
    else if (Config_Ptr->prescaler == ADC_PRESCALER_4)
        ADCSRA |= (1 << ADPS1);
    else if (Config_Ptr->prescaler == ADC_PRESCALER_8)
        ADCSRA |= (1 << ADPS1) | (1 << ADPS0);
    else if (Config_Ptr->prescaler == ADC_PRESCALER_16)
        ADCSRA |= (1 << ADPS2);
    else if (Config_Ptr->prescaler == ADC_PRESCALER_32)
        ADCSRA |= (1 << ADPS2) | (1 << ADPS0);
    else if (Config_Ptr->prescaler == ADC_PRESCALER_64)
        ADCSRA |= (1 << ADPS2) | (1 << ADPS1);
    else
        /* The remaining enum option is ADC_PRESCALER_128: ADPS2:ADPS1:ADPS0 = 111. */
        ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    /*
     * Save the mode selected by the application.
     * ADC_isConversionComplete() uses this value to decide whether it should
     * inspect ADIF itself or rely on the interrupt routine.
     */
    g_adcMode = Config_Ptr->mode;

    /*
     * ADIE - ADC Interrupt Enable bit in ADCSRA:
     *     ADIE = 1 enables the ADC conversion-complete interrupt request.
     *
     * This is the peripheral-level interrupt enable. The AVR global interrupt
     * flag must also be enabled by the application using sei().
     */
    if (g_adcMode == ADC_INTERRUPT)
        ADCSRA |= (1 << ADIE);
}

/*
 * ADC_startConversion:
 *     Selects an ADC input channel and starts one conversion.
 *     This function does not wait for the conversion to finish.
 *
 * channel_num:
 *     Channel number from ADC_CHANNEL_0 through ADC_CHANNEL_7.
 *
 * ADSC - ADC Start Conversion bit:
 *     Writing 1 starts a conversion. While a conversion is in progress,
 *     ADSC remains set. The check below prevents this function from starting
 *     another conversion while one is already running.
 */
void ADC_startConversion(uint8 channel_num)
{
    /* Reject channel numbers outside the ATmega32 ADC0..ADC7 range. */
    if (channel_num > ADC_CHANNEL_7)
        return;

    /* If ADSC is still 1, a conversion is already in progress; do not restart it. */
    if (ADCSRA & (1 << ADSC))
        return;

    /*
     * Select the ADC channel using ADMUX MUX bits.
     * (ADMUX & 0xE0) preserves the top three bits (including reference selection),
     * while clearing the lower five channel-selection bits.
     * OR with channel_num inserts the requested channel number.
     */
    ADMUX = (ADMUX & 0xE0) | channel_num;

    /*
     * Clear the software completion flag before starting a new conversion.
     * This prevents an old result from being mistaken for the new conversion.
     */
    g_conversionComplete = 0;

    /*
     * Set ADSC to begin the conversion.
     * In Polling mode, the application checks for completion.
     * In Interrupt mode, ADC_vect runs when the conversion finishes.
     */
    ADCSRA |= (1 << ADSC);
}

/*
 * ADC_isConversionComplete:
 *     Reports whether the current conversion result is ready.
 *
 * Polling mode:
 *     ADIF is the ADC Interrupt Flag in ADCSRA. Hardware sets ADIF when a
 *     conversion completes. The function checks ADIF, reads the ADC result,
 *     stores it, sets the software flag, and clears ADIF by writing a 1 to it.
 *
 * Interrupt mode:
 *     The ISR stores the result and sets g_conversionComplete, so this function
 *     only returns the software flag.
 *
 * Return:
 *     1 if a completed result is ready; otherwise 0.
 */
uint8 ADC_isConversionComplete(void)
{
    if (g_adcMode == ADC_POLLING)
    {
        /* Check whether hardware has set the conversion-complete flag. */
        if (ADCSRA & (1 << ADIF))
        {
            /* Read and save the completed 10-bit ADC result. */
            g_adcResult = ADC;
            g_conversionComplete = 1;

            /*
             * ADIF is cleared by writing a 1 to it on this AVR.
             * This acknowledges the completed conversion so a later conversion
             * can set the flag again.
             */
            ADCSRA |= (1 << ADIF);
        }
    }

    /* Return the software status flag in either operating mode. */
    return g_conversionComplete;
}

/*
 * ADC_getResult:
 *     Copies a ready conversion result into the application variable.
 *
 * Result_Ptr:
 *     Pointer to a uint16 variable supplied by the application. The ADC result
 *     is 10-bit, but uint16 safely holds the value from 0 to 1023.
 *
 * Return:
 *     1 if a result was copied successfully.
 *     0 if Result_Ptr is NULL or no completed result is currently available.
 *
 * After returning the result, this function clears the software completion flag,
 * marking the stored result as consumed. The application can then start another
 * conversion and check for the next result.
 */
uint8 ADC_getResult(uint16 *Result_Ptr)
{
    /* Validate the destination pointer and make sure a result is ready. */
    if (Result_Ptr == 0 || g_conversionComplete == 0)
        return 0;

    /* Copy the stored conversion result to the variable owned by the application. */
    *Result_Ptr = g_adcResult;

    /* Mark the result as consumed so it is not read again as a new result. */
    g_conversionComplete = 0;

    return 1;
}
