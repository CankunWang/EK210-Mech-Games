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
  bool readButtonActive(uint8_t buttonIndex) const;
  bool readLimitActive(uint8_t limitIndex) const;
  bool readBreakBeamActive() const;
  uint16_t readUltrasonicDistanceCm() const;
  bool detectUltrasonicChange(uint16_t distanceCm);
  uint16_t readTcrtRaw(uint8_t sensorIndex) const;
  bool readTcrtDetected(uint8_t sensorIndex, uint16_t rawValue) const;

  const AppConfig& config_;
  InputSnapshot snapshot_;

  bool rawButtonDown_[kButtonCount] = {false, false, false, false};
  bool stableButtonDown_[kButtonCount] = {false, false, false, false};
  uint32_t lastRawButtonChangeMs_[kButtonCount] = {0, 0, 0, 0};

  bool previousBreakBeamActive_ = false;
  bool previousUltrasonicActive_ = false;
  uint16_t previousUltrasonicDistanceCm_ = 0;
  bool hasPreviousUltrasonicDistance_ = false;
  uint32_t lastUltrasonicSampleMs_ = 0;
  bool previousTcrtDetected_[kSensorCount] = {false, false, false};
  uint32_t lastTcrtEventMs_[kSensorCount] = {0, 0, 0};
};
