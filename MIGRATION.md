# Migration Plan: Arduino (Main) to ESP-IDF (v2)

This document outlines the steps to migrate the JFixLib codebase from the Arduino/PlatformIO-based `main` branch to the ESP-IDF-based `v2` branch.

## 1. Migration Status

### Already Migrated / Implemented
- **Base Class:** `jFixture` (was `JFixture`)
- **App Class:** `Jonisk` (was `JJonisk`)
- **Smoothing:** `Lagger` (was `JLag`)
- **Dimming:** `Dimmer` (was `JFixtureDimmer`)
- **Motor Control:** `MotorController` (Ported with simple task-based stepping)
- **Storage:** `NVSStorage` (New, replacing EEPROM)
- **Communication:**
    - `UART` (Robust binary protocol with CRC)
    - `Parser` (Centralized ProtoBuf dispatcher, also replaces `JModes.h` one-shot flag system)
- **Updates:** `OTAUpdater` (was `JOtaServer`)
- **Hardware Drivers:**
    - `BQ25792` (Charger)
    - `I2CWrapper` (I2C abstraction)
    - `Blink` (Status LED, replaces `JWS2812B_status.h`)
- **Networking:**
    - `EspnowHandler` (New, replaces `JEspnowDevice` logic)
    - `EthernetHandler` (New, replaces `JEthernetDevice` — W5500 SPI, async UDP → Parser)
- **Math & Tables:**
    - `JWavetable` (Ported)
- **Animation/Events:**
    - `Event` (Base class ported)
    - `JEnv` (Ported)
    - `JEvent_Perlin` (Ported)
    - `JOsc` (Wavetable oscillator — ported as part of animation engine)
    - `ofNoise` (Ported)
    - `JRect` (Ported)
    - `expRandLines` (Ported)
- **Graphics/UI:**
    - `jFixtureGraphics` (Ported)
    - `jFixtureAddr` (Ported, using `led_strip` component)
- **Specialized Devices:**
    - `JLedBeam` → `led_beam.h` (header-only, WS2812B)
    - `JTlFix` → `tl_fix.h` (header-only, WS2816B)
    - `JTlFixEthernet` → `tl_fix_ethernet.h` (header-only, WS2816B + EthernetHandler)
    - `Jllll` → `jllll.h` (header-only, WS2816B + EthernetHandler + MotorController)
- **Global:**
    - `defines.h` (Ported)

### Intentionally Dropped (No v2 Equivalent Needed)
| File | Reason |
| :--- | :--- |
| `JFixLib.h` | Was just a collection of `#include` statements; replaced by CMakeLists.txt + individual includes. |
| `JModes.h` | One-shot flag/message system replaced by `Parser` ProtoBuf command dispatch. |
| `FastLedContstants.h` | FastLED-specific pin-mapping helper; FastLED removed in v2 (replaced by `led_strip`). |
| `JWS2812B_status.h` | Thin FastLED wrapper for a single status pixel; superseded by `Blink`. |

### Files Pending Migration
All files from `src/` in the `main` branch have been ported. Phase 3 is complete.

---

## 2. Migration Strategy

For each file listed above, the migration should follow these steps:

1.  **Header Refactoring:**
    - Convert `.h` to ESP-IDF style (remove `Arduino.h`, use `stdint.h`).
    - Adjust naming to match `v2` conventions if necessary.
2.  **Implementation Refactoring:**
    - Replace Arduino functions (e.g., `millis()`, `delay()`, `digitalWrite()`) with ESP-IDF equivalents (`esp_timer_get_time()`, `vTaskDelay()`, `gpio_set_level()`).
    - Integrate with the new `Parser` for command handling via ProtoBuf.
3.  **CMake Integration:**
    - Add the new `.cpp` file to `JFixLib/CMakeLists.txt`.
4.  **Verification:**
    - Update `examples/jonisk/` to test the new functionality.
    - Run `idf.py build` to ensure no regressions.

## 3. Migration To-Do Checklist

### Phase 1: Core Systems & Networking
- [x] **Addressing & ID Management (`JFixtureAddr`)**
    - [x] Port logic to read/write device ID from `NVSStorage`.
    - [x] Integrate ID check into `Parser`.
- [x] **ESP-NOW Integration (`JEspnowDevice`)**
    - [x] Implement ESP-IDF ESP-NOW initialization.
    - [x] Create receive callback that passes binary data to `Parser::processIncomingBuffer`.
    - [x] Implement send logic for peer-to-peer communication.
- [x] **Math & Tables (`JWavetable`)**
    - [x] Port wavetable generation and lookup logic.
    - [x] Ensure compatibility with `Lagger` for smooth transitions.

### Phase 2: Animation & Event Engine
- [x] **Base Event System (`Event`)**
    - [x] Refactor base `Event` class for ESP-IDF.
    - [x] Port time-tracking from `millis()` to `esp_timer`.
- [x] **Envelopes (`JEnv`)**
    - [x] Migrate ADSR/Envelope logic.
- [x] **Wavetable Oscillator (`JOsc`)**
    - [x] Port phase/frequency-driven oscillator (extends `Event`, drives pixel rendering via `JWavetable`) → `JOsc.h/cpp`.
- [x] **Noise & Patterns (`ofNoise`, `JEvent_Perlin`)**
    - [x] Port Perlin noise and random line generation logic.
- [x] **Spatial Calculations (`JRect`, `expRandLines`)**
    - [x] Port `JRect` for spatial calculations.
    - [x] Port `expRandLines`.

### Phase 3: Specialized Drivers
- [x] **Motor Control (`JMotorController`)**
    - [x] Port stepper control logic for motor drivers.
    - [x] Implement background step task.
- [x] **Ethernet (`JEthernetDevice`)**
    - [x] Implement ESP-IDF SPI Ethernet driver support (replaces Arduino `Ethernet.h`) → `ethernet_handler.h/cpp`.
    - [x] Port UDP send/receive logic (async FreeRTOS task, feeds `Parser::processIncomingBuffer`).
- [x] **Ethernet-dependent Devices (`JTlFixEthernet`)** *(was blocked on `JEthernetDevice`)*
    - [x] Port `JTlFixEthernet` (TL fixture + Ethernet) → `tl_fix_ethernet.h`.
    - [x] Port `Jllll` (LED + Ethernet + Motor combined device) → `jllll.h`.
- [x] **Device Specifics (`JLedBeam`, `JTlFix`)**
    - [x] Port high-level device wrappers → `led_beam.h`, `tl_fix.h`.

### Phase 4: UI & Graphics
- [x] **Addressable LED Framework (`JFixtureGraphics`, `JFixtureAddr`)**
    - [x] Port `jFixtureGraphics` event management logic.
    - [x] Port `jFixtureAddr` with `led_strip` component integration.

## 4. SuperCollider Library (SC/)

The SC control library has been updated from the old binary wire protocol to the
v2 ProtoBuf protocol (`simple.proto`).

### Protocol Change Summary

| Old | New |
|-----|-----|
| `0xFF×6 ++ [cmd_byte] ++ mac_addr ++ payload ++ "end"` | ProtoBuf-encoded `Command` message |
| MAC address routing | `Command.id` field (0 = broadcast, N = device N) |
| Message batching via 0x30 wrapper | Individual messages sent sequentially |

### Command Mapping

| SC Method | Old byte | Proto field | Notes |
|-----------|----------|-------------|-------|
| `setBrightness` | 0x21 | tag 2 `led` | |
| `setRGBW` | 0x20 | tag 3 `channel` ×4 | 4 channel commands (ch 0–3) |
| `setLag` | 0x22 | tag 9 `lag` | val converted seconds → ms |
| `deepSleep` | 0x10 | tag 8 `sleep` | minutes converted → ms |
| `writeID` | 0x11 | tag 5 `set_id` | |
| `blink` | _(new)_ | tag 7 `blink` | |
| `reboot` | _(new)_ | tag 12 `reboot` | |
| `setWifi` | _(new)_ | tag 4 `wifi` | |
| `setParameterBus` | 0x38 | tag 19 `set_param_bus` | |
| `setParameterBusN` | 0x39 | tag 19 ×N unicast | one cmd per device |
| `addEvent` | 0x23 | tag 13 `add_event` | |
| `doEnv` | 0x24 | tag 14 `add_env` | |
| `setVal` | 0x28 | tag 15 `set_val` | |
| `setValN` | 0x29 | tag 16 `set_val_n` | |
| `setCustomArg` | 0x31 | tag 17 `set_custom` | |
| `linkBus` | 0x37 | tag 18 `link_bus` | |
| `sync` | 0x32 | tag 11 `sync` | |
| `deleteEvents` | 0x25 | tag 10 `delete_events` | |
| `setOTAServer` | 0x15 | tag 20 `set_ota_url` | URL only; WiFi via setWifi |
| `setBackground` | 0x26 | tag 21 `set_background` | restored; applied as pixel floor in writeLeds |
| `setViewportOffset` | 0x27 | tag 22 `set_viewport_offset` | restored; shifts event coordinate origin |

### Not Supported in v2 Proto

| Feature | Reason |
|---------|--------|
| `setBackground` | No proto command; workaround: full-canvas JFixEvent_JRect |
| `setViewportOffset` | Removed from proto |
| `sendRGBWn` / `sendBrightnessN` (0x33/0x34) | No proto equivalent |
| `setAcceleration` / `setMaxSpeed` | Removed; pass `speed` in move/moveTo |
| Motor `moveN` per-device | Broadcast move only; per-device: N unicast commands |

### Device ID Convention

`JFixtureCollection.readConfigFile` now assigns IDs starting at **1** (0-indexed
position + 1). `Command.id = 0` is reserved for broadcast. Devices must have
matching `device_id` values in NVS (set via `writeID` or `tools/flash_nvs.sh`).

### New File
- `SC/JFixLib/JPb.sc` — ProtoBuf binary encoder (varint, float32, string,
  message, repeatedFloat, command builder).

## 5. Immediate Next Steps
All phases complete. Run `idf.py build` from `examples/jonisk/` to verify.
