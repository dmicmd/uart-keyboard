/**
 * @file main.c
 * @brief UART keyboard with active backlight (ATmega128A)
 */

#define F_CPU 7372800UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

/* ===================== Configuration ===================== */

#define NUM_KEYS        12
#define DEBOUNCE_MS     20
#define TX_PAUSE_MS     20
#define KEEPALIVE_MS    1000
#define REPEAT_MS       200
#define NO_KEY          0xFF

/* ===================== Globals ===================== */

static volatile uint32_t g_ms = 0;

/* ===================== UART TX queue ===================== */

#define TX_QUEUE_SIZE 16
static uint8_t tx_queue[TX_QUEUE_SIZE];
static uint8_t tx_head = 0;
static uint8_t tx_tail = 0;

/* ===================== Queue ===================== */

static inline uint8_t queue_is_empty(void)
{
    return tx_head == tx_tail;
}

static inline uint8_t queue_is_full(void)
{
    return ((tx_head + 1) % TX_QUEUE_SIZE) == tx_tail;
}

static void queue_push(uint8_t data)
{
    if (queue_is_full())
        return;

    tx_queue[tx_head] = data;
    tx_head = (tx_head + 1) % TX_QUEUE_SIZE;
}

static uint8_t queue_pop(void)
{
    uint8_t data = tx_queue[tx_tail];
    tx_tail = (tx_tail + 1) % TX_QUEUE_SIZE;
    return data;
}

/* ===================== Timer ===================== */

ISR(TIMER0_COMP_vect)
{
    g_ms++;
}

static void timer0_init(void)
{
    /* CTC, prescaler 64 */
    TCCR0 = (1 << WGM01) | (1 << CS02);
    OCR0  = 114; /* ~1 ms @ 7.3728 MHz */
    TIMSK |= (1 << OCIE0);
}

/* ===================== UART0 ===================== */

ISR(USART0_RX_vect)
{
    uint8_t data = UDR0;

    uint8_t led = data & 0x1F;
    uint8_t state = (data >> 6) & 0x03;

    if (led >= 1 && led <= 12)
    {
        uint8_t idx = led - 1;

        if (idx < 6)
        {
            /* PA0–PA5 */
            if (state)
                PORTA |=  (1 << idx);
            else
                PORTA &= ~(1 << idx);
        }
        else
        {
            /* PC0–PC5 */
            uint8_t bit = idx - 6;
            if (state)
                PORTC |=  (1 << bit);
            else
                PORTC &= ~(1 << bit);
        }
    }
}

static void uart_init(void)
{
    /* 115200 baud @ 7.3728 MHz → UBRR = 3 */
    UBRR0H = 0;
    UBRR0L = 3;

    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

static void uart_send(uint8_t data)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

/* ===================== Keyboard ===================== */

/* Чтение кнопок напрямую с портов */
static uint8_t read_key(uint8_t i)
{
    switch (i)
    {
        /* PF7–PF2 */
        case 0: return !(PINF & (1 << 7));
        case 1: return !(PINF & (1 << 6));
        case 2: return !(PINF & (1 << 5));
        case 3: return !(PINF & (1 << 4));
        case 4: return !(PINF & (1 << 3));
        case 5: return !(PINF & (1 << 2));

        /* PD0–PD5 */
        case 6: return !(PIND & (1 << 0));
        case 7: return !(PIND & (1 << 1));
        case 8: return !(PIND & (1 << 2));
        case 9: return !(PIND & (1 << 3));
        case 10:return !(PIND & (1 << 4));
        case 11:return !(PIND & (1 << 5));
    }
    return 0;
}

static void keyboard_init(void)
{
    /* PF2–PF7 input + pull-up */
    DDRF &= ~((1 << 7)|(1 << 6)|(1 << 5)|(1 << 4)|(1 << 3)|(1 << 2));
    PORTF |=  ((1 << 7)|(1 << 6)|(1 << 5)|(1 << 4)|(1 << 3)|(1 << 2));

    /* PD0–PD5 input + pull-up */
    DDRD &= ~0x3F;
    PORTD |=  0x3F;
}

static void send_key(uint8_t i, uint8_t pressed)
{
    uint8_t packet = (i + 1) & 0x1F;
    if (pressed)
        packet |= (1 << 7);

    queue_push(packet);
}

static void keyboard_task(void)
{
    static uint8_t  active_key = NO_KEY;
    static uint8_t  raw_state[NUM_KEYS];
    static uint32_t debounce_time[NUM_KEYS];
    static uint32_t repeat_time = 0;

    uint32_t now = g_ms;

    /* ---------- Читаем все кнопки ---------- */
    for (uint8_t i = 0; i < NUM_KEYS; i++)
    {
        uint8_t phys = read_key(i);

        if (phys != raw_state[i])
        {
            /* старт debounce */
            if (debounce_time[i] == 0)
                debounce_time[i] = now;

            if ((now - debounce_time[i]) >= DEBOUNCE_MS)
            {
                raw_state[i] = phys;
                debounce_time[i] = 0;
            }
        }
        else
        {
            debounce_time[i] = 0;
        }
    }

    /* ---------- Если нет активной — ищем первую ---------- */
    if (active_key == NO_KEY)
    {
        for (uint8_t i = 0; i < NUM_KEYS; i++)
        {
            if (raw_state[i])
            {
                active_key = i;
                repeat_time = now + REPEAT_MS;
                send_key(i, 1);
                break;
            }
        }
        return;
    }

    /* ---------- Если активная отпущена ---------- */
    if (!raw_state[active_key])
    {
        send_key(active_key, 0);
        active_key = NO_KEY;

        /* Ищем следующую */
        for (uint8_t i = 0; i < NUM_KEYS; i++)
        {
            if (raw_state[i])
            {
                active_key = i;
                repeat_time = now + REPEAT_MS;
                send_key(i, 1);
                break;
            }
        }
        return;
    }

    /* ---------- Автоповтор ---------- */
    if ((int32_t)(now - repeat_time) >= 0)
    {
        repeat_time = now + REPEAT_MS;
        send_key(active_key, 1);
    }
}

/* ===================== UART TX task ===================== */

static void uart_tx_task(void)
{
    static uint32_t last_tx_ms = 0;

    if ((g_ms - last_tx_ms) < TX_PAUSE_MS)
        return;

    if (!queue_is_empty())
    {
        uart_send(queue_pop());
        last_tx_ms = g_ms;
        return;
    }

    if ((g_ms - last_tx_ms) >= KEEPALIVE_MS)
    {
        uart_send(0x1F);
        last_tx_ms = g_ms;
    }
}

/* ===================== Main ===================== */

int main(void)
{
    /* LEDs outputs */
    DDRA |= 0x3F;   /* PA0–PA5 */
    DDRC |= 0x3F;   /* PC0–PC5 */
    PORTA &= ~0x3F;
    PORTC &= ~0x3F;

    keyboard_init();
    uart_init();
    timer0_init();

    sei();

    while (1)
    {
        keyboard_task();
        uart_tx_task();
    }
}