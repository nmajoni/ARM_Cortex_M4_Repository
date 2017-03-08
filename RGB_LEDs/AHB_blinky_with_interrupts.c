/*
____________________________________________________________________________________________________________________________
The ARM-Cortex M family of processors are some of the most energy efficient and most used in the Embedded Systems world. 
I currently own a Tiva C Series LaunchPad development board with the ARM-Cortex M4 processor. The Tiva™ C Series TM4C123G 
LaunchPad Evaluation Board (EK-TM4C123GXL) is a low-cost evaluation platform for ARM® Cortex™-M4F-based microcontrollers. 
The Tiva C Series LaunchPad design highlights the TM4C123GH6PMI microcontroller USB 2.0 device interface, hibernation module,
and motion control pulse-width modulator (MC PWM) module. The Tiva C Series LaunchPad also features programmable user buttons 
and an RGB LED for custom applications.Below is one of my first codes attempting to blink the Red LED using interrupts. 
The data sheet was a pain to navigate at first. In the end the program ran successfully on the board.
____________________________________________________________________________________________________________________________

Nyasha Godknows Majoni
Embedded Software Developer
Ingolstadt, Germany

*/

#include "TM4C123GH6PM.h"

#define SYS_CLOCK_HZ 16000000UL
#define LED_RED   (1U << 1)
#define LED_BLUE  (1U << 2)
#define LED_GREEN (1U << 3)

int main() {
    SYSCTL->RCGCGPIO  |= (1U << 5); /* enable Run mode for GPIOF */
    SYSCTL->GPIOHBCTL |= (1U << 5); /* enable AHB for GPIOF */
    GPIOF_AHB->DIR |= (LED_RED | LED_BLUE | LED_GREEN);
    GPIOF_AHB->DEN |= (LED_RED | LED_BLUE | LED_GREEN);

    SysTick->LOAD = SYS_CLOCK_HZ/2U - 1U;
    SysTick->VAL  = 0U;
    SysTick->CTRL = (1U << 2) | (1U << 1) | 1U;

    SysTick_Handler();
    
    __enable_interrupt();
    
    while (1) {
        //Do nothing :) or other useful stuff...
    }
    //return 0;
}

void SysTick_Handler(void) { 
    GPIOF_AHB->DATA_Bits[LED_RED] ^= LED_RED;
}


