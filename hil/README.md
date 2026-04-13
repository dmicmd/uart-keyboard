# HIL (Hardware-in-the-Loop) test scaffold

Этот каталог содержит минимальный, но рабочий каркас HIL для проекта прошивки.

## Цели

- отделить host unit tests от тестов с реальным железом;
- запускать HIL вручную/по расписанию в CI;
- поддержать локальный запуск на Windows и Linux.

## Структура

- `hil/tests/hil_smoke.py` — smoke-сценарий проверки UART взаимодействия.
- `hil/scripts/windows/run_hil.ps1` — запуск HIL в Windows.
- `hil/scripts/linux/run_hil.sh` — запуск HIL в Linux.
- `hil/artifacts/` — логи и артефакты прогона.

## Быстрый старт (Windows)

```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install pyserial
$env:HIL_PORT = "COM5"
$env:HIL_BAUD = "115200"
pwsh ./hil/scripts/windows/run_hil.ps1
```

## Быстрый старт (Linux)

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install pyserial
export HIL_PORT=/dev/ttyUSB0
export HIL_BAUD=115200
bash ./hil/scripts/linux/run_hil.sh
```

## Переменные окружения

- `HIL_PORT` — serial-порт стенда (например `COM5` или `/dev/ttyUSB0`).
- `HIL_BAUD` — baud rate (по умолчанию `115200`).
- `HIL_TIMEOUT_SEC` — timeout чтения (по умолчанию `3`).
- `HIL_LOG_PATH` — путь к лог-файлу (по умолчанию `hil/artifacts/hil_smoke.log`).

## Что проверяет smoke-test

1. Порт успешно открывается.
2. Чтение данных с UART выполняется без исключений.
3. За timeout получен хотя бы один байт (например keepalive).

> На раннем этапе этого достаточно как smoke. По мере готовности стенда добавляются шаги управления входами клавиш и проверки LED-линий.
