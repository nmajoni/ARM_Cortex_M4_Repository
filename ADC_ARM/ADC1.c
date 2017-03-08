/*
 * Using the ADC of the ARM_Cortex_M4
 * main.c
 * Author: Nyasha Godknows Majoni
 * Embedded Software Developer
 * Ingolstadt, Bayern
 * Germany
 *
 */

#include <stdint.h>
#include "TM4C123GH6PM.h"

volatile static uint32_t adc_val = 0;

void ADC1Seq3_IRQHandler(void){
	 adc_val = ADC1->SSFIFO3;
	 ADC1->ISC = (1u << 3); 
}

#define LED_GREEN (1u << 3)
#define ANALOG_IN (1u << 1)

int main(void) {

	/*From the Datasheet:
	  Initialization of the ADC module is a simple process with very few steps: enabling the clock to the ADC,
	  disabling the analog isolation circuit associated with all inputs that are to be used, and reconfiguring
	  the sample sequencer priorities (if needed). The initialization sequence for the ADC is as follows:*/

    //step 1. Enable the ADC clock using the RCGCADC register (see page 352).
	SYSCTL->RCGCADC |= (1u << 1);
	//step 2. Enable the clock to the appropriate GPIO modules via the RCGCGPIO register (see page 340). To find out which GPIO ports to enable, refer to “Signal Description” on page 801.
	SYSCTL->RCGCGPIO |= (1u << 4)|(1u << 5); //Port E and F

	/*configuration for the test LEDs*/
	GPIOF->AFSEL  &= ~LED_GREEN; //disable alternate function we wanna use digital
	GPIOF->DIR    = 0xFF; //enable as output
	GPIOF->DEN    = 0xFF; //enable digital functionality
    /************************************/

	//step 3. Set the GPIO AFSEL bits for the ADC input pins (see page 671). To determine which GPIOs to configure, see Table 23-4 on page 1344.
	GPIOE->AFSEL  = ANALOG_IN;
	//step 4. Configure the AINx signals to be analog inputs by clearing the corresponding DEN bit in the GPIO Digital Enable (GPIODEN) register (see page 682).
	GPIOE->DEN &= ~ANALOG_IN;
	GPIOE->DIR &= ~ANALOG_IN; //make it input
	//step 5. Disable the analog isolation circuit for all ADC input pins that are to be used by writing a 1 to the appropriate bits of the GPIOAMSEL register (see page 687) in the associated GPIO block.
	GPIOE->AMSEL = ANALOG_IN;
	//step 6. If required by the application, reconfigure the sample sequencer priorities in the ADCSSPRI register. The default configuration has Sample Sequencer 0 with the highest priority and Sample
    //        Sequencer 3 as the lowest priority.
    /*not required for now*/

    /*Sample Sequencer Configuration*/

	//step 1. Ensure that the sample sequencer is disabled by clearing the corresponding ASENn bit in the ADCACTSS register. Programming of the sample sequencers is allowed without having them
	//enabled. Disabling the sequencer during programming prevents erroneous execution if a trigger event were to occur during the configuration process.
	ADC1->ACTSS &= ~(1u << 3);  //SS3 off
	//step 2. Configure the trigger event for the sample sequencer in the ADCEMUX register.
	ADC1->EMUX = (0xF << 12); //trigger source continuous sample
	//step 3. When using a PWM generator as the trigger source, use the ADC Trigger Source Select (ADCTSSEL) register to specify in which PWM module the generator is located. The default
	//register reset selects PWM module 0 for all generators.
	/*not required for now*/
	//step 4. For each sample in the sample sequence, configure the corresponding input source in the ADCSSMUXn register.
	ADC1->SSMUX3 = 2; //AIN2
	//step 5. For each sample in the sample sequence, configure the sample control bits in the corresponding nibble in the ADCSSCTLn register. When programming the last nibble, ensure that the END bit
	//is set. Failure to set the END bit causes unpredictable behavior.
	ADC1->SSCTL3 = 0x6; //END0 (end of conversion) and IE0 (interrupt enable)
	//step 6. If interrupts are to be used, set the corresponding MASK bit in the ADCIM register.
	ADC1->IM = (1u << 3);
	//step 7. Enable the sample seq  on
    ADC1->ISC = (1u << 3); //Interrupt Status and Clear. This bit is cleared by writing a 1. Clearing this bit also clears the INR3 (Raw Interrupt Status) bit in the ADCRIS register which says conversion complete

    NVIC_EnableIRQ( ADC1SS3_IRQn); //Enable interrupt request for sequence sampler 3

	while(1){
	
		if(adc_val > 2048)
		{
			GPIOF->DATA_Bits[LED_GREEN] = LED_GREEN; //turn LED on
		}
		else
		{
			GPIOF->DATA_Bits[LED_GREEN] = 0x0; //turn LED off
		}

	}
	return 0;
}





