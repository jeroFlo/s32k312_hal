# S32K312 Tasks

This repository contains embedded C exercises and reusable drivers for the
NXP S32K312 microcontroller. The projects are developed for S32 Design Studio
(S32DS) and use the ARM Cortex-M7 startup code, CMSIS support, S32K312 device
definitions, and linker configurations provided by the project.

The source is organized in layers:

```text
+------------------------------------------------------+
| Applications                                         |
| apps/                                                |
| Task-specific behavior and demonstrations            |
+--------------------------+---------------------------+
                           |
                           v
+------------------------------------------------------+
| Reusable components                                   |
| components/                                           |
| MCU-independent services and protocol parsers        |
+--------------------------+---------------------------+
                           |
                           v
+------------------------------------------------------+
| Hardware abstraction layer                            |
| hal/                                                  |
| S32K312 peripheral drivers and interrupt adapters    |
+--------------------------+---------------------------+
                           |
                           v
+------------------------------------------------------+
| MCU and startup support                               |
| include/, Project_Settings/, src/                    |
| Device definitions, startup, system, and base code    |
+------------------------------------------------------+
```

## Repository layout

### `apps/`

Application-level exercises. Each application has its own `main.c`, optional
application header, and README.

- `apps/hw01_gpio_timer/`: GPIO, board switch, LED, and timer exercise.
- `apps/hw02_uart_gps/`: UART GPS exercise. It receives GNGGA data through
  LPUART6, parses latitude and longitude, and prints the result through UART.

Applications should contain the behavior being demonstrated. Register-level
peripheral configuration belongs in `hal/`, and reusable protocol or data
structures belong in `components/`.

### `components/`

Reusable modules that are intended to work independently of a particular
application.

- `components/ring_buffer/`: Byte ring buffer used to decouple interrupt-driven
  UART reception from foreground processing.
- `components/nmea_gps/`: GNGGA parser and GPS data formatting. The parser
  consumes one byte at a time and exposes parser initialization, RX processing,
  and coordinate output functions.

A new microcontroller interface can reuse these components by providing bytes
through the same APIs, without copying the parser or ring-buffer logic.

### `hal/`

Hardware Abstraction Layer for S32K312 peripherals. This directory contains
register-level drivers and hardware-specific adapters. 

- `GPIO.c/.h`: GPIO and board pin control.
- `timer.c/.h`: Timer configuration and control.
- `UART.c/.h`: S32K312 LPUART6 initialization, transmit/receive helpers, RX
  interrupt handling, and connection to the receive ring buffer.
- `helper.h`: Shared hardware macros, clock definitions, and small common types.

The HAL is the hardware-dependent boundary. Application code should use these
APIs instead of accessing S32K312 peripheral registers directly.

When adding a new application:

1. Create its source under `apps/<application_name>/`.
2. Reuse existing modules from `components/` where possible.
3. Add hardware-specific code to `hal/` only when it belongs to a peripheral
   adapter or driver.
4. Configure the application target in S32DS so its generated build metadata
   includes the new sources.
5. Add an application README describing its purpose and runtime flow.
