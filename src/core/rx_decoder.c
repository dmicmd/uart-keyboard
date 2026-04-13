#include "rx_decoder.h"

void rx_decoder_handle_byte(rx_decoder_io_t io, uint8_t data)
{
    uint8_t led = data & 0x1FU;
    uint8_t state = (data >> 6) & 0x03U;

    if (led < 1U || led > 12U) {
        return;
    }

    io.set_led(io.ctx, (uint8_t)(led - 1U), state != 0U);
}
