/* UART0 Microcontroller - PC communication
 * main.c
 * Author: Nyasha Majoni
 */

#include <stdint.h>
#include "TM4C123GH6PM.h"

#define SYSCLOCK        16000000UL
#define BAUD            9600
#define BaudRateDivisor (SYSCLOCK/16/BAUD)
#define LED_RED         (1u << 1)
#define LED_GREEN	(1u << 2)
#define LED_BLUE	(1u << 3)

char UART0_receive(void);
void UART0_transmit(char c);
void printString(char *string);

int main(void) {
	
    char c;

    /*--------------To enable and initialize the UART, the following steps are necessary:------------------*/

    //SYSCTL->RCGC1 |= (1u << 0); //1. Enable the UART0 module using the RCGCUART register or RCGC1
    *((unsigned int *)0x400FE104U) |= (1u << 0); //SYSCTL not working for some reason
    //SYSCTL->RCGC0 |= (1u << 0); //2. Enable the clock to the appropriate GPIO module via the RCGCGPIO register. Port A chosen
    *((unsigned int *)0x400FE608U) |= (1u << 0); //SYSCTL not working for some reason
    GPIOA->AFSEL  |= (1u << 1)|(1u << 0); //3. Set the GPIO AFSEL bits for the appropriate pins
    //TODO: 4. Configure the GPIO current level and/or slew rate as specified for the mode selected.
    GPIOA->PCTL = (1u << 0)|(1u << 4); //5. Configure the PMCn fields in the GPIOPCTL register to assign the UART signals to the appropriate pins
    GPIOA->DEN  = (1u << 0)|(1u << 1); //Enable digital functionality

   /*
    * The first thing to consider when programming the UART is the baud-rate divisor (BRD), because the UARTIBRD and UARTFBRD registers
    * must be written before the UARTLCRH register. Using the equation described in “Baud-Rate Generation” on page 896, the BRD can be
    * calculated: BRD = SYSCLOCK / (16 * BAUD) which means that the DIVINT field of the UARTIBRD register (see page 914)
    * should be set to 10 decimal or 0xA. The value to be loaded into the UARTFBRD register (see page 915) is calculated by the equation:
    * UARTFBRD[DIVFRAC] = integer(decimal_part_of_BRD * 64 + 0.5) With the BRD values in hand, the UART configuration is written to the module in
    * the following order:
    */

    UART0->CTL  &= ~(1u << 0);      //1. Disable the UART by clearing the UARTEN bit in the UARTCTL register.
    UART0->IBRD  = BaudRateDivisor; //2. Write the integer portion of the BRD to the UARTIBRD register.
    UART0->FBRD  = 11;              //3. Write the fractional portion of the BRD to the UARTFBRD register. (manually calculated, (0.1666667 * 64 + 0.5) TODO: Implement with FPU)
    UART0->LCRH  |= (0x3u << 5);    //4. Write the desired serial parameters to the UARTLCRH register. In this case, 8-bit, no parity, 1-stop bit
    UART0->CC    = 0x0;             //5. Configure the UART clock source by writing to the UARTCC register.
    //TODO: 6. Optionally, configure the microDMA channel (see “Micro Direct Memory Access (microDMA)” on page 585) and enable the DMA option(s) in the UARTDMACTL register.
    UART0->CTL |= (1u << 0)|(1u << 8)|(1u << 9);       //7. Enable the UART by setting the UARTEN bit in the UARTCTL register.

    /*--------------------Test using on-board LEDs---------------------------*/
    //SYSCTL->RCGCGPIO |= (1u << 5);  //Enable clock for PORTF
    *((unsigned int *)0x400FE608U) = 0x20U;            //SYSCTL->RCGCGPIO not working for now
    GPIOF->DIR    |=  (LED_RED|LED_GREEN|LED_BLUE);    //Set as outputs
    GPIOF->DEN    |=  (LED_RED|LED_GREEN|LED_BLUE);    //Enable digital functionality
    GPIOF->DATA   &= ~(LED_RED|LED_GREEN|LED_BLUE);    //All LEDs off

    while(1)
    {
	  printString("Enter \"r\", \"g\", or \"b\":\n\r");
	  c = UART0_receive();         //wait for keyboard input
	  UART0_transmit(c);           //transmit entered character so you see
	  printString("\n\r");         //newline

	  switch(c)
	  {
	  case 'r':
		  GPIOF->DATA = LED_RED;    //red led on
		  break;
	  case 'b':
		  GPIOF->DATA = LED_GREEN;  //green led on
		  break;
	  case 'g':
		  GPIOF->DATA = LED_BLUE;   //blue led on
		  break;
	  default:
		  GPIOF->DATA &= ~(LED_RED|LED_GREEN|LED_BLUE); //all leds off
	 }
    }
}

char UART0_receive(void)
{
	char c;
	while((UART0->FR & (1u << 4)) != 0);   //(Flag Register) wait until receiver is not empty.
	c = UART0->DR;                         //Get data/char from the data register
	return c;
}

void UART0_transmit(char c)
{
	while((UART0->FR & (1u << 5)) != 0);   //check if previous transmission has been completed in the Flag Register
	UART0->DR = c;                         //load the data into the data register
}

void printString(char *string)
{
	while(*string)                         //till pointer reaches NULL
	{
		UART0_transmit(*(string++));
	}
}
