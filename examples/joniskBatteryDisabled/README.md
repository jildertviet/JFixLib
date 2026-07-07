# joniskBatteryDisabled

The **jonisk** fixture, built with battery support disabled. Functionally
identical to [`examples/jonisk`](../jonisk) — same firmware, same commands, same
colour/brightness behaviour — except the unit **runs only on its 12 V input and
powers off when that input is removed**, instead of falling back to the battery.

The only source difference from the jonisk example is one line in
`CMakeLists.txt`:

```cmake
set(JFIX_DISABLE_BATTERY ON CACHE BOOL "" FORCE)
```

## What the flag does

`JFIX_DISABLE_BATTERY` is a build-time behavioural flag (see
`JFixLib/CMakeLists.txt`). It does **not** remove the charger driver — the
BQ25792 stays fully alive so you still get battery/VBUS/fault monitoring in the
logs. It changes two things:

1. **No charging.** `Jonisk::init()` skips `charger->enableCharging()`, so the
   battery is never charged.

2. **Power off on 12 V loss.** The charger's 1 Hz monitor task
   (`BQ25792::update()`) watches the `VBUS_PRESENT` status bit. When the 12 V
   input disappears, it commands the external ship FET **off** by writing
   `SDRV_CTRL = Ship` (REG11 `0x11`, bits `[2:1] = 2h`). That disconnects the
   battery from the system rail, so with no input present the unit simply loses
   power and turns off.

With a normal (battery-enabled) build the ship FET stays on, so removing 12 V
would let the fixture keep running from the battery. This example prevents that.

## Timing

- The check runs once per second (the monitor-task poll interval).
- `enterShipMode()` also sets `SDRV_DLY = 1` (REG11 bit 0) in the same register
  write, which removes the BQ25792's default 10 s ship-FET turn-off delay.
- So power-off happens **within ~1 s** of the 12 V input being removed.

## Why this is safe (and why there's no gate clamp)

The ship FET (Q1, an IRLR3105) is driven by the BQ25792's dedicated `SDRV`
charge-pump output, and its **source is tied to the BQ25792 `BAT` pin**. The
gate drive is therefore referenced to the source: `Vgs = SDRV − BAT`, which the
datasheet bounds to ~5 V (abs-max `SDRV − BAT = 6 V`). So the gate–source
voltage never approaches the FET's ±20 V limit in any state — on, off, or
switching — and no external gate–source Zener clamp is required. Commanding ship
mode is a safe, supported operation.

Note: the BQ25792 only accepts the ship-mode write **when no adapter is
present**. That is exactly the condition here (we only write it once VBUS has
dropped), so the write is honoured.

## Build & flash

```sh
source /home/jildert/esp-idf/export.sh    # or wherever your ESP-IDF lives
cd examples/joniskBatteryDisabled
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

Target is `esp32s3`. On boot you should see `Battery support disabled
(JFIX_DISABLE_BATTERY)` in the log; pulling the 12 V input then powers the unit
off within about a second.

## Relation to the jonisk example

Everything except the flag is copied from `examples/jonisk`
(`sdkconfig`, `partitions.csv`, `dependencies.lock`, `src/`, and a
`managed_components` symlink). Colour/brightness still come from the
`led` / `channel` / `setBootState` commands and persist across reboot via the
NVS `boot_state` key — see the jonisk example for that side of the fixture.
