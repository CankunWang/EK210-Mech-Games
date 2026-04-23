#pragma once

#include <Arduino.h>

#include "domain_types.h"

constexpr uint8_t kSensorCount = 3;
constexpr uint8_t kMotorCount = 4;
constexpr uint8_t kLimitCount = 4;
constexpr uint8_t kButtonCount = 4;

constexpr uint8_t kUnassignedPin = 0xFF;

struct MotorPins {
  uint8_t in1 = kUnassignedPin;
  uint8_t in2 = kUnassignedPin;
};

enum class TcrtDetectMode : uint8_t {
  AnalogThreshold = 0,
  DigitalPin,
};

struct TcrtSensorConfig {
  // AO pin from TCRT5000 module.
  uint8_t analogPin = kUnassignedPin;
  // DO pin from TCRT5000 module.
  uint8_t digitalPin = kUnassignedPin;
  TcrtDetectMode detectMode = TcrtDetectMode::DigitalPin;
  bool activeLow = true;
  uint16_t analogThreshold = 600;
};

struct UltrasonicConfig {
  uint8_t trigPin = kUnassignedPin;
  uint8_t echoPin = kUnassignedPin;
  uint16_t detectDeltaMm = 5;
  uint16_t sampleIntervalMs = 60;
  uint32_t echoTimeoutUs = 12000;
};

struct AppConfig {
  uint8_t buttonPins[kButtonCount] = {A0, A1, A2, A3};
  bool buttonActiveLow = true;
  bool useInternalPullupsForButtons = true;
  uint16_t buttonDebounceMs = 25;

  // 3x TCRT5000 modules (each module has VCC/GND/AO/DO).
  TcrtSensorConfig tcrt[kSensorCount];
  uint16_t tcrtEventLockoutMs = 50;

  uint8_t breakBeamPin = kUnassignedPin;
  bool breakBeamActiveLow = true;
  bool useInternalPullupForBreakBeam = true;
  UltrasonicConfig ultrasonic;

  uint8_t limitPins[kLimitCount] = {2, 3, 4, 0};
  bool limitActiveLow = true;
  bool useInternalPullupsForLimits = true;

  MotorPins motors[kMotorCount];
  bool motorForwardIsReverse[kMotorCount] = {false, false, false, false};

  uint8_t launcherMotorIds[2] = {2, 3};
  uint8_t flipperServoPins[2] = {34, 35};
  uint8_t flipperServoRestAngles[2] = {70, 110};
  uint8_t flipperServoStrikeAngles[2] = {120, 60};
  uint16_t flipperServoStrikeHoldMs = 120;
  uint16_t flipperServoReturnHoldMs = 120;
  uint8_t launcherMotorSpeed = 128;
  uint16_t launcherReleaseGraceMs = 120;
  ButtonId leftFlipperButton = ButtonId::Btn1;
  ButtonId rightFlipperButton = ButtonId::Btn2;
  ButtonId servoButton = ButtonId::Btn3;
  ButtonId launchButton = ButtonId::Btn4;

  uint8_t drvSleepPins[2] = {kUnassignedPin, kUnassignedPin};
  bool drvSleepActiveHigh = true;

  uint8_t servoPin = 31;
  uint8_t servoRestAngle = 10;
  uint8_t servoPushAngle = 80;
  uint16_t servoPushHoldMs = 120;
  uint16_t servoReturnHoldMs = 120;

  uint8_t lcdAddress = 0x27;
  uint8_t lcdCols = 16;
  uint8_t lcdRows = 2;
  uint16_t lcdRefreshMs = 120;

  // Mega <-> Uno actuator link uses Serial1 by default:
  // Mega TX1 (18) -> Uno RX (0)
  // Mega RX1 (19) -> Uno TX (1)
  uint32_t actuatorSerialBaud = 9600;
  uint16_t actuatorKeepAliveMs = 250;

  // TCRT score placeholders. Tune after sensor calibration.
  uint16_t tcrtScoreValue[kSensorCount] = {200, 150, 100};
  uint16_t tcrtScoreCooldownMs = 1000;
  uint16_t comboWindowMs = 1000;

  bool serialDebug = false;
  uint32_t serialBaud = 115200;
};

inline AppConfig makeDefaultConfig() {
  AppConfig cfg;

  cfg.buttonPins[0] = A0;
  cfg.buttonPins[1] = A1;
  cfg.buttonPins[2] = A2;
  cfg.buttonPins[3] = A3;

  cfg.limitPins[0] = kUnassignedPin;
  cfg.limitPins[1] = kUnassignedPin;
  cfg.limitPins[2] = kUnassignedPin;
  cfg.limitPins[3] = kUnassignedPin;

  // TCRT5000 modules use DO for quick digital detection on Mega.
  cfg.tcrt[0].analogPin = kUnassignedPin;
  cfg.tcrt[1].analogPin = kUnassignedPin;
  cfg.tcrt[2].analogPin = kUnassignedPin;
  cfg.tcrt[0].digitalPin = 22;
  cfg.tcrt[1].digitalPin = 23;
  cfg.tcrt[2].digitalPin = 24;
  cfg.tcrt[0].detectMode = TcrtDetectMode::DigitalPin;
  cfg.tcrt[1].detectMode = TcrtDetectMode::DigitalPin;
  cfg.tcrt[2].detectMode = TcrtDetectMode::DigitalPin;
  cfg.tcrt[0].activeLow = true;
  cfg.tcrt[1].activeLow = true;
  cfg.tcrt[2].activeLow = true;
  // If your module outputs HIGH on detection, flip activeLow to false.

  // HC-SR04 style ultrasonic sensor on Mega.
  cfg.ultrasonic.trigPin = 26;
  cfg.ultrasonic.echoPin = 27;
  cfg.ultrasonic.detectDeltaMm = 5;
  cfg.ultrasonic.sampleIntervalMs = 60;
  cfg.ultrasonic.echoTimeoutUs = 12000;

  // DRV8833: two control pins per motor.
  cfg.motors[0].in1 = kUnassignedPin;
  cfg.motors[0].in2 = kUnassignedPin;
  cfg.motors[1].in1 = kUnassignedPin;
  cfg.motors[1].in2 = kUnassignedPin;
  cfg.motors[2].in1 = 9;
  cfg.motors[2].in2 = 10;
  cfg.motors[3].in1 = 11;
  cfg.motors[3].in2 = 12;

  // Depending on mechanical layout, you may need to invert one mirrored motor.
  cfg.motorForwardIsReverse[0] = false;
  cfg.motorForwardIsReverse[1] = false;
  cfg.motorForwardIsReverse[2] = false;
  cfg.motorForwardIsReverse[3] = false;

  return cfg;
}
