#include "platform_hal.h"
#include "critical_section.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

static volatile uint32_t g_ms = 0;
static volatile uint8_t g_rx_byte = 0;
static volatile uint8_t g_rx_ready = 0;

ISR(TIMER0_COMP_vect)
{
    g_ms++;
}

static void timer0_init(void)
{
    TCCR0 = (1 << WGM01) | (1 << CS02);
    OCR0 = 114;
    TIMSK |= (1 << OCIE0);
}

ISR(USART0_RX_vect)
{
    g_rx_byte = UDR0;
    g_rx_ready = 1U;
}

static uint32_t avr_millis(void)
{
    uint8_t sreg = SREG;
    cli();
    uint32_t snapshot = g_ms;
    SREG = sreg;
    return snapshot;
}

static bool avr_read_key(uint8_t i)
{
    switch (i) {
        case 0: return (PINF & (1 << 7)) == 0;
        case 1: return (PINF & (1 << 6)) == 0;
        case 2: return (PINF & (1 << 5)) == 0;
        case 3: return (PINF & (1 << 4)) == 0;
        case 4: return (PINF & (1 << 3)) == 0;
        case 5: return (PINF & (1 << 2)) == 0;
        case 6: return (PIND & (1 << 0)) == 0;
        case 7: return (PIND & (1 << 1)) == 0;
        case 8: return (PIND & (1 << 2)) == 0;
        case 9: return (PIND & (1 << 3)) == 0;
        case 10: return (PIND & (1 << 4)) == 0;
        case 11: return (PIND & (1 << 5)) == 0;
        default: return false;
    }
}

static void avr_set_led(uint8_t led_index, bool on)
{
    if (led_index < 6U) {
        if (on) {
            PORTA |= (1U << led_index);
        } else {
            PORTA &= (uint8_t)~(1U << led_index);
        }
        return;
    }

    led_index = (uint8_t)(led_index - 6U);
    if (led_index < 6U) {
        if (on) {
            PORTC |= (1U << led_index);
        } else {
            PORTC &= (uint8_t)~(1U << led_index);
        }
    }
}

static bool avr_uart_try_read(uint8_t *data)
{
    uint8_t sreg = SREG;
    cli();
    if (!g_rx_ready) {
        SREG = sreg;
        return false;
    }
    *data = g_rx_byte;
    g_rx_ready = 0U;
    SREG = sreg;
    return true;
}

static bool avr_uart_send(uint8_t data)
{
    while (!(UCSR0A & (1 << UDRE0))) {
    }
    UDR0 = data;
    return true;
}

static void avr_enable_interrupts(void)
{
    sei();
}

static void keyboard_gpio_init(void)
{
    DDRF &= (uint8_t)~((1 << 7) | (1 << 6) | (1 << 5) | (1 << 4) | (1 << 3) | (1 << 2));
    PORTF |= (1 << 7) | (1 << 6) | (1 << 5) | (1 << 4) | (1 << 3) | (1 << 2);

    DDRD &= (uint8_t)~0x3FU;
    PORTD |= 0x3FU;
}

static void led_gpio_init(void)
{
    DDRA |= 0x3FU;
    DDRC |= 0x3FU;
    PORTA &= (uint8_t)~0x3FU;
    PORTC &= (uint8_t)~0x3FU;
}

static void uart0_init(void)
{
    UBRR0H = 0;
    UBRR0L = 3;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

static void avr_init(void)
{
    led_gpio_init();
    keyboard_gpio_init();
    uart0_init();
    timer0_init();
}

static const platform_hal_t HAL = {
    .init = avr_init,
    .millis = avr_millis,
    .read_key = avr_read_key,
    .set_led = avr_set_led,
    .uart_try_read = avr_uart_try_read,
    .uart_send = avr_uart_send,
    .enable_interrupts = avr_enable_interrupts,
};

const platform_hal_t *platform_hal_get(void)
{
    return &HAL;
}

uint8_t avr_irq_enter(void *ctx)
{
    (void)ctx;
    uint8_t sreg = SREG;
    cli();
    return sreg;
}

void avr_irq_exit(void *ctx, uint8_t state)
{
    (void)ctx;
    SREG = state;
}
