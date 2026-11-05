/******************************************************************************
 * File: SmartHome.c
 *
 * Purpose:
 *     Main application of the ATmega32 Smart Home system.
 *     Reads the light (LDR), temperature (LM35) and flame sensors, then:
 *       - controls the RGB LEDs according to the light level,
 *       - controls the fan (DC motor) speed according to the temperature,
 *       - shows the readings on the LCD,
 *       - raises an alarm (buzzer + LCD message) when a flame is detected.
 *
 * Configuration assumptions:
 *     - CPU clock F_CPU = 16 MHz.
 *     - LDR uses ADC_CHANNEL_0; LM35 uses ADC_CHANNEL_7.
 *     - ADC reference is the internal 2.56 V reference.
 *     - ADC_PRESCALER_128 -> ADC clock = 16 MHz / 128 = 125 kHz
 *       (inside the recommended 50..200 kHz range).
 *     - The flame logic assumes HIGH means flame detected.
 *       Verify the sensor (or Proteus model) OUT polarity.
 *     - sensors_config.h defines ADC_REF_VOLT_VALUE as 2560UL (mV), matching
 *       the 2.56 V reference used in ADC_init().
 ******************************************************************************/

#include <util/delay.h>       /* _delay_ms() used to hold the start-up message. */

#include "std_types.h"        /* Project data types. */
#include "adc.h"              /* ADC configuration types and ADC_init(). */
#include "sensors_config.h"   /* ADC reference voltage value. */

#include "lcd.h"              /* LCD driver. */
#include "LED.h"              /* RGB LED driver. */
#include "buzzer.h"           /* Buzzer driver. */
#include "dc_motor.h"         /* Fan (DC motor) driver. */
#include "FlameSensor.h"      /* Flame sensor driver. */
#include "lm35_sensor.h"      /* Temperature sensor driver. */
#include "LDR.h"              /* Light sensor driver. */

/* Application function prototypes (static = used only inside this file). */
static void Automatic_Lighting_Control(uint16 light_intensity);
static void Automatic_Fan_Speed_Control(uint8 room_temp);
static void LCD_Display_Real_Time_Feedback(uint16 light_intensity,
                                           uint8 room_temp);
static void Handle_Flame_Alert(void);

int main(void)
{
    /*
     * Step 1: ADC configuration: reference, ADC clock prescaler, and polling mode.
     * The values are listed in the same order as the ADC_ConfigType fields:
     * reference, prescaler, mode.
     */
    const ADC_ConfigType adc_config =
    {
        ADC_REF_INTERNAL_2_56V,
		 ADC_PRESCALER_128,
        ADC_POLLING
    };

    /* Step 2: Initialize every driver once, before the main super loop. */
    ADC_init(&adc_config);
    Buzzer_init();
    DcMotor_Init();
    FlameSensor_init();
    LCD_init();
    LEDS_init();

    /* Step 3: Establish known initial output states (buzzer silent, fan stopped). */
    Buzzer_off();
    DcMotor_Rotate(DC_Motor_stop, 0);

    /* Step 4: Show a start-up message on both LCD rows. */
    LCD_clearScreen();
    LCD_displayString("Smart Home");
    LCD_displayStringRowColumn(1, 0, "System Ready");

    /*
     * Keep the start-up message visible for 1 second, then clear the screen
     * so the normal display starts from a blank LCD.
     */
    _delay_ms(1000);
    LCD_clearScreen();

    /* Step 5: Super loop: repeat the read -> decide -> update cycle forever. */
    while (1)
    {
        /*
         * Step 5.1: Read sensors sequentially:
         * LDR uses ADC0, then LM35 uses ADC7. The ADC peripheral performs
         * one conversion at a time; the sensor-driver calls wait for completion.
         */
        uint16 light_intensity = LDR_getLightIntensity();
        uint8 room_temp = LM35_getTemperature();

        /* Step 5.2: Digital flame sensor read. HIGH is assumed to mean flame detected. */
        uint8 flame_flag = FlameSensor_getValue();

        /*
         * Step 5.3: Flame alert has priority over normal automation.
         * After handling the alert, continue starts a fresh iteration so that
         * old temperature/light readings are not used to restart the fan.
         */
        if (flame_flag == LOGIC_HIGH)
        {
            Handle_Flame_Alert();
            continue;
        }

        /* Step 5.4: Normal mode: display measurements and update outputs. */
        LCD_Display_Real_Time_Feedback(light_intensity, room_temp);
        Automatic_Lighting_Control(light_intensity);
        Automatic_Fan_Speed_Control(room_temp);
    }

    return 0; /* The super loop normally never exits. */
}

/* Stop the fan, sound the buzzer, and wait until the flame input clears. */
static void Handle_Flame_Alert(void)
{
    /* Step 1: Stop the fan (it would feed the fire) and turn on the alarm. */
    DcMotor_Rotate(DC_Motor_stop, 0);
    Buzzer_on();

    /* Step 2: Show the alert message on the LCD. */
    LCD_clearScreen();
    LCD_displayString("Critical alert!");

    /*
     * Step 3: Blocking polling loop: while the input remains HIGH, the application
     * stays here and does not update the normal lighting/fan functions.
     */
    while (FlameSensor_getValue() == LOGIC_HIGH)
    {
        /* Re-read the digital input continuously. */
    }

    /* Step 4: Flame is gone: silence the buzzer and clear the alert message. */
    Buzzer_off();
    LCD_clearScreen();
}

/* Apply the RGB LED pattern for the measured light-intensity range. */
static void Automatic_Lighting_Control(uint16 light_intensity)
{
    /* Step 1: Clear the previous pattern first to avoid stale LEDs between ranges. */
    LED_off(RED);
    LED_off(GREEN);
    LED_off(BLUE);

    /* Step 2: Turn on the LEDs that match the current light range. */
    if (light_intensity <= 15)
    {
        /* 0..15 %: very low light: all RGB channels ON. */
        LED_on(RED);
        LED_on(GREEN);
        LED_on(BLUE);
    }
    else if (light_intensity <= 50)
    {
        /*
         * 16..50 %: GREEN only.
         * The supplied sample's final state for this range is GREEN only:
         * it turned RED and GREEN ON, then turned RED OFF.
         */
        LED_on(GREEN);
    }
    else if (light_intensity <= 70)
    {
        /* 51..70 %: moderate light: RED only. */
        LED_on(RED);
    }
    else
    {
        /* Above 70%: all channels remain OFF. */
    }
}

/* Control the fan using the project's DC Motor driver and PWM speed argument. */
static void Automatic_Fan_Speed_Control(uint8 room_temp)
{
    /*
     * Check the temperature from the highest range down, so the first
     * matching range sets the fan speed (duty cycle in %):
     *   >= 40 C -> 100 %, >= 35 C -> 75 %, >= 30 C -> 50 %,
     *   >= 25 C -> 25 %,  below 25 C -> fan off.
     * Each branch also updates the fan status text on LCD row 0, column 4.
     */
    if (room_temp >= 40)
    {
        DcMotor_Rotate(DC_Motor_clockwise, 100);
        LCD_displayStringRowColumn(0, 4, "FAN IS ON ");
    }
    else if (room_temp >= 35)
    {
        DcMotor_Rotate(DC_Motor_clockwise, 75);
        LCD_displayStringRowColumn(0, 4, "FAN IS ON ");
    }
    else if (room_temp >= 30)
    {
        DcMotor_Rotate(DC_Motor_clockwise, 50);
        LCD_displayStringRowColumn(0, 4, "FAN IS ON ");
    }
    else if (room_temp >= 25)
    {
        DcMotor_Rotate(DC_Motor_clockwise, 25);
        LCD_displayStringRowColumn(0, 4, "FAN IS ON ");
    }
    else
    {
        /* Below 25 C: stop the fan. The extra space in "FAN IS ON " above
         * makes both messages 10 characters long, so no old letter is left. */
        DcMotor_Rotate(DC_Motor_stop, 0);
        LCD_displayStringRowColumn(0, 4, "FAN IS OFF");
    }
}

/* Show the current temperature and LDR percentage on the 16x2 LCD. */
static void LCD_Display_Real_Time_Feedback(uint16 light_intensity,
                                           uint8 room_temp)
{
    /*
     * Row 0 is not cleared here: the fan-status text always has the same
     * length (10 characters) and overwrites itself, so rewriting it in place
     * avoids the flicker that clearing the row on every loop would cause.
     * Row 1 is laid out as two fixed 8-character fields:
     *     "TEMP=xx " (columns 0..7) and "LDR=xx %" (columns 8..15).
     */

    /* Step 1: Temperature field: row 1, columns 0 onward. */
    LCD_displayStringRowColumn(1, 0, "TEMP=");
    LCD_intgerToString(room_temp);

    /*
     * Step 2: Pad with spaces so the field is always 8 characters wide:
     * one extra space for values below 100 and another for values below 10.
     * This also overwrites any old digit left from a longer previous value.
     */
    if (room_temp < 100)
    {
        LCD_displayCharacter(' ');
    }
    if (room_temp < 10)
    {
        LCD_displayCharacter(' ');
    }

    /* Step 3: Light intensity field: row 1, starting at column 8. */
    LCD_displayStringRowColumn(1, 8, "LDR=");
    LCD_intgerToString(light_intensity);

    /* Step 4: Same padding, so "%" always lands in column 15 and no old character remains. */
    if (light_intensity < 100)
    {
        LCD_displayCharacter(' ');
    }
    if (light_intensity < 10)
    {
        LCD_displayCharacter(' ');
    }

    /* Step 5: Print the percent sign after the light value. */
    LCD_displayCharacter('%');
}
