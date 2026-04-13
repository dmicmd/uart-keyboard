#include "ring_buffer.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int enter_calls;
    int exit_calls;
} mock_irq_t;

static uint8_t mock_enter(void *ctx)
{
    mock_irq_t *m = (mock_irq_t *)ctx;
    m->enter_calls++;
    return 0xAA;
}

static void mock_exit(void *ctx, uint8_t state)
{
    mock_irq_t *m = (mock_irq_t *)ctx;
    assert(state == 0xAA);
    m->exit_calls++;
}

int main(void)
{
    uint8_t storage[4];
    mock_irq_t irq = {0};
    ring_buffer_t rb;

    ring_buffer_init(&rb,
                     storage,
                     4,
                     (critical_section_ops_t){.enter = mock_enter, .exit = mock_exit, .ctx = &irq});

    assert(ring_buffer_is_empty(&rb));
    assert(ring_buffer_push(&rb, 1));
    assert(ring_buffer_push(&rb, 2));
    assert(ring_buffer_push(&rb, 3));
    assert(!ring_buffer_push(&rb, 4));

    uint8_t out = 0;
    assert(ring_buffer_peek(&rb, &out) && out == 1);
    assert(ring_buffer_pop(&rb, &out) && out == 1);
    assert(ring_buffer_pop(&rb, &out) && out == 2);
    assert(ring_buffer_pop(&rb, &out) && out == 3);
    assert(!ring_buffer_pop(&rb, &out));

    assert(irq.enter_calls == irq.exit_calls);
    return 0;
}
