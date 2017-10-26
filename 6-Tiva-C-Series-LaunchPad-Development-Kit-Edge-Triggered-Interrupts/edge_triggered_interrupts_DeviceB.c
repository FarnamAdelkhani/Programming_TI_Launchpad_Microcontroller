// Device B
#include <stdint.h>
#include <stdbool.h>
#include "C:\ti\TivaWare_C_Series-2.1.2.111\examples\boards\ek-tm4c123gxl\switch_counter_interrupt_TivaWare\switch_counter_interrupt_TivaWare.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"

//*****************************************************************************
#define blue_mask 0x04
#define read_pin 0x80

void
PortFunctionInit(void)
{
    //
    // Enable Peripheral Clocks 
    //
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //
    // Enable pin PA7 for GPIOInput
    //
GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, read_pin);

    //
    // Enable pin PF2 for GPIOOutput
    //
GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, blue_mask);
GPIO_PORTA_PUR_R |=0X11;
}


void
Interrupt_Init(void)
{
IntEnable(INT_GPIOA);  							// enable interrupt 30 in NVIC (GPIOA)
	IntPrioritySet(INT_GPIOA, 0x00); 		// configure GPIOF interrupt priority as 0
	GPIO_PORTA_IM_R |= read_pin;   		// arm interrupt on PF0 and PF4
	GPIO_PORTA_IS_R &= ~read_pin;     // PF0 and PF4 are edge-sensitive
  GPIO_PORTA_IBE_R |= read_pin;   	// PF0 and PF4 not both edges trigger 
  GPIO_PORTA_IEV_R &= ~0x80;  	// PF0 and PF4 falling edge event
	IntMasterEnable();       		// globally enable interrupt
}

//interrupt handler
void GPIOPortA_Handler(void)
{
	
	//SW1 is pressed
	if(GPIO_PORTA_RIS_R&read_pin)
	{
		// acknowledge flag for PF4
		GPIOIntClear(GPIO_PORTA_BASE, read_pin);
			if((GPIO_PORTA_DATA_R&read_pin)==read_pin)
				GPIO_PORTF_DATA_R |= blue_mask;         //Turn on LED.
			else
				GPIO_PORTF_DATA_R &= ~blue_mask;        //Turn off LED.
		}
	}

int main(void)
{
	
		//initialize the GPIO ports	
		PortFunctionInit();
		
	//configure the GPIOF interrupt
		Interrupt_Init();
	
    //
    // Loop forever.
    //
while(1)
    {

    }
}
