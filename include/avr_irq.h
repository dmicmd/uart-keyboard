#ifndef AVR_IRQ_H
#define AVR_IRQ_H

#include <stdint.h>

uint8_t avr_irq_enter(void *ctx);
void avr_irq_exit(void *ctx, uint8_t state);

#endif
