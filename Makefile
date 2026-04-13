APP=uart_keyboard

HOST_CC=gcc
HOST_CFLAGS=-std=c11 -Wall -Wextra -Werror -Iinclude

AVR_CC=avr-gcc
AVR_OBJCOPY=avr-objcopy
MCU=atmega128a
F_CPU=7372800UL
AVR_CFLAGS=-std=c11 -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra -Werror -Iinclude

CORE_SOURCES=src/ring_buffer.c src/core/keyboard_logic.c src/core/tx_scheduler.c src/core/rx_decoder.c
AVR_SOURCES=$(CORE_SOURCES) src/platform/avr/hal_avr.c src/main.c

.PHONY: all avr clean test

all: test

avr: build/$(APP).hex

build/$(APP).elf: $(AVR_SOURCES)
	@mkdir -p build
	$(AVR_CC) $(AVR_CFLAGS) $^ -o $@

build/$(APP).hex: build/$(APP).elf
	$(AVR_OBJCOPY) -O ihex -R .eeprom $< $@

test: build/test_ring_buffer build/test_keyboard_logic build/test_rx_decoder
	./build/test_ring_buffer
	./build/test_keyboard_logic
	./build/test_rx_decoder

build/test_ring_buffer: tests/test_ring_buffer.c src/ring_buffer.c include/ring_buffer.h include/critical_section.h
	@mkdir -p build
	$(HOST_CC) $(HOST_CFLAGS) tests/test_ring_buffer.c src/ring_buffer.c -o $@

build/test_keyboard_logic: tests/test_keyboard_logic.c src/core/keyboard_logic.c include/keyboard_logic.h
	@mkdir -p build
	$(HOST_CC) $(HOST_CFLAGS) tests/test_keyboard_logic.c src/core/keyboard_logic.c -o $@

build/test_rx_decoder: tests/test_rx_decoder.c src/core/rx_decoder.c include/rx_decoder.h
	@mkdir -p build
	$(HOST_CC) $(HOST_CFLAGS) tests/test_rx_decoder.c src/core/rx_decoder.c -o $@

clean:
	rm -rf build
