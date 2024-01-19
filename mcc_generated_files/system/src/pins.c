/**
 * Generated Driver File
 * 
 * @file pins.c
 * 
 * @ingroup  pinsdriver
 * 
 * @brief This is generated driver implementation for pins. 
 *        This file provides implementations for pin APIs for all pins selected in the GUI.
 *
 * @version Driver Version 3.0.0
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

#include "../pins.h"

void (*IO_RA5_InterruptHandler)(void);

void PIN_MANAGER_Initialize(void)
{
   /**
    LATx registers
    */
    LATA = 0x7;

    /**
    TRISx registers
    */
    TRISA = 0x20;

    /**
    ANSELx registers
    */
    ANSELA = 0x7;

    /**
    WPUx registers
    */
    WPUA = 0x20;
  
    /**
    ODx registers
    */
   
    ODCONA = 0x0;
    /**
    SLRCONx registers
    */
    SLRCONA = 0x3F;
    /**
    INLVLx registers
    */
    INLVLA = 0x3F;

    /**
    PPS registers
    */
    RA0PPS = 0x0B;  //RA0->PWM1_16BIT:PWM11;
    RA1PPS = 0x0C;  //RA1->PWM1_16BIT:PWM12;
    RA2PPS = 0x0D;  //RA2->PWM2_16BIT:PWM21;

    /**
    APFCON registers
    */

   /**
    IOCx registers 
    */
    IOCAP = 0x0;
    IOCAN = 0x20;
    IOCAF = 0x0;

    IO_RA5_SetInterruptHandler(IO_RA5_DefaultInterruptHandler);

    // Enable PIE0bits.IOCIE interrupt 
    PIE0bits.IOCIE = 1; 
}
  
void PIN_MANAGER_IOC(void)
{
    // interrupt on change for pin IO_RA5}
    if(IOCAFbits.IOCAF5 == 1)
    {
        IO_RA5_ISR();  
    }
}
   
/**
   IO_RA5 Interrupt Service Routine
*/
void IO_RA5_ISR(void) {

    // Add custom IOCAF5 code

    // Call the interrupt handler for the callback registered at runtime
    if(IO_RA5_InterruptHandler)
    {
        IO_RA5_InterruptHandler();
    }
    IOCAFbits.IOCAF5 = 0;
}

/**
  Allows selecting an interrupt handler for IOCAF5 at application runtime
*/
void IO_RA5_SetInterruptHandler(void (* InterruptHandler)(void)){
    IO_RA5_InterruptHandler = InterruptHandler;
}

/**
  Default interrupt handler for IOCAF5
*/
void IO_RA5_DefaultInterruptHandler(void){
    // add your IO_RA5 interrupt custom code
    // or set custom function using IO_RA5_SetInterruptHandler()
}
/**
 End of File
*/