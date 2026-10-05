#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>

#define RING_BUFFER_SIZE 128U

typedef struct {
    uint8_t data[RING_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
} ringBuffer_t;

void ringBuffer_init(ringBuffer_t *buffer);
int ringBuffer_push(ringBuffer_t *buffer, uint8_t value);
int ringBuffer_pop(ringBuffer_t *buffer, uint8_t *value);

#endif /* RING_BUFFER_H */
