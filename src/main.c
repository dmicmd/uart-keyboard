#define F_CPU 7372800UL

#include "avr_irq.h"
#include "critical_section.h"
#include "keyboard_logic.h"
#include "platform_hal.h"
#include "rx_decoder.h"
#include "ring_buffer.h"
#include "tx_scheduler.h"
#include "uart_protocol.h"

#include <stdbool.h>
#include <stdint.h>

#define NUM_KEYS 12U
#define TX_QUEUE_SIZE 16U

static uint8_t tx_storage[TX_QUEUE_SIZE];
static ring_buffer_t tx_queue;

static keyboard_logic_t keyboard;
static uint8_t keyboard_raw_state[NUM_KEYS];
static uint32_t keyboard_debounce[NUM_KEYS];

static tx_scheduler_t tx;

static const platform_hal_t *g_hal;

static bool app_uart_send(void *ctx, uint8_t data)
{
    const platform_hal_t *hal = (const platform_hal_t *)ctx;
    return hal->uart_send(data);
}

static bool app_read_key(void *ctx, uint8_t index)
{
    const platform_hal_t *hal = (const platform_hal_t *)ctx;
    return hal->read_key(index);
}

static void app_on_key_event(void *ctx, uint8_t key_index, bool pressed)
{
    (void)ctx;
    (void)ring_buffer_push(&tx_queue, uart_protocol_key_packet(key_index, pressed));
}

static void app_set_led(void *ctx, uint8_t led_index, bool on)
{
    const platform_hal_t *hal = (const platform_hal_t *)ctx;
    hal->set_led(led_index, on);
}

int main(void)
{
    g_hal = platform_hal_get();
    g_hal->init();

    critical_section_ops_t cs = {
        .enter = avr_irq_enter,
        .exit = avr_irq_exit,
        .ctx = 0,
    };

    ring_buffer_init(&tx_queue, tx_storage, TX_QUEUE_SIZE, cs);

    keyboard_logic_init(&keyboard,
                        (keyboard_config_t){
                            .num_keys = NUM_KEYS,
                            .debounce_ms = 20,
                            .repeat_ms = 200,
                        },
                        (keyboard_io_t){
                            .read_key = app_read_key,
                            .on_event = app_on_key_event,
                            .ctx = (void *)g_hal,
                        },
                        keyboard_raw_state,
                        keyboard_debounce);

    tx_scheduler_init(&tx,
                      (tx_scheduler_config_t){
                          .tx_pause_ms = 20,
                          .keepalive_ms = 1000,
                          .keepalive_packet = 0x1F,
                      },
                      app_uart_send,
                      (void *)g_hal);

    g_hal->enable_interrupts();

    while (1) {
        uint32_t now = g_hal->millis();
        keyboard_logic_task(&keyboard, now);

        uint8_t rx_data;
        if (g_hal->uart_try_read(&rx_data)) {
            rx_decoder_handle_byte((rx_decoder_io_t){
                                      .set_led = app_set_led,
                                      .ctx = (void *)g_hal,
                                  },
                                  rx_data);
        }

        uint8_t payload;
        bool has_payload = ring_buffer_pop(&tx_queue, &payload);
        (void)tx_scheduler_try_send(&tx, now, has_payload, payload);
    }
}
