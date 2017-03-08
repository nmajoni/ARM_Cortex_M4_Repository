/*
 * Use the buttons sw1 and sw2 on the Tiva C series Launchpad to turn any of the 3
 * LEDs on and off.
 *
 * Author nyasha.majoni
 */

#include "tm4c_cmsis.h"

#define LED_RED   (1U << 1)
#define LED_BLUE  (1U << 2)
#define LED_GREEN (1U << 3)
#define SW_1      (1U << 0)
#define SW_2      (1U << 4)

int main() {

    SYSCTL->RCGC2     = (1U << 5);       //enable Run mode for GPIOF
    SYSCTL->GPIOHSCTL = (1U << 5);       // enable AHB for GPIOF
    GPIOF_HS->LOCK    = 0x4C4F434B;      // unlock value (stated in the data sheet)
    GPIOF_HS->CR      = 0xFF;            //enable ability to write to the PUR (Pull Up Resistor) for PFO
    GPIOF_HS->DIR     = (LED_RED | LED_BLUE | LED_GREEN);                //enable PF1, PF2, PF3 as outputs
    GPIOF_HS->PUR     =  (SW_1|SW_2);                                    //enable pull up resistors (PF0 and PF4)
    GPIOF_HS->DEN     = (LED_RED | LED_BLUE | LED_GREEN | SW_1 | SW_2);  //enable digital function ability


    while (1) {
       switch(GPIOF_HS->DATA & 0x11)
       {
       case 0x00:                         //both switches pressed
          	  GPIOF_HS->DATA = LED_RED;
          break;
       case 0x01:                         //one switch pressed
          	  GPIOF_HS->DATA = LED_BLUE;
		  break;
       case 0x10:                         //the other switch pressed
         	  GPIOF_HS->DATA = LED_GREEN;
          break;
       default:
    	      GPIOF_HS->DATA &= ~(LED_RED | LED_GREEN | LED_BLUE);
    	  break;
       }
   }
}


