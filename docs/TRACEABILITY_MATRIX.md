# Traceability Matrix

> Аудитория: разработчики, тестировщики и инженеры сопровождения, работающие с прошивкой.
> Документ описывает проект в эксплуатационном формате и используется как рабочий артефакт в MR/релизном процессе.

| ID требования | Описание | Код | Тесты | Статус |
|---|---|---|---|---|
| SRS-F-01 / SRSw-01 | 12 клавиш | `src/main.c`, `src/platform/avr/hal_avr.c`, `src/core/keyboard_logic.c` | `tests/test_keyboard_logic.c` | Covered |
| SRS-F-02 / SRSw-KB-02 | Debounce | `src/core/keyboard_logic.c` | `tests/test_keyboard_logic.c` | Covered |
| SRS-F-03 / SRSw-KB-04 | Autorepeat | `src/core/keyboard_logic.c` | `tests/test_keyboard_logic.c` | Covered |
| SRS-F-04 | TX событий клавиш | `include/uart_protocol.h`, `src/main.c` | (интеграционный, planned) | Partial |
| SRS-F-05 / SRSw-TX-* | Keepalive и pacing | `src/core/tx_scheduler.c` | `tests/test_tx_scheduler.c` | Covered |
| SRS-F-06 / SRSw-RX-* | RX LED команды | `src/core/rx_decoder.c`, `src/platform/avr/hal_avr.c`, `src/main.c` | `tests/test_rx_decoder.c` | Covered |
| SRS-NF-01 | Модульность HAL/core | `include/platform_hal.h`, `src/core/*`, `src/platform/avr/*` | code review | Covered |
| SRS-NF-02 / SRSw-QA-01 | Host unit tests | `tests/*`, `Makefile` | `make test` | Covered |
| SRS-NF-03 / SRSw-QA-02 | CI build+test | `.gitlab-ci.yml` | GitLab pipeline | Covered |

## Примечания

- Для UART TX полного пути рекомендуется HIL/интеграционный тест.
