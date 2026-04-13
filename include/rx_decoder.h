#ifndef RX_DECODER_H
#define RX_DECODER_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*rx_led_set_fn)(void *ctx, uint8_t led_index, bool on);

typedef struct {
    rx_led_set_fn set_led;
    void *ctx;
} rx_decoder_io_t;

void rx_decoder_handle_byte(rx_decoder_io_t io, uint8_t data);

#endif
