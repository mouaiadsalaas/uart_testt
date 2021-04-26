
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "utils/uartstdio.h"
#include "utils/uartstdio.c"
unsigned char str[];
uint32_t cnt;
void UARTIntHandler(void)
{
    uint32_t ui32Status;
    ui32Status = UARTIntStatus(UART0_BASE, true); // Get the interrrupt status.
    UARTIntClear(UART0_BASE, ui32Status);  // Clear the asserted interrupts.

    while (UARTCharsAvail(UART0_BASE)) // Loop while there are characters in the receive FIFO.
    {
        str[cnt++] = UARTCharGetNonBlocking(UART0_BASE);
        if (cnt == 300)
        {
            cnt = 0;
        }

        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
        SysCtlDelay(SysCtlClockGet() / (1000 * 3)); // Delay for 1 millisecond.  Each SysCtlDelay is about 3 clocks.
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, !GPIO_PIN_2);

    }
}

void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    while (ui32Count--)    // Loop while there are more characters to send.
    {
        UARTCharPut(UART0_BASE, *pui8Buffer++); // Write the next character to the UART.
    }
}
/*
void Bufclear()
{
    memset(str, 0, sizeof(str));
}*/
int main(void)
{
    //Clock Config.........................
    SysCtlClockSet(
            SYSCTL_SYSDIV_5 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN
                    | SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
//UARt0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    IntMasterEnable();
    GPIOPinConfigure(GPIO_PA0_U0RX); // Set GPIO A0 and A1 as UART pins.
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 250000,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                            UART_CONFIG_PAR_NONE));
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT | UART_INT_TX);
    UARTSend((uint8_t *) "\033[2JEnter text: ", 16);
while(1)
{
    UARTSend((uint8_t *)str  , 8);
    SysCtlDelay(SysCtlClockGet() / (3));
}
}
