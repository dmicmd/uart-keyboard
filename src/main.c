#define F_CPU 7372800UL

#include "app_runtime.h"
#include "avr_irq.h"
#include "platform_hal.h"

#include <stdint.h>

#define NUM_KEYS 12U
#define TX_QUEUE_SIZE 16U

static uint8_t tx_storage[TX_QUEUE_SIZE];
static uint8_t keyboard_raw_state[NUM_KEYS];
static uint32_t keyboard_debounce[NUM_KEYS];

int main(void)
{
    const platform_hal_t *hal = platform_hal_get();
    app_runtime_t app;

    hal->init();

    app_runtime_init(&app,
                     hal,
                     (app_runtime_config_t){
                         .num_keys = NUM_KEYS,
                         .tx_storage = tx_storage,
                         .tx_storage_size = TX_QUEUE_SIZE,
                         .keyboard_raw_state = keyboard_raw_state,
                         .keyboard_debounce_state = keyboard_debounce,
                         .keyboard = {
                             .num_keys = NUM_KEYS,
                             .debounce_ms = 20,
                             .repeat_ms = 200,
                         },
                         .tx = {
                             .tx_pause_ms = 20,
                             .keepalive_ms = 1000,
                             .keepalive_packet = 0x1F,
                         },
                         .cs = {
                             .enter = avr_irq_enter,
                             .exit = avr_irq_exit,
                             .ctx = 0,
                         },
                     });

    hal->enable_interrupts();

    while (1) {
        app_runtime_tick(&app);
    }
}
