//#include <peripheral_test.h>
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

#include "peripheral_test.h"
#include "ir_wifi.h"
#include "ir.h"
#include "wlan.h"

extern void (* const g_pfnVectors[])(void);

static void BoardInit(void)
{
// In case of TI-RTOS vector table is initialize by OS itself
#ifndef USE_TIRTOS
    //
    // Set vector table base
    //
#if defined(ccs) || defined(gcc)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif //USE_TIRTOS

    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

void IR_PinMuxConfig(void)
{
    //
        // Enable Peripheral Clocks
        //
        MAP_PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);
        MAP_PRCMPeripheralClkEnable(PRCM_UARTA0, PRCM_RUN_MODE_CLK);

        //
        // Configure PIN_55 for UART0 UART0_TX
        //
        MAP_PinTypeUART(PIN_55, PIN_MODE_3);

        //
        // Configure PIN_57 for UART0 UART0_RX
        //
        MAP_PinTypeUART(PIN_57, PIN_MODE_3);

        //
        // Configure PIN_64 for GPIOOutput
        //
        MAP_PinTypeGPIO(PIN_64, PIN_MODE_0, false);
        MAP_GPIODirModeSet(GPIOA1_BASE, 0x2, GPIO_DIR_MODE_OUT);

        //
        // Configure PIN_01 for GPIOOutput
        //
        MAP_PinTypeGPIO(PIN_01, PIN_MODE_0, false);
        MAP_GPIODirModeSet(GPIOA1_BASE, 0x4, GPIO_DIR_MODE_OUT);

        //
        // Configure PIN_02 for GPIOOutput
        //
        MAP_PinTypeGPIO(PIN_02, PIN_MODE_0, false);
        MAP_GPIODirModeSet(GPIOA1_BASE, 0x8, GPIO_DIR_MODE_OUT);
}

int main(void) {
    BoardInit();

    PRCMCC3200MCUInit();

    IR_PinMuxConfig();

//    Test_LED();
//    TEST_Timer();
//    TEST_UART();
//    TEST_WifiConnecitvity();
//    TEST_WifiConnectivity();
    StartIROperation();

    while(1);

    return 0;
}
