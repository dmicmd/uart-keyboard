#include "rx_decoder.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t calls;
    uint8_t last_led;
    bool last_on;
} fixture_t;

static void set_led(void *ctx, uint8_t led_index, bool on)
{
    fixture_t *f = (fixture_t *)ctx;
    f->calls++;
    f->last_led = led_index;
    f->last_on = on;
}

int main(void)
{
    fixture_t f = {0};
    rx_decoder_io_t io = {.set_led = set_led, .ctx = &f};

    rx_decoder_handle_byte(io, (uint8_t)(0x80U | 0x01U));
    assert(f.calls == 1 && f.last_led == 0 && f.last_on);

    rx_decoder_handle_byte(io, (uint8_t)(0x00U | 0x0CU));
    assert(f.calls == 2 && f.last_led == 11 && !f.last_on);

    rx_decoder_handle_byte(io, 0x00U);
    assert(f.calls == 2);

    return 0;
}
