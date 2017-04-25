
#include "tm4c_cmsis.h"
//#include "TM4C123GH6PM.h"









/*
 * main.c
 */
int main(void) {
	
	/***Steps to configure the I2C module to transmit a single byte as a master from the data sheet***/

	//1. Enable the I2C clock using the RCGCI2C register in the System Control module (see page 348).
	SYSCTL->RCGC1 = (1U << 14);  //I2C Module 1

	//2. Enable the clock to the appropriate GPIO module via the RCGCGPIO register in the System Control module (see page 340).
    SYSCTL->RCGC2 = (1U << 0);  //enable clock for port A

	//3. In the GPIO module, enable the appropriate pins for their alternate function using the GPIOAFSEL register (see page 671). See Table 23-4 on page 1344
    GPIOA->AFSEL = (1U << 6) | (1U << 7); //PA6 (I2C1SCL) and PA7 (I2CSDA)

    GPIOA->DEN   = (1U << 6) | (1U << 7); //enable digital functionality

	//4. Enable the I2CSDA pin for open-drain operation. See page 676.
    GPIOA->ODR  = (1U << 7);  //that's PA7

	//5. Configure the PMCn fields in the GPIOPCTL register to assign the I2C signals to the appropriate pins. See page 688 and Table 23-5 on page 1351.
    GPIOA->PCTL = (3U << 28) | (3U << 24);   //PMC7 and PMC6 both have the value 3 to choose I2C

	//6. Initialize the I2C Master by writing the I2CMCR register with a value of 0x0000.0010.
    I2C1_MASTER->MCR = (1U << 4);   //Master mode enabled

	//7. Set the desired SCL clock speed of 100 Kbps by writing the I2CMTPR register with the correct value.
	/*
	 The value written to the I2CMTPR register represents the number of system clock periods
	 in one SCL clock period. The TPR value is determined by the following equation:
	 TPR = (System Clock/(2*(SCL_LP + SCL_HP)*SCL_CLK))-1;
	 TPR = (20MHz/(2*(6+4)*100000))-1;
	 TPR = 9
	 Write the I2CMTPR register with the value of 0x0000.0009.
	*/
	//8. Specify the slave address of the master and that the next operation is a Transmit by writing the I2CMSA register with a value of 0x0000.0076.
    // This sets the slave address to 0x3B.

	// 9. Place data (byte) to be transmitted in the data register by writing the I2CMDR register with the desired data.

	// 10. Initiate a single byte transmit of the data from Master to Slave by writing the I2CMCS register with a value of 0x0000.0007 (STOP, START, RUN).

	// 11. Wait until the transmission completes by polling the I2CMCS register's BUSBSY bit until it has been cleared.

	// 12. Check the ERROR bit in the I2CMCS register to confirm the transmit was acknowledged.

	return 0;
}

