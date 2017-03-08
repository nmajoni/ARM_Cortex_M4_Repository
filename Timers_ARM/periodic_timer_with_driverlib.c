/*Timers and Interrupts using driverlib and other cool libraries. This calls the ISR twice a second.
 * The ISR just toggles the blue LED on the Tiva Launchpad.
 *
 * Author: Nyasha Majoni
 *         Embedded Software Developer
 *         Germany
 * main.c
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"


int main(void) {
	
	uint32_t period;

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); //System clock configuration, run at 40MHz

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);                                      //Enable the GPIO peripheral
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2| GPIO_PIN_3);        //Enable pins as output

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);                                     //Enable the TIMER0 peripheral
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);                                  //Enable the periodic timer mode

    period = (SysCtlClockGet()/1)/2;                  //Getting clock cycles for the desired frequency (1Hz) and dividing by 2 for duty cycle of 50%
    TimerLoadSet(TIMER0_BASE, TIMER_A, period-1);     //Load the calculated value

    IntEnable(INT_TIMER0A);                           //Enable the specific vector associated with Timer0A
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);  //Enable a specific event within the timer to generate an interrupt. Here we have TIMEOUT
    IntMasterEnable();                                //Enable global interrupts

    TimerEnable(TIMER0_BASE, TIMER_A);                //Start the timer

    while(1)
    {
       //do other useful stuff
    }

	return 0;
}

void Timer0IntHandler(void)                                                   //The handler function has been added in the startup code
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);                           // Clear the timer interrupt
	GPIO_PORTF_DATA_BITS_R[(1u << 2)] ^= (1u << 2);                           //toggle the LED
}
