#include <peripheral_test.h>
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

extern void (* const g_pfnVectors[])(void);

int main(void) {
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();

//    Test_LED();
//    TEST_Timer();
    TEST_UART();

    while(1);

    return 0;
}
