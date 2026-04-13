#ifndef APP_RUNTIME_H
#define APP_RUNTIME_H

#include "critical_section.h"
#include "keyboard_logic.h"
#include "platform_hal.h"
#include "ring_buffer.h"
#include "tx_scheduler.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t num_keys;
    uint8_t *tx_storage;
    uint8_t tx_storage_size;
    uint8_t *keyboard_raw_state;
    uint32_t *keyboard_debounce_state;

    keyboard_config_t keyboard;
    tx_scheduler_config_t tx;
    critical_section_ops_t cs;
} app_runtime_config_t;

typedef struct {
    const platform_hal_t *hal;
    ring_buffer_t tx_queue;
    keyboard_logic_t keyboard;
    tx_scheduler_t tx;

    uint8_t pending_payload;
    bool pending_valid;
} app_runtime_t;

void app_runtime_init(app_runtime_t *app, const platform_hal_t *hal, app_runtime_config_t cfg);
void app_runtime_tick(app_runtime_t *app);

#endif
