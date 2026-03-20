#pragma once

#include <Arduino.h>

#include "domain_types.h"
#include "pins_config.h"

class InputManager {
 public:
  explicit InputManager(const AppConfig& config);

  void begin();
  const InputSnapshot& poll(uint32_t nowMs);
  const InputSnapshot& snapshot() const;

 private:
  ButtonId decodeButton(uint16_t analogValue) const;
  bool readLimitActive(uint8_t limitIndex) const;
  uint16_t readTcrtRaw(uint8_t sensorIndex) const;
  bool readTcrtDetected(uint8_t sensorIndex, uint16_t rawValue) const;

  const AppConfig& config_;
  InputSnapshot snapshot_;

  ButtonId rawButton_ = ButtonId::None;
  ButtonId stableButton_ = ButtonId::None;
  uint32_t lastRawButtonChangeMs_ = 0;

  bool previousTcrtDetected_[kSensorCount] = {false, false, false};
  uint32_t lastTcrtEventMs_[kSensorCount] = {0, 0, 0};
};
