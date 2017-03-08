/*Using PORTA A[2:7] to light up LEDs one after the other (bit shifting really, through multiplication) and
 * also simultaneously. The point was to show the awesomeness of GPIOPinWrite();
 *
 * Author: Nyasha Majoni
 *         Embedded Software Developer
 * main.c
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

int main(void)
{
	uint8_t bit_pos = 2;
        uint8_t state = 0x00u;

	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

	while(1)
	{
		if(state == 0x00)
		{

			GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, bit_pos);
			// Delay for a bit
			SysCtlDelay(1000000);
			// Cycle through array of LEDs
			if (bit_pos == 128) {
				bit_pos = 2;
				state = 0x01u;
			}
			else {
				bit_pos = bit_pos*2;
			}

		}

		if(state == 0x01u){
			GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0xFCu);
			SysCtlDelay(2000000);
			state = 0x00u;
		}
	}
}
