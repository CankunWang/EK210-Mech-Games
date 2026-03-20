#pragma once

#include <Arduino.h>

#include "domain_types.h"

constexpr uint8_t kSensorCount = 3;
constexpr uint8_t kMotorCount = 4;
constexpr uint8_t kLimitCount = 4;
constexpr uint8_t kButtonBands = 4;

constexpr uint8_t kUnassignedPin = 0xFF;

struct MotorPins {
  uint8_t in1 = kUnassignedPin;
  uint8_t in2 = kUnassignedPin;
};

struct ButtonBand {
  ButtonId button = ButtonId::None;
  uint16_t minValue = 0;
  uint16_t maxValue = 0;
};

enum class TcrtDetectMode : uint8_t {
  AnalogThreshold = 0,
  DigitalPin,
};

struct TcrtSensorConfig {
  // AO pin from TCRT5000 module.
  uint8_t analogPin = kUnassignedPin;
  // DO pin from TCRT5000 module (optional).
  uint8_t digitalPin = kUnassignedPin;
  TcrtDetectMode detectMode = TcrtDetectMode::AnalogThreshold;
  bool activeLow = true;
  uint16_t analogThreshold = 600;
};

struct AppConfig {
  uint8_t buttonAnalogPin = A0;
  ButtonBand buttonLadder[kButtonBands];
  uint16_t buttonDebounceMs = 25;

  // 3x TCRT5000 modules (each module has VCC/GND/AO/DO).
  TcrtSensorConfig tcrt[kSensorCount];
  uint16_t tcrtEventLockoutMs = 50;

  uint8_t limitPins[kLimitCount] = {2, 3, 4, 0};
  bool limitActiveLow = true;
  bool useInternalPullupsForLimits = true;

  MotorPins motors[kMotorCount];
  bool motorForwardIsReverse[kMotorCount] = {false, false, false, false};

  uint8_t launcherMotorIds[2] = {0, 1};
  uint8_t flipperMotorIds[2] = {2, 3};
  uint8_t launcherMotorSpeed = 255;
  uint8_t flipperSwingSpeed = 255;
  uint8_t flipperResetSpeed = 220;
  uint16_t flipperSwingMaxMs = 180;
  uint16_t flipperResetMs = 160;
  ButtonId launchButton = ButtonId::Btn4;
  ButtonId flipperButton = ButtonId::Btn3;

  uint8_t drvSleepPins[2] = {kUnassignedPin, kUnassignedPin};
  bool drvSleepActiveHigh = true;

  uint8_t servoPin = 13;
  uint8_t servoRestAngle = 10;
  uint8_t servoPushAngle = 80;
  uint16_t servoPushHoldMs = 120;
  uint16_t servoReturnHoldMs = 120;

  uint8_t lcdAddress = 0x27;
  uint8_t lcdCols = 16;
  uint8_t lcdRows = 2;
  uint16_t lcdRefreshMs = 120;

  bool serialDebug = false;
  uint32_t serialBaud = 115200;
};

inline AppConfig makeDefaultConfig() {
  AppConfig cfg;

  // A0 button ladder bands. Adjust ranges after hardware calibration.
  cfg.buttonLadder[0] = {ButtonId::Btn1, 0, 120};
  cfg.buttonLadder[1] = {ButtonId::Btn2, 180, 340};
  cfg.buttonLadder[2] = {ButtonId::Btn3, 400, 620};
  cfg.buttonLadder[3] = {ButtonId::Btn4, 680, 900};

  // Default wiring from your latest setup: AO -> A1/A2/A3.
  // DO is currently not connected; keep digitalPin as unassigned.
  cfg.tcrt[0].analogPin = A1;
  cfg.tcrt[1].analogPin = A2;
  cfg.tcrt[2].analogPin = A3;
  cfg.tcrt[0].analogThreshold = 600;
  cfg.tcrt[1].analogThreshold = 600;
  cfg.tcrt[2].analogThreshold = 600;
  cfg.tcrt[0].activeLow = true;
  cfg.tcrt[1].activeLow = true;
  cfg.tcrt[2].activeLow = true;
  // If you wire DO pins, switch one/both lines below per sensor:
  // cfg.tcrt[0].digitalPin = /* free digital pin */;
  // cfg.tcrt[0].detectMode = TcrtDetectMode::DigitalPin;

  // DRV8833: two control pins per motor.
  cfg.motors[0] = {5, 6};
  cfg.motors[1] = {7, 8};
  cfg.motors[2] = {9, 10};
  cfg.motors[3] = {11, 12};

  // Depending on mechanical layout, you may need to invert one mirrored motor.
  cfg.motorForwardIsReverse[0] = false;
  cfg.motorForwardIsReverse[1] = false;
  cfg.motorForwardIsReverse[2] = false;
  cfg.motorForwardIsReverse[3] = true;

  return cfg;
}
