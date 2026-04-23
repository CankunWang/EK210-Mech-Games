# Test Sketches

This folder stores standalone Arduino test sketches for breadboard bring-up.

## Sketches

- `tcrt5000_calibration/tcrt5000_calibration.ino`
  - Reads 3x TCRT5000 AO pins (`A1/A2/A3`) and prints live values.
  - Use it to tune sensor thresholds for ball-pass detection.
- `limit_switch_test/limit_switch_test.ino`
  - Reads 4x limit switches (`D2/D3/D4/D0`) with `INPUT_PULLUP`.
  - Use it to verify each switch toggles correctly.
- `launcher_hold_test/launcher_hold_test.ino`
  - Hold one button (`D6`, `INPUT_PULLUP`) to run launcher motors `M2/M3`.
  - Release button to stop both motors.
  - Motors are commanded in opposite directions (`M2` forward, `M3` reverse).
  - `kBypassButton=true` can force old auto-run mode for A/B comparison.
- `motor_limit_test/motor_limit_test.ino`
  - Manual serial test for 4 motors (DRV8833 style IN1/IN2 control).
  - Forward motion is blocked if paired limit switch is active.
- `flipper_slow_cycle_test/flipper_slow_cycle_test.ino`
  - Auto-cycles both flippers with no buttons and no limit switches.
  - Uses current main-sketch wiring: left `D5/D6`, right `D7/D8`.
  - Swings for 2 seconds, then resets for 2 seconds at a slow speed.
- `uno_actuator_controller/uno_actuator_controller.ino`
  - Dedicated Uno sketch for split-board actuator control.
  - Receives serial commands and drives left/right flipper servos, push servo, and launcher motors.
  - Includes a launcher safety timeout.
- `mega_uno_bridge_test/mega_uno_bridge_test.ino`
  - Mega-side bridge smoke test for the split-board setup.
  - Uses `A0..A3` buttons to send `FLIP_L`, `FLIP_R`, `PUSH`, `LAUNCH_ON/OFF` over `Serial1`.
  - Wire Mega `TX1 (18)` to Uno `RX (0)`, Mega `RX1 (19)` to Uno `TX (1)`, and share ground.

## How To Use

1. Open one sketch folder in Arduino IDE.
2. Select board: Arduino Uno.
3. Upload and open Serial Monitor at `115200`.
4. Test only one subsystem at a time before full integration.
