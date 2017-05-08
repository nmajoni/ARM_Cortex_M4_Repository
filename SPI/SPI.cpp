




#include "TM4C123GH6PM.h"

#define SSI2CLK  (1U << 4)   //PB4
#define SSI2Fss  (1U << 5)   //PB5
#define SSI2Rx   (1U << 6)   //PB6
#define SSI2Tx   (1U << 7)   //PB7

uint8_t leds = 0x00;

void SSI2_init(void);
void spi_send(uint8_t data);

/*
 * main.c
 */
int main(void) {

	SSI2_init();               //spi config init

	GPIOB->DATA |= (1U << 5);
	GPIOB->DATA &= ~(1U << 5); //set the Chip Select line low for transmission

	spi_send(0x40);            //slave address
	spi_send(0x00);            //IODIR address, access to it to...
	spi_send(0x00);            //...set pins as outputs

	for(int i = 0; i < 60; i++);   //because the last bit may actually not be sent before line is set high again
	GPIOB->DATA |=  (1U << 5);     //set the Chip Select line high, end transmission

	while(1){


		GPIOB->DATA &= ~(1U << 5); //set the CS line low for transmission

		spi_send(0x40);            //talk to the slave again (slave address)
		spi_send(0x09);            //now we are accessing the GPIO register. Our pins are already set as outputs
		spi_send(leds++);          //for the pins. This will act as a binary counter

		for(int i = 0; i < 60; i++); //because the last bit may actually not be sent before line is set high again
		GPIOB->DATA |=  (1U << 5);   //set the CS line high, end transmission

		for(int i = 0; i < 500000; i++); //observe leds
	}
}


void SSI2_init(void){

	//Initialization and Configuration
	/**
	 * To enable and initialize the SSI, the following steps from the datasheet are necessary:
	 */

	/*step 1. Enable the SSI module using the RCGCSSI register (see page 346).*/
	*((uint32_t *)0x400FE61C) = (1U << 2);  //SSI module 2

	/*step 2. Enable the clock to the appropriate GPIO module via the RCGCGPIO register (see page 340). To find out which GPIO port to enable, refer to Table 23-5 on page 1351.*/
    SYSCTL->RCGC2 = (1U << 1);              //GPIOB

	/*step 3. Set the GPIO AFSEL bits for the appropriate pins (see page 671). To determine which GPIOs to configure, see Table 23-4 on page 1344.*/
    GPIOB->AFSEL |= SSI2CLK | SSI2Rx | SSI2Tx;
    GPIOB->AFSEL &= ~SSI2Fss;   //for controlling the CS (chip select) line

	/*step 4. Configure the PMCn fields in the GPIOPCTL register to assign the SSI signals to the appropriate pins. See page 688 and Table 23-5 on page 1351.*/
    GPIOB->PCTL   = (2U << 28) | (2U << 24) | (2U << 20) | (2U << 16);  //PMC[4:7]

	/*step 5. Program the GPIODEN register to enable the pin's digital function. In addition, the drive strength,
		drain select and pull-up/pull-down functions must be configured. Refer to “General-Purpose Input/Outputs (GPIOs)” on page 649 for more information.
		Note: Pull-ups can be used to avoid unnecessary toggles on the SSI pins, which can take the slave to a wrong state. In addition, if the SSIClk signal is programmed to steady state
		High through the SPO bit in the SSICR0 register, then software must also configure the GPIO port pin corresponding to the SSInClk signal as a pull-up in the GPIO Pull-Up Select (GPIOPUR) register.
	*/
    GPIOB->DEN |= SSI2CLK | SSI2Fss | SSI2Rx | SSI2Tx;
    GPIOB->DIR |= SSI2Fss;   //SS/CS set high

    /*
     * For each of the frame formats, the SSI is configured using the following steps:
     */
    /*step 1. Ensure that the SSE bit in the SSICR1 register is clear before making any configuration changes*/
    SSI2->CR1 &= ~(1U << 1);

    /*step 2. Select whether the SSI is a master or slave:
		a. For master operations, set the SSICR1 register to 0x0000.0000.
		b. For slave mode (output enabled), set the SSICR1 register to 0x0000.0004.
		c. For slave mode (output disabled), set the SSICR1 register to 0x0000.000C.
	*/
    SSI2->CR1 = 0x00;

    /*step 3. Configure the SSI clock source by writing to the SSICC register.*/
    SSI2->CC = 0x00;  //use system clock

    /*step 4. Configure the clock prescale divisor by writing the SSICPSR register.*/
    SSI2->CPSR = 10;  //this means SSI2 clock will be (16 000 000 / 10) = 1 600 000; 1.6Mhz

    /*step 5. Write the SSICR0 register with the following configuration:
		Serial clock rate (SCR)
		Desired clock phase/polarity, if using Freescale SPI mode (SPH and SPO)
		The protocol mode: Freescale SPI, TI SSF, MICROWIRE (FRF)
		The data size (DSS)
    */
    SSI2->CR0 = (0x7U << 0);   //data size = 8 bit data. Used reset values for the rest of the config


    /*step 6.  Optionally, configure the SSI module for uDMA use with the following steps:
		a. Configure a uDMA for SSI use. See “Micro Direct Memory Access (uDMA)” on page 585 for
		more information.
		b. Enable the SSI Module's TX FIFO or RX FIFO by setting the TXDMAE or RXDMAE bit in the SSIDMACTL register.
    */
     //TODO:

    /*step 7. Enable the SSI by setting the SSE bit in the SSICR1 register.*/
    SSI2->CR1 |= (1U << 1);   //re-enable the SSI module

} //End of SSI_init function


void spi_send(uint8_t data){

	SSI2->DR = data;
	 while((SSI2->SR & (1<<0)) == 0); //wait for transmit FIFO to be empty. SR will have a "1"
}
