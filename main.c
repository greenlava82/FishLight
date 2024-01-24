 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.0
*/

/*
© [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#include "mcc_generated_files/system/system.h"

/*
    Main application
*/

#define PWM_PERIOD          10000ul
#define BUTTON_ASSERTED     0
#define BUTTON_IDLE         1
#define BUTTON_HOLD_TIME    30

// LED Values
#define LED_MIN_VAL         0.02f
#define LED_OFF_VAL         0.0f
#define LED_MAX_VAL         1.0f
#define LED_DAYLIGHT_VAL    0.85f

// Time Values
#define TIME_FULL_CYCLE     864000ul
#define TIME_SUNRISE        18000ul
#define TIME_SUNSET         18000ul
#define TIME_DAYLIGHT       432000ul
#define TIME_EVENING        216000ul
#define TIME_NIGHT          (TIME_FULL_CYCLE - TIME_SUNRISE - TIME_SUNSET - TIME_DAYLIGHT - TIME_EVENING)

// Fading Math
#define FADE_SUNRISE_STEP  ((LED_DAYLIGHT_VAL - LED_MIN_VAL) / TIME_SUNRISE)
#define FADE_SUNSET_STEP   ((LED_DAYLIGHT_VAL - LED_MIN_VAL) / TIME_SUNSET)
#define FADE_FAST_STEP     0.001f

typedef enum {
    LED_GREEN = 0,
    LED_BLUE,
    LED_RED,
    LED_ALL
}LEDColor_t;

typedef enum {
    BUTTON_RELEASED = 0,
    BUTTON_NEWPRESS,
    BUTTON_PRESSED,
    BUTTON_NEWHOLD,
    BUTTON_HOLD   
}BUTTONState_t;

enum SM_State {
    SM_STARTUP = 0,
    SM_SUNRISE,
    SM_DAYTIME,
    SM_EVENING,
    SM_SUNSET,
    SM_NIGHT
};

enum SM_State State = SM_STARTUP;
BUTTONState_t buttonState = BUTTON_RELEASED;
uint8_t tmrCounter = 0;
bool tmrFlag = false;

void cbTmrCounter(void) {
    tmrFlag = true;
}

bool buttonPress = false;

void cbButtonChange(void) {
    buttonPress = true;
}

uint8_t setDutyCycle(LEDColor_t channel, float value) {
    if(LED_MAX_VAL < value) {
        return 1;
    }
    uint16_t regval = (uint16_t)(value*PWM_PERIOD);
    switch (channel) {
        case LED_BLUE:
            PWM1_16BIT_SetSlice1Output1DutyCycleRegister(regval);
            PWM1_16BIT_LoadBufferRegisters();
            break;
        case LED_RED:
            PWM1_16BIT_SetSlice1Output2DutyCycleRegister(regval);
            PWM1_16BIT_LoadBufferRegisters();
            break;
        case LED_GREEN:
            PWM2_16BIT_SetSlice1Output1DutyCycleRegister(regval);
            PWM2_16BIT_LoadBufferRegisters();
            break;
        case LED_ALL:
            PWM1_16BIT_SetSlice1Output1DutyCycleRegister(regval);
            PWM1_16BIT_SetSlice1Output2DutyCycleRegister(regval);
            PWM2_16BIT_SetSlice1Output1DutyCycleRegister(regval);
            PWM1_16BIT_LoadBufferRegisters();
            PWM2_16BIT_LoadBufferRegisters();
            break;
        default:
            return 1;
            break;
    }
    return 0;
}

uint32_t dayTimer = 0;
uint32_t buttonCounter = 0;
float currentDaylight = LED_OFF_VAL;

int main(void)
{
    SYSTEM_Initialize(); 
    
    INTERRUPT_GlobalInterruptEnable(); 
    INTERRUPT_PeripheralInterruptEnable(); 
    
    PWM2_16BIT_Enable();
    PWM2_16BIT_WritePeriodRegister(PWM_PERIOD);
    
    PWM1_16BIT_Enable();
    PWM1_16BIT_WritePeriodRegister(PWM_PERIOD);
    
    Timer0_OverflowCallbackRegister(cbTmrCounter);
    Timer0_Start();
    
    IO_RA5_SetInterruptHandler(cbButtonChange);
    
    setDutyCycle(LED_ALL, LED_OFF_VAL);
    
    while(1)
    {
        if (tmrFlag) {
            tmrFlag = false;
            
            // Button Evaluation
            switch (buttonState) {
                case BUTTON_RELEASED:
                    buttonCounter = 0;
                    if (buttonPress) {
                        buttonState = BUTTON_NEWPRESS;
                    }
                    break;
                case BUTTON_NEWPRESS:
                    if (IO_RA5_GetValue() == BUTTON_IDLE) {
                        buttonState = BUTTON_RELEASED;
                    }
                    else {
                        buttonState = BUTTON_PRESSED;
                    }
                    break;
                case BUTTON_PRESSED:
                    if (IO_RA5_GetValue() == BUTTON_IDLE) {
                        buttonState = BUTTON_RELEASED;
                    }
                    else if (buttonCounter++ >= BUTTON_HOLD_TIME) {
                        buttonState = BUTTON_HOLD;
                    }
                    break;
                case BUTTON_NEWHOLD:
                    if (IO_RA5_GetValue() == BUTTON_IDLE) {
                        buttonState = BUTTON_RELEASED;
                    }
                    else {
                        buttonState = BUTTON_HOLD;
                    }
                    break;
                case BUTTON_HOLD:
                    if (IO_RA5_GetValue() == BUTTON_IDLE) {
                        buttonState = BUTTON_RELEASED;
                    }
                    break;
                default:
                    break;
            }
            buttonPress = false;
            
            // LED State Machine
            switch (State) {
                case SM_STARTUP:
                    currentDaylight = LED_MIN_VAL;
                    setDutyCycle(LED_RED, LED_OFF_VAL);
                    setDutyCycle(LED_GREEN, LED_OFF_VAL);
                    setDutyCycle(LED_BLUE, currentDaylight);
                    if (1) { //buttonState == BUTTON_NEWPRESS) {//(IO_RA5_GetValue() == 0) {
                        State = SM_SUNRISE;
                        setDutyCycle(LED_ALL, currentDaylight);
                    }
                    break;
                case SM_SUNRISE:
                    if ((TIME_SUNRISE < dayTimer) || (buttonState == BUTTON_NEWPRESS)) {
                        while (currentDaylight < LED_DAYLIGHT_VAL) {
                            currentDaylight = currentDaylight + FADE_FAST_STEP;
                            setDutyCycle(LED_ALL, currentDaylight);
                            __delay_ms(1);
                        }
                        State = SM_DAYTIME;
                        dayTimer = 0;
                    }
                    else if (LED_DAYLIGHT_VAL < currentDaylight) {
                        currentDaylight = LED_DAYLIGHT_VAL;
                        setDutyCycle(LED_ALL, currentDaylight);
                    }
                    else {
                        currentDaylight += FADE_SUNRISE_STEP;
                        setDutyCycle(LED_ALL, currentDaylight);
                    }
                    break;
                case SM_DAYTIME:
                    if ((TIME_DAYLIGHT < dayTimer) || (buttonState == BUTTON_NEWPRESS)) {
                        if (buttonState == BUTTON_NEWPRESS) {
                            currentDaylight = LED_DAYLIGHT_VAL - .25;
                            setDutyCycle(LED_ALL, currentDaylight);
                        }
                        State = SM_SUNSET;
                        dayTimer = 0;
                    }
                    break;
                case SM_SUNSET:
                    if ((TIME_SUNSET < dayTimer) || (buttonState == BUTTON_NEWPRESS)) {
                        while (currentDaylight > LED_MIN_VAL) {
                            currentDaylight = currentDaylight - FADE_FAST_STEP;
                            setDutyCycle(LED_ALL, currentDaylight);
                            __delay_ms(1);
                        }
                        State = SM_EVENING;
                        dayTimer = 0;
                        currentDaylight = LED_MIN_VAL;
                        setDutyCycle(LED_RED, LED_OFF_VAL);
                        setDutyCycle(LED_GREEN, currentDaylight);
                        setDutyCycle(LED_BLUE, currentDaylight);
                    }
                    else if (LED_MIN_VAL > currentDaylight) {
                        currentDaylight = LED_MIN_VAL;
                        setDutyCycle(LED_ALL, currentDaylight);
                    }
                    else {
                        currentDaylight -= FADE_SUNSET_STEP;
                        setDutyCycle(LED_ALL, currentDaylight); 
                    }
                    break;
                case SM_EVENING:
                    if ((TIME_EVENING < dayTimer) || (buttonState == BUTTON_NEWPRESS)){
                        while (currentDaylight > 0) {
                            currentDaylight = currentDaylight - FADE_FAST_STEP;
                            setDutyCycle(LED_BLUE, currentDaylight);
                            setDutyCycle(LED_GREEN, currentDaylight);
                            __delay_ms(100);
                        }
                        currentDaylight = LED_OFF_VAL;
                        setDutyCycle(LED_ALL, currentDaylight);
                        State = SM_NIGHT;
                        dayTimer = 0;
                    }
                    break;
                case SM_NIGHT: 
                    if ((TIME_NIGHT < dayTimer) || (buttonState == BUTTON_NEWPRESS)){
                        while (currentDaylight < LED_MIN_VAL) {
                            currentDaylight = currentDaylight + FADE_FAST_STEP;
                            setDutyCycle(LED_ALL, currentDaylight);
                            __delay_ms(100);
                        }
                        State = SM_SUNRISE;
                        dayTimer = 0;
                    }
                    else {
                        setDutyCycle(LED_ALL, LED_OFF_VAL);
                    }
                    break;
                default:
                    State = SM_STARTUP;
                    break;
            };
            dayTimer++;
        }
    }    
}