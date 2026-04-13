#include "ring_buffer.h"

static inline uint8_t next_index(const ring_buffer_t *rb, uint8_t index)
{
    return (uint8_t)((index + 1U) % rb->size);
}

void ring_buffer_init(ring_buffer_t *rb, uint8_t *storage, uint8_t size, critical_section_ops_t cs)
{
    rb->buffer = storage;
    rb->head = 0;
    rb->tail = 0;
    rb->size = size;
    rb->cs = cs;
}

bool ring_buffer_is_empty(const ring_buffer_t *rb)
{
    return rb->head == rb->tail;
}

bool ring_buffer_is_full(const ring_buffer_t *rb)
{
    return next_index(rb, rb->head) == rb->tail;
}

bool ring_buffer_push(ring_buffer_t *rb, uint8_t value)
{
    uint8_t irq_state = rb->cs.enter ? rb->cs.enter(rb->cs.ctx) : 0;

    if (ring_buffer_is_full(rb)) {
        if (rb->cs.exit) {
            rb->cs.exit(rb->cs.ctx, irq_state);
        }
        return false;
    }

    rb->buffer[rb->head] = value;
    rb->head = next_index(rb, rb->head);

    if (rb->cs.exit) {
        rb->cs.exit(rb->cs.ctx, irq_state);
    }
    return true;
}

bool ring_buffer_pop(ring_buffer_t *rb, uint8_t *value)
{
    uint8_t irq_state = rb->cs.enter ? rb->cs.enter(rb->cs.ctx) : 0;

    if (ring_buffer_is_empty(rb)) {
        if (rb->cs.exit) {
            rb->cs.exit(rb->cs.ctx, irq_state);
        }
        return false;
    }

    *value = rb->buffer[rb->tail];
    rb->tail = next_index(rb, rb->tail);

    if (rb->cs.exit) {
        rb->cs.exit(rb->cs.ctx, irq_state);
    }
    return true;
}
