# Test Plan / Specification

## 1. Цели

1. Проверить корректность core-логики независимо от AVR HAL.
2. Проверить воспроизводимость сборки host + AVR.

## 2. Уровни тестирования

- **Unit (host gcc):** `tests/test_*`.
- **Build verification:** `make avr`.
- **CI verification:** jobs `unit_tests`, `build_firmware`.

## 3. Набор unit-тестов

## 3.1 Ring Buffer

Файл: `tests/test_ring_buffer.c`

Проверяет:
- push/pop порядок FIFO;
- поведение на переполнении/пустом буфере;
- симметрию вызовов `enter/exit` критической секции.

## 3.2 Keyboard Logic

Файл: `tests/test_keyboard_logic.c`

Проверяет:
- debounce перехода в pressed;
- генерацию повтора;
- генерацию release.

## 3.3 RX Decoder

Файл: `tests/test_rx_decoder.c`

Проверяет:
- декодирование валидных LED-команд;
- игнорирование невалидного индекса.

## 4. Критерии приёмки

- `make test` завершается успешно.
- `make avr` успешно собирает `.elf` и `.hex` в CI.
- Артефакты доступны в GitLab pipeline.

## 5. План расширения

- Добавить unit-тесты для `tx_scheduler`.
- Добавить интеграционный тест полного TX пути (event → UART byte).
