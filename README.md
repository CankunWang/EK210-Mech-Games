# EK210 Mech Games - Arduino Architecture Skeleton

This repository now contains a modular Arduino sketch scaffold for the 3D pinball project.

## File Layout

- `Pinball3D.ino`: app entrypoint (`setup/loop`)
- `src/pins_config.h`: all pin mapping and tunable constants
- `src/domain_types.h`: shared enums and data structs
- `src/input_manager.*`: buttons (A0 ladder), TCRT5000 (A1-A3), limit switches
- `src/motor_driver.*`: DRV8833 motor control wrapper (4 motors)
- `src/launcher_servo.*`: launch servo state machine
- `src/score_rules.*`: score/combo/ball counters
- `src/ui_lcd.*`: LCD render layer (with optional Serial debug)
- `src/game_fsm.*`: top-level game state machine and orchestration

## Important Pin Notes

- Config follows your latest constraints:
  - `A0` -> 4-button resistor ladder
  - `A1/A2/A3` -> 3x TCRT5000 `AO` (analog outputs)
  - LCD uses I2C pins (`SDA/SCL`)
- TCRT5000 module model in code:
  - per sensor: `VCC/GND/AO/DO` hardware model
  - `detectMode` can be `AnalogThreshold` or `DigitalPin`
  - event output is `tcrtPassEdge[]` ("ball passed")
- Default mapping in `pins_config.h` uses `D0` for one limit switch to fit all I/O.
  - If upload/serial becomes unstable, remap pins or move one input through hardware expansion.
- Motor role mapping in code:
  - `motor0/motor1`: launcher motors (hold launch button to keep running)
  - `motor2/motor3`: flipper motors (press button3 once => one swing + reset cycle)
  - each motor action is blocked by its paired limit switch

## Next Steps

1. Calibrate `buttonLadder` voltage bands.
2. Calibrate each sensor `analogThreshold` (or wire `DO` and set `detectMode=DigitalPin`).
3. Replace placeholder motor policies in `game_fsm.cpp` with your exact gameplay behavior.
4. Confirm final DRV8833 pin map against physical wiring.
