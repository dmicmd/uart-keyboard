#include "tx_scheduler.h"

void tx_scheduler_init(tx_scheduler_t *tx, tx_scheduler_config_t cfg, tx_send_fn send, void *ctx)
{
    tx->cfg = cfg;
    tx->send = send;
    tx->ctx = ctx;
    tx->last_tx_ms = 0U;
}

bool tx_scheduler_try_send(tx_scheduler_t *tx, uint32_t now_ms, bool has_payload, uint8_t payload)
{
    if ((now_ms - tx->last_tx_ms) < tx->cfg.tx_pause_ms) {
        return false;
    }

    if (has_payload) {
        if (!tx->send(tx->ctx, payload)) {
            return false;
        }
        tx->last_tx_ms = now_ms;
        return true;
    }

    if ((now_ms - tx->last_tx_ms) >= tx->cfg.keepalive_ms) {
        if (!tx->send(tx->ctx, tx->cfg.keepalive_packet)) {
            return false;
        }
        tx->last_tx_ms = now_ms;
        return true;
    }

    return false;
}
