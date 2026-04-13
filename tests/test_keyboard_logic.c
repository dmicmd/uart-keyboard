#include "keyboard_logic.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool key_state[3];
    uint8_t last_key;
    bool last_pressed;
    uint8_t events;
} fixture_t;

static bool read_key(void *ctx, uint8_t index)
{
    fixture_t *f = (fixture_t *)ctx;
    return f->key_state[index];
}

static void on_event(void *ctx, uint8_t key, bool pressed)
{
    fixture_t *f = (fixture_t *)ctx;
    f->last_key = key;
    f->last_pressed = pressed;
    f->events++;
}

int main(void)
{
    fixture_t f = {0};
    keyboard_logic_t kb;
    uint8_t raw[3];
    uint32_t db[3];

    keyboard_logic_init(&kb,
                        (keyboard_config_t){.num_keys = 3, .debounce_ms = 10, .repeat_ms = 50},
                        (keyboard_io_t){.read_key = read_key, .on_event = on_event, .ctx = &f},
                        raw,
                        db);

    f.key_state[1] = true;
    keyboard_logic_task(&kb, 0);
    keyboard_logic_task(&kb, 11);
    assert(f.events == 1);
    assert(f.last_key == 1 && f.last_pressed);

    keyboard_logic_task(&kb, 61);
    assert(f.events == 2);
    assert(f.last_pressed);

    f.key_state[1] = false;
    keyboard_logic_task(&kb, 72);
    keyboard_logic_task(&kb, 83);
    assert(f.events == 3);
    assert(!f.last_pressed);

    return 0;
}
