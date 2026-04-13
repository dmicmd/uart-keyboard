#ifndef KEYBOARD_LOGIC_H
#define KEYBOARD_LOGIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef bool (*key_read_fn)(void *ctx, uint8_t index);
typedef void (*key_event_fn)(void *ctx, uint8_t key_index, bool pressed);

typedef struct {
    key_read_fn read_key;
    key_event_fn on_event;
    void *ctx;
} keyboard_io_t;

typedef struct {
    uint8_t num_keys;
    uint16_t debounce_ms;
    uint16_t repeat_ms;
} keyboard_config_t;

typedef struct {
    keyboard_config_t cfg;
    keyboard_io_t io;
    uint8_t active_key;
    uint8_t *raw_state;
    uint32_t *debounce_start;
    uint32_t repeat_due;
} keyboard_logic_t;

void keyboard_logic_init(keyboard_logic_t *kb,
                         keyboard_config_t cfg,
                         keyboard_io_t io,
                         uint8_t *raw_state_storage,
                         uint32_t *debounce_storage);

void keyboard_logic_task(keyboard_logic_t *kb, uint32_t now_ms);

#endif
