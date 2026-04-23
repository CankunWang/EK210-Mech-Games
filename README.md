# EK210 Mech Games - 3D Pinball

This project uses a split-board Arduino setup:

- Arduino Mega: main controller for game logic, buttons, sensors, scoring, and LCD.
- Arduino Uno: actuator controller for flipper servos, push servo, and launcher motors.

## Required Uploads

Program both boards before running the full system.

### Arduino Mega

Upload this sketch to the Mega:

- `Pinball3D/Pinball3D.ino`

The Mega handles:

- Four buttons
- TCRT5000 score sensors
- Ultrasonic bottom detection
- LCD display
- Game state and scoring
- Serial commands to the actuator Uno

### Arduino Uno

Upload this sketch to the Uno:

- `tests/uno_actuator_controller/uno_actuator_controller.ino`

The Uno handles:

- Left flipper servo
- Right flipper servo
- Push servo
- Launcher DC motors through DRV8833

## Mega to Uno Serial Wiring

- Mega `TX1` / `D18` -> Uno `RX` / `D0`
- Mega `RX1` / `D19` -> Uno `TX` / `D1`
- Mega `GND` -> Uno `GND`

The shared ground is required for reliable serial communication.

## Current Main Pin Map

### Mega

- `A0` -> Button 1, left flipper
- `A1` -> Button 2, right flipper
- `A2` -> Button 3, push servo
- `A3` -> Button 4, launcher and game start
- `D20` -> LCD SDA
- `D21` -> LCD SCL
- `D22` -> TCRT5000 #1 DO, 200 points
- `D23` -> TCRT5000 #2 DO, 150 points
- `D24` -> TCRT5000 #3 DO, 100 points
- `D26` -> Ultrasonic TRIG
- `D27` -> Ultrasonic ECHO

### Uno Actuator Controller

- `D7` -> Left flipper servo
- `D8` -> Right flipper servo
- `D10` -> Push servo
- `D3` -> Launcher DRV8833 motor A IN1
- `D5` -> Launcher DRV8833 motor A IN2
- `D6` -> Launcher DRV8833 motor B IN1
- `D11` -> Launcher DRV8833 motor B IN2

## Notes

- The `backups/` directory is intentionally ignored by Git.
- The Mega sketch is the full game program.
- The Uno sketch is only the actuator receiver/controller.
- Uploading only one board is not enough for the full system.
