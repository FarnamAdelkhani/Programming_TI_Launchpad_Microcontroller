#include <stdint.h>
#include <stdbool.h>
#include "C:\ti\TivaWare_C_Series-2.1.2.111\examples\boards\my_projects\switch_delay_interrupt_TivaWare\switch_delay_interrupt_PinMux.h"
#include "lab7.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h" //manually added by the programmer
volatile unsigned long count=0;
//#define BLUE_MASK 0x04
//*****************************************************************************
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
// Enable pin PF0 for GPIOInput
//
//
//First open the lock and select the bits we want to modify in the GPIO commit
register.
//
HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;
//
//Now modify the configuration of the pins that we unlocked.
//
GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
//
// Enable pin PF4 for GPIOInput
//
GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
//
// Enable pin PF1 for GPIOOutput
//
GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
//Enable pullup on PF4 and PF0
GPIO_PORTF_PUR_R |= 0x11;
}
void
Interrupt_Init(void)
{
IntEnable(INT_GPIOF); // enable interrupt 30 in NVIC(GPIOF)
IntPrioritySet(INT_GPIOF, 0x04); // configure GPIOF interrupt priority as 2
GPIO_PORTF_IM_R |= 0x11; // arm interrupt on PF0
GPIO_PORTF_IS_R &= ~0x11; // PF0 is edgesensitive
GPIO_PORTF_IBE_R &= ~0x11; // PF0 both edges trigger
GPIO_PORTF_IEV_R &= ~0x11; // PF0 falling edge event
IntMasterEnable(); // globally enable interrupt
}
//interrupt handler
void GPIOPortF_Handler(void){
// Delay for a bit.
SysCtlDelay(2000000);
GPIO_PORTF_DATA_R ^= 0x02;
//SW1 is pressed
if(GPIO_PORTF_RIS_R&0x10)
{
//Acknowledge flag for PF4
GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
if (count==3)
{
count=0;
}
else
{
count++;
}
if (count==1)
{
GPIO_PORTF_DATA_R |= 0x02;
GPIO_PORTF_DATA_R &=~ 0x04;
}
else if (count==2)
{
GPIO_PORTF_DATA_R &=~ 0x02;
GPIO_PORTF_DATA_R |= 0x04;
}
else if (count==3)
{
GPIO_PORTF_DATA_R |= 0x04;
GPIO_PORTF_DATA_R |= 0x02;
}
else
{
GPIO_PORTF_DATA_R &=~ 0x04;
GPIO_PORTF_DATA_R &=~ 0x02;
}
}
//SW2 is pressed
if(GPIO_PORTF_RIS_R&0x01)
{
//Acknowledge flag for PF0
GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);
if(count==0)
{
count=3;
}
else
{
count--;
}
if (count==1)
{
GPIO_PORTF_DATA_R |= 0x02;
GPIO_PORTF_DATA_R &=~ 0x04;
}
else if (count==2)
{
GPIO_PORTF_DATA_R &=~ 0x02;
GPIO_PORTF_DATA_R |= 0x04;
}
else if (count==3)
{
GPIO_PORTF_DATA_R |= 0x04;
GPIO_PORTF_DATA_R |= 0x02;
}
else
{
GPIO_PORTF_DATA_R &=~ 0x04;
GPIO_PORTF_DATA_R &=~ 0x02;
}
}
}
//Globally enable interrupts
void IntGlobalEnable(void)
{
__asm(" cpsie i\n");
}
void Timer0A_Init(unsigned long period)
{
volatile uint32_t ui32Loop;
SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER0; // activate timer0
ui32Loop = SYSCTL_RCGC1_R; // Do a dummy read to insert a few cycles
after enabling the peripheral.
TIMER0_CTL_R &= ~0x00000001; // disable timer0A during setup
TIMER0_CFG_R = 0x00000000; // configure for 32-bit timer mode
TIMER0_TAMR_R = 0x00000002; // configure for periodic mode, default downcount
settings
TIMER0_TAILR_R = period-1; // reload value
NVIC_PRI4_R &= ~0xE0000000; // configure Timer0A interrupt priority as 0
NVIC_EN0_R |= 0x00080000; // enable interrupt 19 in NVIC (Timer0A)
TIMER0_IMR_R |= 0x00000001; // arm timeout interrupt
TIMER0_CTL_R |= 0x00000001; // enable timer0A
}
//interrupt handler for Timer0A
void Timer0A_Handler(void)
{
// acknowledge flag for Timer0A
TIMER0_ICR_R |= 0x00000001;
// // Toggle the blue LED.
// GPIO_PORTF_DATA_R ^=BLUE_MASK;
if (count==3)
{
count = 0;
}
else
{
count++;
}
if (count==1)
{
GPIO_PORTF_DATA_R |= 0x02;
GPIO_PORTF_DATA_R &=~ 0x04;
}
else if (count==2)
{
GPIO_PORTF_DATA_R &=~ 0x02;
GPIO_PORTF_DATA_R |= 0x04;
}
else if (count==3)
{
GPIO_PORTF_DATA_R |= 0x04;
GPIO_PORTF_DATA_R |= 0x02;
}
else
{
GPIO_PORTF_DATA_R &=~ 0x04;
GPIO_PORTF_DATA_R &=~ 0x02;
}
}
int main(void)
{
unsigned long period = 16000000; //reload value to Timer0A to generate one
second delay
//initialize the GPIO ports
PortFunctionInit();
// // Turn on the LED.
// GPIO_PORTF_DATA_R |= 0x04;
//initialize Timer0A and configure the interrupt
Timer0A_Init(period);
//configure the GPIOF interrupt
Interrupt_Init();
IntGlobalEnable(); // globally enable interrupt
//
// Loop forever.
//
while(1)
{
}
}
