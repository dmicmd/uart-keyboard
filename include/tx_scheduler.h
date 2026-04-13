#ifndef TX_SCHEDULER_H
#define TX_SCHEDULER_H

#include <stdbool.h>
#include <stdint.h>

typedef bool (*tx_send_fn)(void *ctx, uint8_t data);

typedef struct {
    uint16_t tx_pause_ms;
    uint16_t keepalive_ms;
    uint8_t keepalive_packet;
} tx_scheduler_config_t;

typedef struct {
    tx_scheduler_config_t cfg;
    tx_send_fn send;
    void *ctx;
    uint32_t last_tx_ms;
} tx_scheduler_t;

void tx_scheduler_init(tx_scheduler_t *tx, tx_scheduler_config_t cfg, tx_send_fn send, void *ctx);
bool tx_scheduler_try_send(tx_scheduler_t *tx, uint32_t now_ms, bool has_payload, uint8_t payload);

#endif
