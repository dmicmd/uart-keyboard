# Architecture / Design

## 1. Принципы

- Умеренный DI: зависимости задаются callback-структурами, без сложных контейнеров.
- Разделение ответственности: `core` (чистая логика) vs `platform/avr` (регистры/ISR).
- Тестируемость: core собирается и тестируется host-компилятором.

## 2. Слои

1. **Application composition** (`src/main.c`)
   - связывает HAL, core и очереди;
   - управляет циклом обработки.
2. **Core** (`src/core/*`)
   - `keyboard_logic`: debounce/active/repeat;
   - `tx_scheduler`: pacing/keepalive;
   - `rx_decoder`: декодирование команд LED.
3. **Infrastructure**
   - `ring_buffer` + `critical_section` abstraction.
4. **Platform HAL** (`src/platform/avr/hal_avr.c`)
   - GPIO, UART, timer, ISR, millis.

## 3. Потоки данных

### 3.1 TX путь

`keyboard_logic` → callback `on_event` → `ring_buffer_push` → `tx_scheduler_try_send` → `hal.uart_send`.

### 3.2 RX путь

UART RX ISR → HAL mailbox (`g_rx_byte`, `g_rx_ready`) → `hal.uart_try_read` → `rx_decoder_handle_byte` → `hal.set_led`.

## 4. Почему так

- ISR остаются короткими и предсказуемыми.
- Протокол и бизнес-правила не завязаны на AVR-регистры.
- Легко добавить новую платформу через альтернативную реализацию HAL.
