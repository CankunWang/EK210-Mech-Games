#include "motor_driver.h"

MotorDriver::MotorDriver(const AppConfig& config) : config_(config) {}

void MotorDriver::begin() {
  for (uint8_t i = 0; i < kMotorCount; ++i) {
    const MotorPins& p = config_.motors[i];
    if (p.in1 != kUnassignedPin) {
      pinMode(p.in1, OUTPUT);
      digitalWrite(p.in1, LOW);
    }
    if (p.in2 != kUnassignedPin) {
      pinMode(p.in2, OUTPUT);
      digitalWrite(p.in2, LOW);
    }
    lastCmd_[i] = {};
  }

  for (uint8_t i = 0; i < 2; ++i) {
    if (config_.drvSleepPins[i] == kUnassignedPin) {
      continue;
    }
    pinMode(config_.drvSleepPins[i], OUTPUT);
    digitalWrite(config_.drvSleepPins[i],
                 config_.drvSleepActiveHigh ? HIGH : LOW);
  }
}

void MotorDriver::setMotor(MotorId motorId, MotorCommand cmd) {
  const uint8_t idx = static_cast<uint8_t>(motorId);
  if (idx >= kMotorCount) {
    return;
  }
  applyCommand(config_.motors[idx], cmd);
  lastCmd_[idx] = cmd;
}

void MotorDriver::stopAll() {
  for (uint8_t i = 0; i < kMotorCount; ++i) {
    setMotor(static_cast<MotorId>(i), {});
  }
}

void MotorDriver::applyCommand(const MotorPins& pins, MotorCommand cmd) {
  if (pins.in1 == kUnassignedPin || pins.in2 == kUnassignedPin) {
    return;
  }

  switch (cmd.direction) {
    case MotorDirection::Stop:
      writeLevel(pins.in1, 0);
      writeLevel(pins.in2, 0);
      break;
    case MotorDirection::Forward:
      writeLevel(pins.in1, cmd.speed);
      writeLevel(pins.in2, 0);
      break;
    case MotorDirection::Reverse:
      writeLevel(pins.in1, 0);
      writeLevel(pins.in2, cmd.speed);
      break;
    case MotorDirection::Brake:
      writeLevel(pins.in1, 255);
      writeLevel(pins.in2, 255);
      break;
  }
}

void MotorDriver::writeLevel(uint8_t pin, uint8_t level) {
  if (level == 0) {
    digitalWrite(pin, LOW);
    return;
  }

  if (level >= 255 || !pinSupportsPwm(pin)) {
    digitalWrite(pin, HIGH);
    return;
  }

  analogWrite(pin, level);
}

bool MotorDriver::pinSupportsPwm(uint8_t pin) const {
#if defined(digitalPinHasPWM)
  return digitalPinHasPWM(pin);
#else
  switch (pin) {
    case 3:
    case 5:
    case 6:
    case 9:
    case 10:
    case 11:
      return true;
    default:
      return false;
  }
#endif
}
