#include "keyboard_logic.h"

#define KEY_NONE 0xFFU
#define DEBOUNCE_IDLE 0xFFFFFFFFUL

static void detect_stable_states(keyboard_logic_t *kb, uint32_t now_ms)
{
    for (uint8_t i = 0; i < kb->cfg.num_keys; ++i) {
        uint8_t phys = kb->io.read_key(kb->io.ctx, i) ? 1U : 0U;

        if (phys != kb->raw_state[i]) {
            if (kb->debounce_start[i] == DEBOUNCE_IDLE) {
                kb->debounce_start[i] = now_ms;
            }

            if ((now_ms - kb->debounce_start[i]) >= kb->cfg.debounce_ms) {
                kb->raw_state[i] = phys;
                kb->debounce_start[i] = DEBOUNCE_IDLE;
            }
        } else {
            kb->debounce_start[i] = DEBOUNCE_IDLE;
        }
    }
}

static void pick_next_active(keyboard_logic_t *kb, uint32_t now_ms)
{
    for (uint8_t i = 0; i < kb->cfg.num_keys; ++i) {
        if (kb->raw_state[i]) {
            kb->active_key = i;
            kb->repeat_due = now_ms + kb->cfg.repeat_ms;
            kb->io.on_event(kb->io.ctx, i, true);
            break;
        }
    }
}

void keyboard_logic_init(keyboard_logic_t *kb,
                         keyboard_config_t cfg,
                         keyboard_io_t io,
                         uint8_t *raw_state_storage,
                         uint32_t *debounce_storage)
{
    kb->cfg = cfg;
    kb->io = io;
    kb->active_key = KEY_NONE;
    kb->raw_state = raw_state_storage;
    kb->debounce_start = debounce_storage;
    kb->repeat_due = 0U;

    for (uint8_t i = 0; i < cfg.num_keys; ++i) {
        kb->raw_state[i] = 0U;
        kb->debounce_start[i] = DEBOUNCE_IDLE;
    }
}

void keyboard_logic_task(keyboard_logic_t *kb, uint32_t now_ms)
{
    detect_stable_states(kb, now_ms);

    if (kb->active_key == KEY_NONE) {
        pick_next_active(kb, now_ms);
        return;
    }

    if (!kb->raw_state[kb->active_key]) {
        kb->io.on_event(kb->io.ctx, kb->active_key, false);
        kb->active_key = KEY_NONE;
        pick_next_active(kb, now_ms);
        return;
    }

    if ((int32_t)(now_ms - kb->repeat_due) >= 0) {
        kb->repeat_due = now_ms + kb->cfg.repeat_ms;
        kb->io.on_event(kb->io.ctx, kb->active_key, true);
    }
}
