#!/usr/bin/env bash
set -euo pipefail

if ! command -v python3 >/dev/null 2>&1; then
  echo "python3 not found in PATH" >&2
  exit 2
fi

if [[ -z "${HIL_PORT:-}" ]]; then
  echo "HIL_PORT is not set (example: /dev/ttyUSB0)" >&2
  exit 2
fi

export HIL_BAUD="${HIL_BAUD:-115200}"
export HIL_TIMEOUT_SEC="${HIL_TIMEOUT_SEC:-3}"
export HIL_LOG_PATH="${HIL_LOG_PATH:-hil/artifacts/hil_smoke.log}"

python3 hil/tests/hil_smoke.py
