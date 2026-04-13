$ErrorActionPreference = "Stop"

if (-not (Get-Command python -ErrorAction SilentlyContinue)) {
  throw "Python not found in PATH"
}

if (-not $env:HIL_PORT) {
  throw "HIL_PORT is not set (example: COM5)"
}

if (-not $env:HIL_BAUD) {
  $env:HIL_BAUD = "115200"
}

if (-not $env:HIL_TIMEOUT_SEC) {
  $env:HIL_TIMEOUT_SEC = "3"
}

if (-not $env:HIL_LOG_PATH) {
  $env:HIL_LOG_PATH = "hil/artifacts/hil_smoke.log"
}

python hil/tests/hil_smoke.py
