#ifndef UART_PROTOCOL_H
#define UART_PROTOCOL_H

#include <stdbool.h>
#include <stdint.h>

static inline uint8_t uart_protocol_key_packet(uint8_t key_index, bool pressed)
{
    uint8_t packet = (uint8_t)((key_index + 1U) & 0x1FU);
    if (pressed) {
        packet |= (1U << 7);
    }
    return packet;
}

#endif
