#pragma once

#include <Arduino.h>

#include "domain_types.h"
#include "pins_config.h"

class MotorDriver {
 public:
  explicit MotorDriver(const AppConfig& config);

  void begin();
  void setMotor(MotorId motorId, MotorCommand cmd);
  void stopAll();

 private:
  void applyCommand(const MotorPins& pins, MotorCommand cmd);
  void writeLevel(uint8_t pin, uint8_t level);
  bool pinSupportsPwm(uint8_t pin) const;

  const AppConfig& config_;
  MotorCommand lastCmd_[kMotorCount];
};
