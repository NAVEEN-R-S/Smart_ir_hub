/*
 * peripheral_test.c
 *
 *  Created on: Apr 19, 2025
 *      Author: Naveen
 */


#include <stdio.h>
#include <stdint.h>
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_ints.h"
#include "pin.h"
#include "gpio.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "utils.h"
#include "timer.h"
#include "interrupt.h"
#include "simplelink.h"
#include "common.h"
#include "uart.h"

#include "osi.h"

#include "ir_uart.h"

/*RED LED*/
#define R_LED_GPIO_PIN 9

/*ORANGE LED*/
#define O_LED_GPIO_PIN 10

/*GREEN LED*/
#define G_LED_GPIO_PIN 11

#define LED_PORT GPIOA1_BASE

#define SYS_CLK                 80000000
#define MILLISECONDS_TO_TICKS(ms)   ((SYS_CLK/1000) * (ms))



unsigned long g_ulTimerInts;







uint8_t led_state = 0;

void delay_ms(uint32_t ms) {
    UtilsDelay((80 * ms * 1000) / 3);
}

void TurnONLED(uint8_t Pin, uint8_t state)
{
    unsigned char GPIORegVal;
    GPIORegVal = state << (Pin%8);
    Pin = 1 << (Pin%8);
    MAP_GPIOPinWrite(LED_PORT, Pin, GPIORegVal);
}

void PinMuxConfig(void)
{
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);

    MAP_PinTypeGPIO(PIN_64, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA1_BASE, 0x2, GPIO_DIR_MODE_OUT);

    MAP_PinTypeGPIO(PIN_01, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA1_BASE, 0x4, GPIO_DIR_MODE_OUT);

    MAP_PinTypeGPIO(PIN_02, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA1_BASE, 0x8, GPIO_DIR_MODE_OUT);
}


void Test_LED(void) {
    uint8_t ledshowcount = 5;

    // Initialize the PRCM for GPIO
    PinMuxConfig();

    while(ledshowcount--)
    {
        TurnONLED(R_LED_GPIO_PIN,0);
        TurnONLED(O_LED_GPIO_PIN,0);
        TurnONLED(G_LED_GPIO_PIN,0);
        delay_ms(1000);

        TurnONLED(R_LED_GPIO_PIN,1);
        delay_ms(1000);
        TurnONLED(O_LED_GPIO_PIN,1);
        delay_ms(1000);
        TurnONLED(G_LED_GPIO_PIN,1);
        delay_ms(1000);

    }

}

void Timer_IF_InterruptClear(unsigned long ulBase)
{
    unsigned long ulInts;
    ulInts = MAP_TimerIntStatus(ulBase, true);

    MAP_TimerIntClear(ulBase, ulInts);
}

void Test_TimerAIntHandler(void)
{
    led_state = (led_state==0)?1:0;
    g_ulTimerInts ++;
    TurnONLED(R_LED_GPIO_PIN,led_state);
    Timer_IF_InterruptClear(TIMERA0_BASE);

}

void TEST_Timer(void)
{
    PinMuxConfig();

    MAP_PRCMPeripheralClkEnable(PRCM_TIMERA0, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralReset(PRCM_TIMERA0);
//    Configure Timer A
    TimerConfigure(TIMERA0_BASE,TIMER_CFG_PERIODIC);
    TimerPrescaleSet( TIMERA0_BASE, TIMER_A, 0);

//    Enable interrupt and set interrupt handler
    MAP_IntPrioritySet(INT_TIMERA0A, INT_PRIORITY_LVL_1);
    TimerIntRegister(TIMERA0_BASE, TIMER_A, Test_TimerAIntHandler);
    TimerIntEnable(TIMERA0_BASE, TIMER_TIMA_TIMEOUT);


//    Enable Timer
    TimerLoadSet( TIMERA0_BASE,  TIMER_A,  MILLISECONDS_TO_TICKS(500));
    TimerEnable(TIMERA0_BASE, TIMER_A);

    while(1);

}

void TEST_UART(void)
{
    InitUART();

    Message("UART test : Hello from CC32200 Launch pad!");
}


