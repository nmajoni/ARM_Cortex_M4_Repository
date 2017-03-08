/*
____________________________________________________________________________________________________________________________
The ARM-Cortex M family of processors are some of the most energy efficient and most used in the Embedded Systems world. 
I currently own a Tiva C Series LaunchPad development board with the ARM-Cortex M4 processor. The Tiva™ C Series TM4C123G 
LaunchPad Evaluation Board (EK-TM4C123GXL) is a low-cost evaluation platform for ARM® Cortex™-M4F-based microcontrollers. 
The Tiva C Series LaunchPad design highlights the TM4C123GH6PMI microcontroller USB 2.0 device interface, hibernation module,
and motion control pulse-width modulator (MC PWM) module. The Tiva C Series LaunchPad also features programmable user buttons 
and an RGB LED for custom applications.Below is one of my first codes attempting to blink the Red, Green and Blue LEDs one after
the other. The data sheet was a pain to navigate at first. In the end the program ran successfully on the board.
____________________________________________________________________________________________________________________________

Nyasha Godknows Majoni
Embedded Software Developer
Bremen, Germany

*/

#include <stdio.h>
#include <stdlib.h>
#include "lm4f120h5qr.h"

#define LED_RED    (1U << 1)
#define LED_BLUE   (1U << 2)
#define LED_GREEN  (1U << 3)

#define STOP while(hold < 1000000){ hold++;}

int main(){
	 
        SYSCTL_RCGCGPIO_R  |= (1U << 5); //setting bit 5 in the register to enable the clock (off because of clock gating)
        GPIO_PORTF_DIR_R   |= (LED_RED | LED_BLUE | LED_GREEN); //GPIO F pin direction register setting bits 1,2 and 3 as outputs
        GPIO_PORTF_DEN_R   |= (LED_RED | LED_BLUE | LED_GREEN); //Enabling bits 1, 2 and 3 in the digital enable register 
        
        
        while(1){
          
            GPIO_PORTF_DATA_R |= LED_RED; //Turn RED LED on
            
            int volatile hold = 0;
            
            STOP;
            
            GPIO_PORTF_DATA_R &= ~LED_RED; //Turn RED LED off
            
            hold = 0;
            
            GPIO_PORTF_DATA_R |= LED_BLUE; //Turn BLUE LED on
            
            STOP;
            
            GPIO_PORTF_DATA_R &= ~LED_BLUE; //Turn BLUE LED off
            
            hold = 0;
            
            GPIO_PORTF_DATA_R |= LED_GREEN; //Turn GREEN LED on
            
            STOP;
            
            GPIO_PORTF_DATA_R &= ~LED_GREEN; //Turn GREEN LED off
        }
        
}
