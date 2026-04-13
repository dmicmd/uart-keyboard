#include "app_runtime.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t now;
    bool key0;
    bool fail_first_send;
    uint8_t tx_attempts;
    uint8_t sent_count;
    uint8_t sent[4];
} fixture_t;

static fixture_t g_fixture;

static void init_fn(void)
{
}

static uint32_t millis_fn(void)
{
    return g_fixture.now;
}

static bool read_key_fn(uint8_t index)
{
    return (index == 0U) ? g_fixture.key0 : false;
}

static void set_led_fn(uint8_t led_index, bool on)
{
    (void)led_index;
    (void)on;
}

static bool uart_try_read_fn(uint8_t *data)
{
    (void)data;
    return false;
}

static bool uart_send_fn(uint8_t data)
{
    g_fixture.tx_attempts++;

    if (g_fixture.fail_first_send) {
        g_fixture.fail_first_send = false;
        return false;
    }

    g_fixture.sent[g_fixture.sent_count++] = data;
    return true;
}

static void enable_interrupts_fn(void)
{
}

static uint8_t enter_noop(void *ctx)
{
    (void)ctx;
    return 0;
}

static void exit_noop(void *ctx, uint8_t state)
{
    (void)ctx;
    (void)state;
}

int main(void)
{
    g_fixture = (fixture_t){
        .now = 0,
        .key0 = false,
        .fail_first_send = true,
        .tx_attempts = 0,
        .sent_count = 0,
    };

    const platform_hal_t hal = {
        .init = init_fn,
        .millis = millis_fn,
        .read_key = read_key_fn,
        .set_led = set_led_fn,
        .uart_try_read = uart_try_read_fn,
        .uart_send = uart_send_fn,
        .enable_interrupts = enable_interrupts_fn,
    };

    app_runtime_t app;
    uint8_t tx_storage[8];
    uint8_t raw[2];
    uint32_t db[2];

    app_runtime_init(&app,
                     &hal,
                     (app_runtime_config_t){
                         .num_keys = 2,
                         .tx_storage = tx_storage,
                         .tx_storage_size = 8,
                         .keyboard_raw_state = raw,
                         .keyboard_debounce_state = db,
                         .keyboard = {.num_keys = 2, .debounce_ms = 20, .repeat_ms = 200},
                         .tx = {.tx_pause_ms = 0, .keepalive_ms = 1000, .keepalive_packet = 0x1F},
                         .cs = {.enter = enter_noop, .exit = exit_noop, .ctx = 0},
                     });

    g_fixture.key0 = true;
    g_fixture.now = 0;
    app_runtime_tick(&app);

    g_fixture.now = 21;
    app_runtime_tick(&app);

    assert(g_fixture.tx_attempts == 1);
    assert(g_fixture.sent_count == 0);

    g_fixture.now = 22;
    app_runtime_tick(&app);

    assert(g_fixture.tx_attempts == 2);
    assert(g_fixture.sent_count == 1);
    assert(g_fixture.sent[0] == 0x81U);

    return 0;
}
