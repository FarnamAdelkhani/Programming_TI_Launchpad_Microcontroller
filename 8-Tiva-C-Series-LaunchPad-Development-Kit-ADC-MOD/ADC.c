#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h"


//*****************************************************************************
//!
//! In this project we use ADC0, SS3 to measure the data from the on-chip 
//! temperature sensor. Then we enabled analog input pin PE3 and connected
//!it to another project to see the fluctuation of high and low frequencies.
//!
//*****************************************************************************

uint32_t ui32ADC0Value[1];
//volatile uint32_t ui32TempAvg;
volatile uint32_t ui32TempValueC;
volatile uint32_t ui32TempValueF;

//ADC0 initializaiton
void ADC0_Init(void)
{
    
	        SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); // configure the system clock to be 40MHz
SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);    //activate the clock of ADC0
SysCtlDelay(2);	//insert a few cycles after enabling the peripheral to allow the clock to be   fully activated.

	    ADCSequenceDisable(ADC0_BASE, 3); //disable ADC0 before the configuration is complete
	    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0); // will use ADC0, SS3, processor-trigger, priority 0
	    //ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS); //ADC0 SS3 Step 0, sample from internal temperature sensor
	    //ADCSequenceStepConfigure(ADC0_BASE, 3, 1, ADC_CTL_TS); //ADC0 SS3 Step 1, sample from internal temperature sensor
	    //ADCSequenceStepConfigure(ADC0_BASE, 3, 2, ADC_CTL_TS); //ADC0 SS3 Step 2, sample from internal temperature sensor
		//ADC0 SS3 Step 0, sample from internal temperature sensor, completion of this step will set RIS, last sample of the sequence
	    ADCSequenceStepConfigure(ADC0_BASE,3,0,ADC_CTL_CH0|ADC_CTL_IE|ADC_CTL_END); 
    //change ADC_CTL_TS to ADC_CTL_CH0 which means the analog input signal
   // will be from Ain0
	    IntPrioritySet(INT_ADC0SS3, 0x00);  	// configure ADC0 SS3 interrupt priority as 0
	    IntEnable(INT_ADC0SS3);		    	// enable interrupt 31 in NVIC (ADC0 SS3)
	    ADCIntEnableEx(ADC0_BASE, ADC_INT_SS3);  	// arm interrupt of ADC0 SS3
    
	    ADCSequenceEnable(ADC0_BASE, 3); //enable ADC0
}
	    
//interrupt handler
void ADC0_Handler(void)
{
    
	    ADCIntClear(ADC0_BASE, 3);
	    ADCProcessorTrigger(ADC0_BASE, 3);
	    ADCSequenceDataGet(ADC0_BASE, 3, ui32ADC0Value);
//ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
		ui32TempValueC =  ui32ADC0Value[0];
		ui32TempValueF = (ui32TempValueC);
}

int main(void)
{
		ADC0_Init();
	    IntMasterEnable();   	    	// globally enable interrupt
	    ADCProcessorTrigger(ADC0_BASE, 3);
    
		while(1)
		{
	        
		}
}
