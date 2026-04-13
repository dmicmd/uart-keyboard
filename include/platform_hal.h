#ifndef PLATFORM_HAL_H
#define PLATFORM_HAL_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    void (*init)(void);
    uint32_t (*millis)(void);
    bool (*read_key)(uint8_t index);
    void (*set_led)(uint8_t led_index, bool on);
    bool (*uart_try_read)(uint8_t *data);
    bool (*uart_send)(uint8_t data);
    void (*enable_interrupts)(void);
} platform_hal_t;

const platform_hal_t *platform_hal_get(void);

#endif
