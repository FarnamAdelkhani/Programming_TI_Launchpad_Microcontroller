// **** toggle_timer_interrupt_TivaWare ****

#include <stdint.h>
#include <stdbool.h>
#include "toggle_timer_interrupt_TivaWare.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h"


#define 	BLUE_MASK 		0x04

//*****************************************************************************
//
//!
//! A very simple example that uses a general purpose timer generated periodic 
//! interrupt to toggle the on-board LED.
//
//*****************************************************************************
volatile unsigned short count = 0;

void
PortFunctionInit(void)
{
    //
    // Enable Peripheral Clocks 
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //
    // Enable pin PF2 for GPIOOutput
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
	
		//
		// Enable pin PF1 for GPIOOutput
		//
		GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	
		//
		// Enable pin PF0 for GPIOInput
		//
		
		//
		// First open the lock and select the bits we want to modify in the GPIO commit register.
		//
		HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
		HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;
		
		//
		// Now modify the configuration of the pins that we unlocked.
		//
		GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
		
		//
		// Enable pin PF4 for GPIOInput
		//
		GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
		
		GPIO_PORTF_PUR_R |= 0x11;
}

//Globally enable interrupts 
void Interrupt_Init(void)
{
		IntEnable(INT_GPIOF);											// enable interrupt 30 in NVIC (GPIOF)
		IntPrioritySet(INT_GPIOF, 0x01);					// configure GPIOF interrupt priority as 1
		GPIO_PORTF_IM_R |= 0x11;									// arm interrupt on PF0 and PF4
		GPIO_PORTF_IS_R &= ~0x00;									// PF0 and PF4 are edge-sensitive
		GPIO_PORTF_IBE_R &= ~0x00;								// PF0 and PF4 not both edges trigger
		GPIO_PORTF_IEV_R &= ~0x00;								// PF0 and PF4 falling edge event
		IntMasterEnable();												// globally enable interrupt
    
}

void Timer0A_Init(unsigned long period)
{   
	//
  // Enable Peripheral Clocks 
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); 		// configure for 32-bit timer mode
  TimerLoadSet(TIMER0_BASE, TIMER_A, period -1);      //reload value
	IntPrioritySet(INT_TIMER0A, 0x00);  	 // configure Timer0A interrupt priority as 0
  IntEnable(INT_TIMER0A);    				// enable interrupt 19 in NVIC (Timer0A)
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);      // arm timeout interrupt
  TimerEnable(TIMER0_BASE, TIMER_A);      // enable timer0A
}

void GPIOPortF_Handler(void){
	//switch debouncing
	NVIC_EN0_R &= 0x40000000;
	SysCtlDelay(733333);
	NVIC_EN0_R |=0x40000000;
	
	//SW1 is pressed
	if(GPIO_PORTF_RIS_R&0x10)
	{
		// acknowledge flag for PF4
		GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4); 
		//counter imcremented by 1
		count++;
		//Color configurations
		if(count==1){ //01
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
		}
		else if (count==2){//10
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x04);
		}
		else if (count==3){//11
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x04);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
		}
		else{//00
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
			count=0;
		}
	}
	
	//SW2 is pressed
  if(GPIO_PORTF_RIS_R&0x01)
	{
		// acknowledge flag for PF0
		GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);
		//counter decremented by 1
		if(count==0)
			count=3;
		else
			count--;
		
		if (count==0){//00
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
		}
		else if (count==3){//11
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x04);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
		}
		else if (count==2){//10
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x04);
		}
		else if(count==1){//01
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
		}
		else{
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
		}
		
	}
}

//interrupt handler for Timer0A
void Timer0A_Handler(void)
{
		// acknowledge flag for Timer0A timeout
		TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	
		// Toggle the blue LED.
   // GPIO_PORTF_DATA_R ^=BLUE_MASK;
		count++;
		//Color configurations
		if(count==1){ //01
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
		}
		else if (count==2){//10
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x04);
		}
		else if (count==3){//11
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x04);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
		}
		else{//00
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
			count=0;
		}

}

int main(void)
{	
		unsigned long period = 16000000; //reload value to Timer0A to generate half second delay
	
		//initialize the GPIO ports	
		PortFunctionInit();
	
		Interrupt_Init();
    // Turn on the LED.
    GPIO_PORTF_DATA_R |= 0x04;

    //initialize Timer0A and configure the interrupt
		Timer0A_Init(period);
	
		IntMasterEnable();        		// globally enable interrupt
	
    //
    // Loop forever.
    //
    while(1)
    {
				
    }
}
