#include "tx_scheduler.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t sent[8];
    uint8_t count;
} fixture_t;

static bool send_cb(void *ctx, uint8_t data)
{
    fixture_t *f = (fixture_t *)ctx;
    if (f->count >= 8) {
        return false;
    }
    f->sent[f->count++] = data;
    return true;
}

int main(void)
{
    fixture_t f = {0};
    tx_scheduler_t tx;

    tx_scheduler_init(&tx,
                      (tx_scheduler_config_t){.tx_pause_ms = 20, .keepalive_ms = 1000, .keepalive_packet = 0x1FU},
                      send_cb,
                      &f);

    /* Пауза еще не истекла — ничего не уходит. */
    assert(!tx_scheduler_try_send(&tx, 10, true, 0x81));
    assert(f.count == 0);

    /* Истекла пауза и есть payload — отправляем payload. */
    assert(tx_scheduler_try_send(&tx, 20, true, 0x81));
    assert(f.count == 1 && f.sent[0] == 0x81);

    /* Снова до паузы — не отправляем. */
    assert(!tx_scheduler_try_send(&tx, 30, true, 0x82));
    assert(f.count == 1);

    /* После паузы, payload отсутствует и keepalive еще рано. */
    assert(!tx_scheduler_try_send(&tx, 100, false, 0));
    assert(f.count == 1);

    /* После keepalive timeout — отправляем keepalive. */
    assert(tx_scheduler_try_send(&tx, 1020, false, 0));
    assert(f.count == 2 && f.sent[1] == 0x1FU);

    return 0;
}
