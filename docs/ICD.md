# ICD — Interface Control Document

> Аудитория: разработчики, тестировщики и инженеры сопровождения, работающие с прошивкой.
> Документ описывает проект в эксплуатационном формате и используется как рабочий артефакт в MR/релизном процессе.

## 1. Внутренние интерфейсы модулей

## 1.1 `platform_hal_t`

```c
void (*init)(void);
uint32_t (*millis)(void);
bool (*read_key)(uint8_t index);
void (*set_led)(uint8_t led_index, bool on);
bool (*uart_try_read)(uint8_t *data);
bool (*uart_send)(uint8_t data);
void (*enable_interrupts)(void);
```

Контракт:
- `millis` монотонен (переполнение допускается, сравнения через разность).
- `uart_try_read` неблокирующий.
- `uart_send` выполняет отправку одного байта.

## 1.2 `keyboard_logic`

Вход:
- `read_key(ctx, i)`.

Выход:
- `on_event(ctx, key_index, pressed)`.

## 1.3 `tx_scheduler`

Вход:
- текущее время, наличие payload.

Выход:
- вызов `send(ctx, data)` при выполнении условий pacing/keepalive.

## 1.4 `rx_decoder`

Вход:
- байт UART RX.

Выход:
- `set_led(ctx, led_index_0_based, on)` для валидного пакета.

## 1.5 `ring_buffer`

Операции:
- `push/pop/is_empty/is_full`.

Параметры синхронизации:
- `critical_section_ops_t { enter, exit, ctx }`.

## 2. Внешний интерфейс протокола (UART)

### 2.1 TX пакет клавиши

- Биты [4:0]: `key_id = index + 1`.
- Бит [7]: `pressed`.

### 2.2 RX пакет LED

- Биты [4:0]: `led_id` в диапазоне 1..12.
- Биты [7:6]: состояние LED (0 off, иначе on).
