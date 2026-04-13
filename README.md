# UART keyboard firmware (ATmega128A)

Прошивка клавиатурного контроллера для AVR 8-bit с UART-протоколом событий клавиш и управления LED.

## Возможности

- Опрос 12 кнопок с debounce.
- Автоповтор активной клавиши.
- Передача событий по UART.
- Keepalive при отсутствии событий.
- Приём UART-команд управления 12 LED-каналами.
- Разделение на platform HAL и testable core.

## Архитектура (кратко)

- `src/core/` — платформенно-независимая логика (`keyboard_logic`, `tx_scheduler`, `rx_decoder`).
- `src/platform/avr/` — AVR HAL (GPIO/UART/Timer/ISR).
- `src/main.c` — минимальная точка входа (инициализация + бесконечный цикл).
- `src/app_runtime.c` — orchestration-слой приложения (tick без бизнес-логики в `main`).
- `src/ring_buffer.c` — кольцевой буфер с critical-section hooks.
- `tests/` — host unit tests.

## Требования к окружению

- Для host-тестов: `gcc`, `make`.
- Для AVR сборки: `avr-gcc`, `avr-libc`, `binutils-avr`.

## Сборка и проверка

```bash
make test   # host unit tests
make avr    # build/uart_keyboard.elf и build/uart_keyboard.hex
```

## CI/CD

GitLab pipeline (`.gitlab-ci.yml`):
- `unit_tests` → `make test`
- `build_firmware` → `make avr` + artifacts (`.elf`, `.hex`)

Runner tag: `docker`.

## Документация

Документы проекта находятся в `docs/`. Рекомендуемая точка входа: `docs/DOCUMENTS_INDEX.md`.

Ключевые документы:
- SRS / SRSw
- Architecture / ICD
- Pin Functional Specification
- Traceability Matrix
- Test Plan
- Safety / Risk Analysis
- Configuration Management

История изменений: `CHANGELOG.md`.

## HIL (Hardware-in-the-Loop)

Базовый каркас HIL находится в `hil/`:
- smoke-тест: `hil/tests/hil_smoke.py`
- запуск в Windows: `hil/scripts/windows/run_hil.ps1`
- запуск в Linux: `hil/scripts/linux/run_hil.sh`

См. `hil/README.md` для переменных окружения и сценариев запуска.
