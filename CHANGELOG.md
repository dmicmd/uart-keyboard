# Changelog

Все заметные изменения проекта документируются в этом файле.

Формат основан на Keep a Changelog, версиях SemVer.

## [Unreleased]

### Added
- Модульная архитектура (core + AVR HAL + composition root).
- LUFA-style ring buffer с абстракцией critical section.
- Core-модули `keyboard_logic`, `tx_scheduler`, `rx_decoder`.
- Host unit tests для core-компонентов.
- GitLab CI/CD pipeline для тестов и сборки firmware artifacts.
- Пакет проектной документации в `docs/`:
  - SRS
  - SRSw
  - Architecture / Design
  - ICD
  - Traceability Matrix
  - Test Plan
  - Safety / Risk Analysis
  - Configuration Management

### Changed
- Монолитный `app.c` заменён модульной структурой.
