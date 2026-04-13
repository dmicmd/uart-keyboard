#!/usr/bin/env python3
import os
import pathlib
import sys
import time

try:
    import serial
except ImportError:
    print("pyserial is not installed. Install with: pip install pyserial", file=sys.stderr)
    sys.exit(2)


def getenv(name: str, default: str) -> str:
    value = os.getenv(name, default)
    if not value:
        return default
    return value


def main() -> int:
    port = os.getenv("HIL_PORT")
    if not port:
        print("HIL_PORT is required (e.g. COM5 or /dev/ttyUSB0)", file=sys.stderr)
        return 2

    baud = int(getenv("HIL_BAUD", "115200"))
    timeout_sec = float(getenv("HIL_TIMEOUT_SEC", "3"))
    log_path = pathlib.Path(getenv("HIL_LOG_PATH", "hil/artifacts/hil_smoke.log"))
    log_path.parent.mkdir(parents=True, exist_ok=True)

    started = time.time()
    rx = bytearray()

    with serial.Serial(port=port, baudrate=baud, timeout=0.2) as ser:
        while (time.time() - started) < timeout_sec:
            chunk = ser.read(32)
            if chunk:
                rx.extend(chunk)
                break

    with log_path.open("w", encoding="utf-8") as f:
        f.write(f"port={port} baud={baud} timeout_sec={timeout_sec}\n")
        f.write(f"received_len={len(rx)}\n")
        if rx:
            f.write("received_hex=" + rx.hex() + "\n")

    if not rx:
        print("FAIL: no UART data received within timeout", file=sys.stderr)
        return 1

    print(f"PASS: received {len(rx)} byte(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
