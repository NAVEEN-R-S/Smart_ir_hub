/*
 * ir.c
 *
 *  Created on: Apr 19, 2025
 *      Author: Naveen
 */




#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_ints.h"
#include "hw_gpio.h"
#include "gpio.h"
#include "prcm.h"
#include "interrupt.h"
#include "timer.h"
#include "pin.h"
#include "common.h"
#include "rom_map.h"

#include "ir_uart.h"

#define IR_GPIO_PIN GPIO_PIN_6  // GPIO pin connected to the IR receiver
#define IR_GPIO_PORT GPIOA0_BASE
#define TIMER_BASE TIMERA0_BASE

#define MAX_SIGNAL_LENGTH 10  // Maximum number of signal edges to store
#define TOLERANCE 200          // Tolerance for signal timing in microseconds

volatile uint32_t irSignal[MAX_SIGNAL_LENGTH];
volatile uint32_t signalIndex = 0;
volatile bool signalCaptured = false;

// Example function to decode NEC protocol
bool DecodeNEC(uint32_t *durations, uint32_t length) {
    // NEC protocol decoding logic here
    // Compare durations with NEC timing specifications
    return false;  // Return true if successfully decoded
}

// Example function to decode Sony protocol
bool DecodeSony(uint32_t *durations, uint32_t length) {
    // Sony protocol decoding logic here
    return false;  // Return true if successfully decoded
}

// Example function to decode RC5 protocol
bool DecodeRC5(uint32_t *durations, uint32_t length) {
    // RC5 protocol decoding logic here
    return false;  // Return true if successfully decoded
}

// Interrupt handler for GPIO pin
void IR_GPIO_InterruptHandler(void) {
    uint32_t currentTime;
    unsigned long ulstatus;

    ulstatus = MAP_GPIOIntStatus(IR_GPIO_PORT, true);

    if(ulstatus & IR_GPIO_PIN)
    {
        UART_PRINT("A IR signal received");
        return;
    }

    // Clear the interrupt
    MAP_GPIOIntClear(IR_GPIO_PORT, IR_GPIO_PIN);

    // Read the current timer value
    currentTime = MAP_TimerValueGet(TIMER_BASE, TIMER_A);

    // Store the time difference in the array
    if (signalIndex < MAX_SIGNAL_LENGTH) {
        irSignal[signalIndex++] = currentTime;
    } else {
        signalCaptured = true;  // Signal capture complete
    }
}

// Function to initialize the GPIO pin for the IR receiver
void IR_GPIO_Init(void) {
    // Enable the peripheral clock for GPIO
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);

    // Configure the GPIO pin as input
    MAP_PinTypeGPIO(PIN_61, PIN_MODE_0, false);
    MAP_GPIODirModeSet(IR_GPIO_PORT, IR_GPIO_PIN, GPIO_DIR_MODE_IN);

    // Configure the GPIO interrupt
    MAP_GPIOIntTypeSet(IR_GPIO_PORT, IR_GPIO_PIN, GPIO_FALLING_EDGE);
    MAP_GPIOIntRegister(IR_GPIO_PORT, IR_GPIO_InterruptHandler);
    MAP_GPIOIntEnable(IR_GPIO_PORT, IR_GPIO_PIN);
}

// Function to initialize the timer for capturing IR signal timing
void Timer_Init(void) {
    // Enable the peripheral clock for the timer
    MAP_PRCMPeripheralClkEnable(PRCM_TIMERA0, PRCM_RUN_MODE_CLK);

    // Configure the timer
    MAP_TimerConfigure(TIMER_BASE, TIMER_CFG_PERIODIC_UP);
    MAP_TimerLoadSet(TIMER_BASE, TIMER_A, 0xFFFFFFFF);

    // Start the timer
    MAP_TimerEnable(TIMER_BASE, TIMER_A);
}

// Function to analyze the captured signal and decode the IR code
void AnalyzeSignal(void) {
    uint32_t i = 1;
    if (signalIndex < 2) {
        UART_PRINT("No valid signal captured.\n");
        return;
    }

    uint32_t durations[MAX_SIGNAL_LENGTH - 1];
    for (; i < signalIndex; i++) {
        durations[i - 1] = irSignal[i] - irSignal[i - 1];
    }

    // Example: Decode NEC protocol
    if (DecodeNEC(durations, signalIndex - 1)) {
        UART_PRINT("NEC protocol decoded successfully.\n");
    } else if (DecodeSony(durations, signalIndex - 1)) {
        UART_PRINT("Sony protocol decoded successfully.\n");
    } else if (DecodeRC5(durations, signalIndex - 1)) {
        UART_PRINT("RC5 protocol decoded successfully.\n");
    } else {
        UART_PRINT("Unknown protocol.\n");
    }

    // Reset signal index for the next capture
    signalIndex = 0;
    signalCaptured = false;
}

void StartIROperation(void)
{

    // Initialize the GPIO and timer
    IR_GPIO_Init();
    Timer_Init();

    UART_PRINT("IR signal capture started...\n");

    // Main loop
    while (1) {
        if (signalCaptured) {
            AnalyzeSignal();
        }
    }

}
