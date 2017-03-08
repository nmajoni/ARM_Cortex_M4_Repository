/*
 * Using the periodic timer mode using interrupts. Test using one of the LEDs on the TIVA Launchpad
 * main.c
 * Author: Nyasha Godknows Majoni
 */

#include  "tm4c_cmsis.h"

#define LED_BLUE  (1u << 2)

void Timer0A_IRQHandler(void){
	TIMER0->ICR |= (1u << 0);
	GPIOF->DATA_Bits[LED_BLUE] ^= LED_BLUE; //toggle
}

int main(void) {
	
   /*configuration for the test LED*/
    SYSCTL->RCGC2 |= (1u << 5); //enable clock on PORTF
    GPIOF->AFSEL &= ~LED_BLUE;  //disable alternate function for PF2 (connected blue LED)
    GPIOF->DIR    |= LED_BLUE;  //set pin as output
    GPIOF->DEN    |= LED_BLUE;  //enable digital functionality
    GPIOF->DATA_Bits[LED_BLUE] = LED_BLUE;  //turn it on

    /*so we want to use a GPTM (General Purpose Timer Mode) lets set that up and enable a clock to it*/
    SYSCTL->RCGC1 |= (1u << 16);

    //step 1: Ensure the timer is disabled (TnEN bit in the GPTMCTL register) before making any changes
    TIMER0->CTL &= ~(1u << 0);
    //step 2: Write the GPTM Configuration Register (GPTMCFG) with a value of 0x0000.0000
    TIMER0->CFG = 0x00000000;
    //step 3: Configure the TnMR field in the GPTM Timer n Mode Register (GPTMTnMR): Write a value of 0x2 for Periodic mode.
    TIMER0->TAMR |= (0x2 << 0);
    //step 4: Optionally configure the TnSNAPS, TnWOT, TnMTE, and TnCDIR bits in the GPTMTnMR register to select whether to capture the value of
    //the free-running timer at time-out, use an external trigger to start counting, configure an additional trigger or interrupt, and count up or down.
    TIMER0->TAMR &= ~(1u << 4);
    //step 5: Load the start value into the GPTM Timer n Interval Load Register (GPTMTnILR). max_val = 4 294 967 295
    TIMER0->TAILR = 0x00F42400; //16 000 000, i have got a clock of 16MHz so that would be a second
    //step 6: If interrupts are required, set the appropriate bits in the GPTM Interrupt Mask Register (GPTMIMR).
    TIMER0->IMR |= (1u << 0);

    NVIC_EnableIRQ(Timer0A_IRQn); //Enable the interrupt request
    //step 7: Set the TnEN bit in the GPTMCTL register to enable the timer and start counting.
    TIMER0->CTL |= (1u << 0);

    while(1){
     //Do other useful stuff
    }

	return 0;
}
