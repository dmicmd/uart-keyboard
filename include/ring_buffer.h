#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "critical_section.h"

typedef struct {
    uint8_t *buffer;
    uint8_t head;
    uint8_t tail;
    uint8_t size;
    critical_section_ops_t cs;
} ring_buffer_t;

void ring_buffer_init(ring_buffer_t *rb, uint8_t *storage, uint8_t size, critical_section_ops_t cs);
bool ring_buffer_is_empty(const ring_buffer_t *rb);
bool ring_buffer_is_full(const ring_buffer_t *rb);
bool ring_buffer_push(ring_buffer_t *rb, uint8_t value);
bool ring_buffer_peek(ring_buffer_t *rb, uint8_t *value);
bool ring_buffer_pop(ring_buffer_t *rb, uint8_t *value);

#endif
