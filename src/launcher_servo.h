#pragma once

#include <Arduino.h>
#include <Servo.h>

#include "pins_config.h"

class LauncherServo {
 public:
  explicit LauncherServo(const AppConfig& config);

  void begin();
  void requestLaunch(uint32_t nowMs);
  void update(uint32_t nowMs);
  bool isBusy() const;

 private:
  enum class Phase : uint8_t {
    Idle = 0,
    HoldPush,
    HoldReturn,
  };

  const AppConfig& config_;
  Servo servo_;
  Phase phase_ = Phase::Idle;
  uint32_t phaseStartMs_ = 0;
};

