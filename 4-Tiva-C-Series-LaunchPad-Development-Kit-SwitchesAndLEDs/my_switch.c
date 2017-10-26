// ***** My_Switch ***** 
// The goal of this lab assignment is almost identeical to that of lab #3 which had directions
// to modify the C program in the project my_switch. In the modified system, when SW2
// (connected with PF0) is pressed, the red LED (connected with PF1) is turned off, otherwise the
// red LED is on. We modified the configurations in both the PortFunctionInit() function and the
// main() function.
// The difference in this lab assignment is that the user were required to use the tiva
// pinmux software to setup the i/o for the code.
#include <stdint.h>
#include <stdbool.h>
#include "my_switch_PinMux.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "inc/tm4c123gh6pm.h"

//*****************************************************************************
void
PortFunctionInit(void)
{
//
// Enable Peripheral Clocks
//
MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
//
// Enable pin PF1 for GPIOOutput
//
MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
//
// Enable pin PF0 for GPIOInput
//
//
//First open the lock and select the bits we want to modify in the GPIO commit register.
//
HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;
//
//Now modify the configuration of the pins that we unlocked.
//
MAP_GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
//Enable pullup
on PF0
GPIO_PORTF_PUR_R |= 0x01;
}
int main(void)
{
PortFunctionInit();
while(1)
{
//!PF0 == 0 means SW2 is pressed (since switches are negative logic)
if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)==0x00)
{
//!We wanted the red LED to remain on, so we changed it so that if SW2 (PF0) == 0
//!indicating that it was pressed, the GPIO_PIN_1 (PF1) = 0 indicating that the
//!red LED was off (since leds are positive logic)
(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
}
else
{
//!now we make it so that the GPIO_PIN_1 (PF1) = 1, we do so using pinmux
//!therefore you can indicate that the led remains on when assigned 0x02 (red)
//!unless SW2 is pressed
GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
}
}
}
