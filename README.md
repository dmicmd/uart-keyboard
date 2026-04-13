# UART keyboard (ATmega128A)

Проект разделён на платформенно-независимую логику и AVR HAL с умеренным DI.

## Структура

- `src/core/*` — кросс-тестируемая бизнес-логика клавиатуры, RX decoder и расписание TX.
- `src/ring_buffer.c` + `include/ring_buffer.h` — кольцевой буфер в стиле LUFA.
- `src/platform/avr/hal_avr.c` — работа с регистрами AVR, ISR, GPIO/UART/Timer.
- `src/main.c` — композиция зависимостей.
- `tests/*` — host unit tests (gcc) для core-компонентов.

## DI подход

- Платформенные функции передаются через `platform_hal_t`.
- Логика клавиатуры получает callbacks `read_key/on_event`.
- Передатчик получает `send` callback.
- RingBuffer получает `critical_section_ops_t` с `enter/exit` (в AVR: cli/SREG restore).

## Сборка локально

```bash
make test        # host tests
make avr         # firmware .elf/.hex
```

## GitLab CI/CD

Пайплайн в `.gitlab-ci.yml`:

1. `unit_tests` — запускает `make test`.
2. `build_firmware` — собирает `.elf` и `.hex`, сохраняет artifacts.

Оба job запускаются на раннере с тегом `docker`.

## Документация процесса и требований

В `docs/` добавлены проектные документы:

- `SRS.md`
- `SRSw.md`
- `ARCHITECTURE.md`
- `ICD.md`
- `TRACEABILITY_MATRIX.md`
- `TEST_PLAN.md`
- `SAFETY_RISK_ANALYSIS.md`
- `CONFIGURATION_MANAGEMENT.md`

История изменений ведётся в `CHANGELOG.md`.
