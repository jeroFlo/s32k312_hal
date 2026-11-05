#ifndef UART_H
#define UART_H

/**
 * @file UART.h
 * @brief S32K312 LPUART6 interface.
 *
 * This adapter configures LPUART6 on the S32K312 and connects received bytes
 * to the shared UART6 receive ring buffer.
 */

#include "helper.h"
#include <stdint.h>
#include "../components/ring_buffer/ringBuffer.h"

extern ringBuffer_t rxBuffer;

void UART6_Init(uint32_t);
void uprintc(char);
void uprint(const char *);
char ugetc(void);
void UART6_RX_IRQCallback(void (*)(void));
#endif /* UART_H */