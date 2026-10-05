#ifndef UART_H
#define UART_H
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