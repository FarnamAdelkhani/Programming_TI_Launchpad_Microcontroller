// ***** my_toggle *****
#include <stdint.h>
#include <stdbool.h>
#include "my_toggle_PinMux.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
//!need to define that GREEN_MASK is represented by 0x08 and vice versa
#define GREEN_MASK 0x08
//*****************************************************************************
void
PortFunctionInit(void)
{
//
// Enable Peripheral Clocks
//
MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
//
// Enable pin PF3 for GPIOOutput
//
MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
}
int main(void)
{
uint8_t LED_data;
//initialize the GPIO ports
PortFunctionInit();
// Turn on the LED.
//!need to change the LED_data from 0x02 to 0x08 to get the green led
LED_data= 0x08;
//!We need to make sure the the pin changes from PF1 to PF3
GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, LED_data);
//
// Loop forever.
//
while(1)
{
// Delay for a bit.
SysCtlDelay(2000000);
// Toggle the LED.
//!we need to make sure that the mask is changed from red to green
LED_data^=GREEN_MASK; //!toggle green LED (PF3)
GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, LED_data);
}
}
