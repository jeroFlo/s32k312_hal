#ifndef UART_H
#define UART_H

#include <stdint.h>

#define UART6_RX_BUFFER_SIZE 128U

typedef struct {
	uint8_t data[UART6_RX_BUFFER_SIZE];
	volatile uint16_t head;
	volatile uint16_t tail;
} ringBuffer_t;

extern ringBuffer_t rxBuffer;

void UART6_Init(uint32_t);
void uprintc(char);
void uprint(const char *);
char ugetc(void);
void UART6_RX_IRQCallback(void (*)(void));
void ringBuffer_init(ringBuffer_t *);
int ringBuffer_push(ringBuffer_t *, uint8_t);
int ringBuffer_pop(ringBuffer_t *, uint8_t *);
#endif /* UART_H */