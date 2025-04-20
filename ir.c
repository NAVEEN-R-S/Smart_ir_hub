/*
 * ir.c
 *
 *  Created on: Apr 19, 2025
 *      Author: Naveen
 */




#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
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

// === CONFIGURABLE PINS ===
#define IR_RX_GPIO_BASE   GPIOA2_BASE
#define IR_RX_GPIO_PIN    0x40              // GPIO13
#define IR_RX_PIN_NUM     13
#define IR_RX_HW_PIN      PIN_03

#define IR_TX_GPIO_BASE   GPIOA1_BASE
#define IR_TX_GPIO_PIN    0x08              // GPIO11
#define IR_TX_PIN_NUM     11
#define IR_TX_HW_PIN      PIN_45

#define CARRIER_FREQ        38000             // 38kHz
#define PULSE_PERIOD_US     (1000000 / CARRIER_FREQ)  // ~26us
#define PULSE_ON_US         (PULSE_PERIOD_US / 2)     // ~13us
#define PULSE_OFF_US        (PULSE_PERIOD_US / 2)

#define SYS_CLK                 80000000
#define MILLISECONDS_TO_TICKS(ms)   ((SYS_CLK/1000) * (ms))
#define MICROSECONDS_TO_TICKS(ms)   ((SYS_CLK/1000000) * (ms))

#define MAX_EDGES         100


volatile uint32_t irDurations[MAX_EDGES];
volatile int edgeIndex = 0;
volatile uint32_t lastEdgeTime = 0;
volatile bool irCaptured = false;
uint32_t decodedIR = 0;
char protocol[10] = "Unknown";

/*Function prototypes*/
void initIRReceiver(void);
void initIRTransmitter(void);
void transmitIR(void);                         // Send stored IR code based on protocol
void sendPulse(uint32_t duration_us);          // Emit 38kHz modulated signal for given duration
void sendSpace(uint32_t duration_us);          // Delay for space (IR off)
void sendNEC(uint32_t data);                   // Transmit NEC protocol signal
bool analyzeIR(void);                          // Analyze captured IR signal and detect protocol
bool decodeNEC(uint32_t *durations, int count, uint32_t *decodedValue);
bool decodeRC5(uint32_t *durations, int count, uint16_t *decodedValue);
bool decodeSony(uint32_t *durations, int count, uint16_t *decodedValue);
void sendSony(uint32_t data, uint8_t nbits);
void sendRC5(uint32_t data);
void sendNEC(uint32_t data);
uint32_t getTimeUs(void);                      // Returns current time in microseconds
void IRIntHandler(void);                       // GPIO interrupt for IR signal reception

void sendPulse(uint32_t duration_us) {
    uint32_t i = 0;
uint32_t cycles = duration_us / PULSE_PERIOD_US;
for (; i < cycles; i++) {
MAP_GPIOPinWrite(IR_TX_GPIO_BASE, IR_TX_GPIO_PIN, IR_TX_GPIO_PIN);
UtilsDelay(MICROSECONDS_TO_TICKS(PULSE_ON_US));
MAP_GPIOPinWrite(IR_TX_GPIO_BASE, IR_TX_GPIO_PIN, 0);
UtilsDelay(MICROSECONDS_TO_TICKS(PULSE_OFF_US));
}
}

void sendSpace(uint32_t duration_us) {
MAP_GPIOPinWrite(IR_TX_GPIO_BASE, IR_TX_GPIO_PIN, 0);
UtilsDelay(MICROSECONDS_TO_TICKS(duration_us));
}

bool decodeNEC(uint32_t *durations, int count, uint32_t *decodedValue) {
if (count < 66) return false;
// Leader code: 9ms mark + 4.5ms space
if (durations[0] < 8500 || durations[0] > 9500 || durations[1] < 4000 || durations[1] > 5000)
    return false;

uint32_t result = 0;
uint32_t bitIndex = 0;
uint32_t i = 2;

for (; i + 1 < count && bitIndex < 32; i += 2) {
    if (durations[i] > 400 && durations[i] < 700) { // 560us mark
        if (durations[i + 1] > 1600 && durations[i + 1] < 1800) {
            result |= (1UL << bitIndex); // Logical '1'
        } else if (durations[i + 1] > 400 && durations[i + 1] < 700) {
            result |= (0UL << bitIndex); // Logical '0'
        } else {
            return false; // Invalid space duration
        }
        bitIndex++;
    } else {
        return false; // Invalid mark duration
    }
}

if (bitIndex == 32) {
    *decodedValue = result;
    return true;
}

return false;
}

bool decodeRC5(uint32_t *durations, int count, uint16_t *decodedValue) {
if (count < 26) return false;
uint16_t result = 0;
int bitCount = 0;
uint32_t tolerance = 400; // Acceptable pulse variation

// RC5 uses 889us per half-bit
uint32_t i = 0;
for (; i < count - 1 && bitCount < 14; i++) {
    uint32_t delta = durations[i];
    if (delta > (889 - tolerance) && delta < (889 + tolerance)) {
        result <<= 1;
        result |= 1;  // Manchester 1
        bitCount++;
    } else if (delta > (1778 - tolerance) && delta < (1778 + tolerance)) {
        result <<= 1;
        result |= 0;  // Manchester 0
        bitCount++;
    } else {
        return false;  // Out of spec
    }
}

if (bitCount == 14) {
    *decodedValue = result;
    return true;
}

return false;
}

bool decodeSony(uint32_t *durations, int count, uint16_t *decodedValue) {
if (count < 25) return false;
// Check 2.4ms start pulse
if (durations[0] < 2000 || durations[0] > 2600)
    return false;

uint16_t result = 0;
uint32_t bitIndex = 0;
uint32_t i = 1;

for (; i < count && bitIndex < 15; i++) {
    if (durations[i] > 400 && durations[i] < 800) {
        result |= (0 << bitIndex); // Logical 0
    } else if (durations[i] > 1000 && durations[i] < 1400) {
        result |= (1 << bitIndex); // Logical 1
    } else {
        return false;
    }
    bitIndex++;
}

if (bitIndex >= 12) {  // Accept 12 or more bits
    *decodedValue = result;
    return true;
}

return false;

}

uint32_t getTimeUs() {
    return TimerValueGet(TIMERA0_BASE, TIMER_A);
}

void IRIntHandler() {
    uint32_t now = getTimeUs();
    uint32_t delta = now - lastEdgeTime;
    lastEdgeTime = now;
    if (edgeIndex < MAX_EDGES) {
        irDurations[edgeIndex++] = delta;
    }

    // Simple timeout logic to finish after ~70ms (mocked)
    if (delta > 50000 && edgeIndex > 10) {
        irCaptured = true;
    }

    MAP_GPIOIntClear(IR_RX_GPIO_BASE, IR_RX_GPIO_PIN);
}

bool analyzeIR() {
    if (!irCaptured) return false;
    if (decodeNEC((uint32_t*)irDurations, edgeIndex, &decodedIR)) {
        strcpy(protocol, "NEC");
    } else if (decodeRC5((uint32_t*)irDurations, edgeIndex, (uint16_t*)&decodedIR)) {
        strcpy(protocol, "RC5");
    } else if (decodeSony((uint32_t*)irDurations, edgeIndex, (uint16_t*)&decodedIR)) {
        strcpy(protocol, "Sony");
    } else {
        strcpy(protocol, "Unknown");
        return false;
    }

    UART_PRINT("Decoded IR: Protocol=%s, Code=0x%08X\n", protocol, decodedIR);
    return true;
}


void transmitIR() {
if (strcmp(protocol, "NEC") == 0) {
sendNEC(decodedIR);
} else if (strcmp(protocol, "Sony") == 0) {
sendSony((uint32_t)decodedIR, 12);  // Or 15
} else if (strcmp(protocol, "RC5") == 0) {
sendRC5((uint32_t)decodedIR);
} else {
UART_PRINT("Error: Unknown protocol to transmit\n");
}
}

void sendRC5(uint32_t data) {
const uint16_t RC5_TOTAL_BITS = 14;
uint32_t i = RC5_TOTAL_BITS - 1;
// RC5 uses bi-phase encoding (Manchester)
for (; i >= 0; i--) {
    bool bit = (data >> i) & 1;
    if (bit) {
        sendPulse(889);  // 1 = high then low
        sendSpace(889);
    } else {
        sendSpace(889);  // 0 = low then high
        sendPulse(889);
    }
}

MAP_GPIOPinWrite(IR_TX_GPIO_BASE, IR_TX_GPIO_PIN, 0);
}

void sendSony(uint32_t data, uint8_t nbits) {
    uint32_t i = 0;
// Header pulse
sendPulse(2400);
sendSpace(600);
// Send bits LSB first
for (; i < nbits; i++) {
    if (data & (1 << i)) {
        sendPulse(1200);
    } else {
        sendPulse(600);
    }
    sendSpace(600);
}

MAP_GPIOPinWrite(IR_TX_GPIO_BASE, IR_TX_GPIO_PIN, 0);
}

void sendNEC(uint32_t data) {
    uint32_t i = 0;
// Header
sendPulse(9000);
sendSpace(4500);
// Send 32 bits LSB first
for (; i < 32; i++) {
    sendPulse(560);
    if (data & (1UL << i)) {
        sendSpace(1690);
    } else {
        sendSpace(560);
    }
}

// End mark
sendPulse(560);
MAP_GPIOPinWrite(IR_TX_GPIO_BASE, IR_TX_GPIO_PIN, 0);
}

void IRTask(void *pvParameters) {
while (1) {
if (irCaptured) {
MAP_GPIOIntDisable(IR_RX_GPIO_BASE, IR_RX_GPIO_PIN);
bool success = analyzeIR();
if (success) {
transmitIR();
} else {
UART_PRINT("Error: Failed to decode IR signal\n");
}
irCaptured = false;
edgeIndex = 0;
MAP_GPIOIntEnable(IR_RX_GPIO_BASE, IR_RX_GPIO_PIN);
}
//vTaskDelay(pdMS_TO_TICKS(100));  // Delay before checking again
}
}

void initIRReceiver() {
MAP_PRCMPeripheralClkEnable(PRCM_GPIOA2, PRCM_RUN_MODE_CLK);
MAP_PinTypeGPIO(IR_RX_HW_PIN, PIN_MODE_0, false);
MAP_GPIODirModeSet(IR_RX_GPIO_BASE, IR_RX_GPIO_PIN, GPIO_DIR_MODE_IN);
MAP_GPIOIntTypeSet(IR_RX_GPIO_BASE, IR_RX_GPIO_PIN, GPIO_FALLING_EDGE);
MAP_GPIOIntRegister(IR_RX_GPIO_BASE, IRIntHandler);
MAP_GPIOIntClear(IR_RX_GPIO_BASE, IR_RX_GPIO_PIN);
MAP_GPIOIntEnable(IR_RX_GPIO_BASE, IR_RX_GPIO_PIN);
}

void initIRTransmitter() {
MAP_PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);
MAP_PinTypeGPIO(IR_TX_HW_PIN, PIN_MODE_0, false);
MAP_GPIODirModeSet(IR_TX_GPIO_BASE, IR_TX_GPIO_PIN, GPIO_DIR_MODE_OUT);
MAP_GPIOPinWrite(IR_TX_GPIO_BASE, IR_TX_GPIO_PIN, 0);
}

void StartIROperation(void) {

InitTerm();

UART_PRINT("IR Hub Starting...\n");

initIRReceiver();
initIRTransmitter();

osi_TaskCreate(IRTask, "IR Task", 1024, NULL, 1, NULL);
osi_start();

}
