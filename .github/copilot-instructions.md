# Copilot instructions for SquachWatch-CYD

## Project overview

This repository is a PlatformIO-based ESP32 firmware project for SquachWatch-CYD, a Wi‑Fi/Bluetooth surveillance-device detector. The codebase is not a generic Arduino sketch; it is a multi-board firmware family with shared application code and many board-specific device setup headers.

- Source code lives in `src/` and shared headers in `include/`.
- `platformio.ini` defines the board matrix and the build-time configuration for each environment.
- `include/*_user_setup.h` files are the per-board display/touch configuration layer; many environments share the same app code and only differ by setup headers and compile flags.
- `test/` contains host-side logic tests that compile pure decoder/logic code against the repo's simulator shims rather than the full ESP32 firmware.
- `sim/` provides the desktop shims for tests that need Arduino-like behavior without pulling in the full board stack.

## Architecture and conventions

- The app is organized around board-specific compilation flags such as `CYD`, `AWOK`, `SQUACH_MESH`, and per-display setup macros.
- Most runtime code is shared across boards; features are selected via `#if`/`#ifdef` checks rather than separate forks.
- `src/main.cpp` is the main entry point; many modules implement detection, mesh networking, LoRa, UI screens, and settings.
- The code expects ESP32 constraints: memory pressure, flash layout, NimBLE configuration, and board-specific SPI pin arrangements matter.
- `test/Makefile` is the canonical desktop validation path for much of the logic layer; it intentionally excludes the full firmware to avoid dragging in TFT/NimBLE/UI dependencies.

## Build and validation workflow

Use the repo's build system and keep commands explicit.

- Prefer board-specific builds with `pio run -e <env>`.
- Do not do a bare `pio run` or `pio run -t upload` unless you intentionally want the default env set and know what it will build.
- For the common CYD targets, use explicit environments such as:
  - `cyd`
  - `cyd-fast`
  - `cyd-ili9341`
  - `cyd-ili9341-fast`
  - `awok`
  - `freenove-s3`
  - `crowpanel7`
  - `nm-cyd-c5`
  - `twatch-s3`
- For host-side logic changes, run `make -C test` (or targeted tests if you know the affected area) before assuming a fix is valid.
- For firmware-only changes, compile the smallest relevant PlatformIO environment rather than rebuilding every board.

## Project-specific warnings

These are important repo facts that future agents must respect.

- `platformio.ini` pins `espressif32@6.5.0` and the Arduino/ESP32 toolchain combination. Do not casually upgrade it; the file explicitly documents that a silent toolchain drift broke releases previously.
- `cyd35` is intentionally frozen and not active development. The comments in `platformio.ini` explicitly say not to treat a clean build as proof of hardware validity and note a known boot-loop condition on real hardware.
- The default environment and board matrix are intentionally constrained; not every environment is safe or tested. Build only the env needed for the board or area under change.
- The ST7789 vs ILI9341 CYD display variants are the same board family with different drivers; a white screen usually means the wrong display driver selection, not an application bug.
- On CYD builds, serial monitor speed is 2,000,000 baud; the repo documents why. Do not assume plain 115200 behavior on these boards.

## Safety and scope rules

- Keep patches focused. This project has strong board-specific logic and many parallel implementation paths; do not broaden scope beyond the requested fix.
- Preserve the existing board-family layout and build flag semantics when editing `platformio.ini`.
- Prefer minimal, correct changes over large refactors.
- When touching detection logic or protocol decoders, favor the exact metrics/tests in `test/` and avoid speculative rewrites.
- Hardware, firmware, and host-side tests are not interchangeable: a host test passing is necessary but not sufficient for embedded correctness.

## Change guidance

- When adjusting a board-specific path, check the matching `include/*_user_setup.h` and the corresponding environment in `platformio.ini`.
- When editing detection or signature tables, keep the same confidence/quality conventions documented in `README.md` and `docs/DETCTIONS.md`.
- When changing mesh, LoRa, or device-signature logic, validate with the relevant `test/*.cpp` coverage rather than only compiling the firmware.
- When changing UI, alert logic, or display behavior, consider compilation across multiple relevant environments, but do not do broad matrix builds unless necessary.

## Preferred validation commands

- Desktop logic/decoder checks: `make -C test`
- Single firmware env build: `pio run -e <env>`
- Upload targeted env: `pio run -e <env> -t upload`
- Monitor a specific env: `pio device monitor -e <env>`

## Working style for agents

- Explain the reason for the chosen build/test command when reporting status.
- Note any board-specific caveats or unsupported hardware assumptions in the summary.
- When a board-specific fix depends on hardware realities, call them out explicitly rather than presenting a generic compile result as proof of correctness.

This repository’s most important rule is: if the change affects a board, a display driver, or ESP32 toolchain configuration, validate against the relevant board environment and keep the change aligned with the project’s explicit build and hardware caveats.
