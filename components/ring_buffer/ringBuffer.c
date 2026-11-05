#include "ringBuffer.h"

void ringBuffer_init(ringBuffer_t *buffer)
{
    buffer->head = 0U;
    buffer->tail = 0U;
}

int ringBuffer_push(ringBuffer_t *buffer, uint8_t value)
{
    uint16_t nextHead = (uint16_t)((buffer->head + 1U) % RING_BUFFER_SIZE);

    if (nextHead == buffer->tail) {
        return 0;
    }

    buffer->data[buffer->head] = value;
    buffer->head = nextHead;
    return 1;
}

int ringBuffer_pop(ringBuffer_t *buffer, uint8_t *value)
{
    if (buffer->head == buffer->tail) {
        return 0;
    }

    *value = buffer->data[buffer->tail];
    buffer->tail = (uint16_t)((buffer->tail + 1U) % RING_BUFFER_SIZE);
    return 1;
}
