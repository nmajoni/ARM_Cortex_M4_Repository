



#include "tm4c_cmsis.h"
#include "stdbool.h"
//#include "TM4C123GH6PM.h"


#define TRANSMIT (I2C1_MASTER->MSA &= ~(1U << 0))
#define RECEIVE  (I2C1_MASTER->MSA |= (1U << 0))

enum {
    WRITE,
    READ
};

static uint8_t LEDs = 0x00;

void I2C1_setSlaveAddress(uint8_t slaveAddress);
void I2C1_setRW(bool mode);
void I2C1_TransmitByte(uint8_t data, uint8_t conditions);
void I2C1_init(void);


/*
 * main.c
 */
int main(void) {

   I2C1_init();
   I2C1_setSlaveAddress(0x20);
   I2C1_setRW(WRITE);
   I2C1_TransmitByte(0x00, (1U << 0)|(1U << 1));     //the value of the I/0 expander data direction register address is 0x00, conditions = START and RUN
   I2C1_TransmitByte(0x00, (1U << 0)|(1U << 2));     //the value to be written to that register is 0x00 meaning make pins outputs, conditions = STOP, RUN

    while(1){

        I2C1_TransmitByte(0x09, (1U << 0)|(1U << 1));
        I2C1_TransmitByte(LEDs++, (1U << 0)|(1U << 2));

        for(int i = 0; i < 1000000; i++){}
    }
	return 0;

}

void I2C1_init(void){

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
     TPR = (16MHz/(2*(6+4)*100000))-1;      ->note: system clock is 16MHz in my case
     TPR = 7
     Write the I2CMTPR register with the value of 0x0000.0007.
    */
    I2C1_MASTER->MTPR = (7U << 0);

}


/**
 * @brief   sets the slave address
 * @param[in]  slaveAddress    slave address
 */
void I2C1_setSlaveAddress(uint8_t slaveAddress){

    //8. Specify the slave address of the master
    I2C1_MASTER->MSA = (slaveAddress << 1);
}


/**
 * @brief   sets the mode ie. Read or Write data
 * @param[in]  mode   O for write and 1 for read
 */
void I2C1_setRW(bool mode){

    mode ? RECEIVE : TRANSMIT;
}


/**
 * @brief   function to transmit a byte of data
 * @param[in]  data          data byte to be transmitted
 * @param[in]  conditions    STOP, START, RUN
 */
void I2C1_TransmitByte(uint8_t data, uint8_t conditions){

    // 9. Place data (byte) to be transmitted in the data register by writing the I2CMDR register with the desired data.
    I2C1_MASTER->MDR = data;

    // 10. Initiate a single byte transmit of the data from Master to Slave by writing the I2CMCS register with a value of 0x0000.0007 (STOP, START, RUN).
    I2C1_MASTER->MCS = conditions;

    // 11. Wait until the transmission completes by polling the I2CMCS register's BUSBSY bit until it has been cleared.
    while((I2C1_MASTER->MCS & (1U << 0)) != 0);

    // 12. Check the ERROR bit in the I2CMCS register to confirm the transmit was acknowledged.
    if((I2C1_MASTER->MCS & (1U << 1)) != 0){

        if(I2C1_MASTER->MCS & (1U << 4)){
            //TODO: print log - controller lost arbitration
        }else {
            I2C1_MASTER->MCS = (1U << 2); //send STOP condition
            while((I2C1_MASTER->MCS & (1U << 0)) != 0);
        }
    }
}

