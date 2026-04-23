#pragma once

#include <Arduino.h>

#include "pins_config.h"

class ActuatorLink {
 public:
  ActuatorLink(const AppConfig& config, HardwareSerial& serial);

  void begin();
  void update(uint32_t nowMs);

  void flipLeft();
  void flipRight();
  void pushBall();
  void setLauncherRunning(bool enabled);
  void stopAll();

 private:
  void sendLine(const __FlashStringHelper* command);

  const AppConfig& config_;
  HardwareSerial& serial_;
  bool launcherRunning_ = false;
  uint32_t lastLauncherKeepAliveMs_ = 0;
};
