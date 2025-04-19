/*
 * ir_uart.h
 *
 *  Created on: Apr 19, 2025
 *      Author: Naveen
 */

#ifndef IR_UART_H_
#define IR_UART_H_


#define UART_BAUD_RATE  115200
#define SYSCLK          80000000
#define CONSOLE         UARTA0_BASE
#define CONSOLE_PERIPH  PRCM_UARTA0
//
// Define the size of UART IF buffer for RX
//
#define UART_IF_BUFFER           64

//
// Define the UART IF buffer
//
extern unsigned char g_ucUARTBuffer[];

static unsigned long __Errorlog;

void Message(const char *str);
void InitUART(void);


#endif /* IR_UART_H_ */
