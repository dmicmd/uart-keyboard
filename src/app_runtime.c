#include "app_runtime.h"

#include "rx_decoder.h"
#include "uart_protocol.h"

static bool app_uart_send(void *ctx, uint8_t data)
{
    app_runtime_t *app = (app_runtime_t *)ctx;
    return app->hal->uart_send(data);
}

static bool app_read_key(void *ctx, uint8_t index)
{
    app_runtime_t *app = (app_runtime_t *)ctx;
    return app->hal->read_key(index);
}

static void app_on_key_event(void *ctx, uint8_t key_index, bool pressed)
{
    app_runtime_t *app = (app_runtime_t *)ctx;
    (void)ring_buffer_push(&app->tx_queue, uart_protocol_key_packet(key_index, pressed));
}

static void app_set_led(void *ctx, uint8_t led_index, bool on)
{
    app_runtime_t *app = (app_runtime_t *)ctx;
    app->hal->set_led(led_index, on);
}

void app_runtime_init(app_runtime_t *app, const platform_hal_t *hal, app_runtime_config_t cfg)
{
    cfg.keyboard.num_keys = cfg.num_keys;

    app->hal = hal;
    app->pending_payload = 0U;
    app->pending_valid = false;

    ring_buffer_init(&app->tx_queue, cfg.tx_storage, cfg.tx_storage_size, cfg.cs);

    keyboard_logic_init(&app->keyboard,
                        cfg.keyboard,
                        (keyboard_io_t){
                            .read_key = app_read_key,
                            .on_event = app_on_key_event,
                            .ctx = app,
                        },
                        cfg.keyboard_raw_state,
                        cfg.keyboard_debounce_state);

    tx_scheduler_init(&app->tx, cfg.tx, app_uart_send, app);
}

static void process_rx(app_runtime_t *app)
{
    uint8_t rx_data;
    if (!app->hal->uart_try_read(&rx_data)) {
        return;
    }

    rx_decoder_handle_byte((rx_decoder_io_t){
                              .set_led = app_set_led,
                              .ctx = app,
                          },
                          rx_data);
}

static void process_tx(app_runtime_t *app, uint32_t now)
{
    if (!app->pending_valid) {
        uint8_t payload;
        if (ring_buffer_peek(&app->tx_queue, &payload)) {
            app->pending_payload = payload;
            app->pending_valid = true;
        }
    }

    if (app->pending_valid) {
        if (tx_scheduler_try_send(&app->tx, now, true, app->pending_payload)) {
            uint8_t drop;
            (void)ring_buffer_pop(&app->tx_queue, &drop);
            app->pending_valid = false;
        }
        return;
    }

    (void)tx_scheduler_try_send(&app->tx, now, false, 0U);
}

void app_runtime_tick(app_runtime_t *app)
{
    uint32_t now = app->hal->millis();

    keyboard_logic_task(&app->keyboard, now);
    process_rx(app);
    process_tx(app, now);
}
