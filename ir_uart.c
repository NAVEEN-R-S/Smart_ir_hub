/*
 * ir_uart.c
 *
 *  Created on: Apr 19, 2025
 *      Author: Naveen
 */

#include "stdio.h"
#include "rom.h"
#include "rom_map.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_types.h"
#include "hw_ints.h"
#include "uart.h"
#include "interrupt.h"
#include "utils.h"
#include "prcm.h"
#include "pin.h"

#include "ir_uart.h"

#define APPLICATION_VERSION  "1.4.0"
#define APP_NAME             "UART Echo"
#define CONSOLE              UARTA0_BASE
#define UartGetChar()        MAP_UARTCharGet(CONSOLE)
#define UartPutChar(c)       MAP_UARTCharPut(CONSOLE,c)
#define MAX_STRING_LENGTH    80


static void DisplayBanner(char * AppName)
{

//    Report("\n\n\n\r");
//    Report("\t\t *************************************************\n\r");
//    Report("\t\t        CC3200 %s Application       \n\r", AppName);
//    Report("\t\t *************************************************\n\r");
//    Report("\n\n\n\r");
}

void UARTPinMuxConfig(void)
{
    //
    // Enable Peripheral Clocks
    //
    MAP_PRCMPeripheralClkEnable(PRCM_UARTA0, PRCM_RUN_MODE_CLK);

    //
    // Configure PIN_55 for UART0 UART0_TX
    //
    MAP_PinTypeUART(PIN_55, PIN_MODE_3);

    //
    // Configure PIN_57 for UART0 UART0_RX
    //
    MAP_PinTypeUART(PIN_57, PIN_MODE_3);
}

void Message(const char *str)
{
#ifndef NOTERM
    if(str != NULL)
    {
        while(*str!='\0')
        {
            MAP_UARTCharPut(CONSOLE,*str++);
        }
    }
#endif
}

void InitTerm()
{
#ifndef NOTERM
  MAP_UARTConfigSetExpClk(CONSOLE,MAP_PRCMPeripheralClockGet(CONSOLE_PERIPH),
                  UART_BAUD_RATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                   UART_CONFIG_PAR_NONE));
#endif
  __Errorlog = 0;
}

void ClearTerm()
{
    Message("\33[2J\r");
}

void InitUART(void)
{

    UARTPinMuxConfig();

    InitTerm();

    ClearTerm();
//    DisplayBanner(APP_NAME);
    Message("\t\t****************************************************\n\r");
    Message("\t\t\t        CC3200 UART Echo Usage        \n\r");
    Message("\t\t Type in a string of alphanumeric characters and  \n\r");
    Message("\t\t presenter, the string will be echoed. \n\r") ;
    Message("\t\t Note: if string length reaches 80 character it will \n\r");
    Message("\t\t echo the string without waiting for enter command \n\r");
    Message("\t\t ****************************************************\n\r");
    Message("\n\n\n\r");

}

