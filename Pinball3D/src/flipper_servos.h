#pragma once

#include <Arduino.h>
#include <Servo.h>

#include "pins_config.h"

class FlipperServos {
 public:
  explicit FlipperServos(const AppConfig& config);

  void begin();
  void update(uint32_t nowMs);
  void requestFlip(uint8_t flipperIndex, uint32_t nowMs);
  void resetToRest();

 private:
  enum class Phase : uint8_t {
    Idle = 0,
    HoldStrike,
    HoldReturn,
  };

  static constexpr uint8_t kFlipperCount = 2;

  void writeRest(uint8_t flipperIndex);
  void writeStrike(uint8_t flipperIndex);

  const AppConfig& config_;
  Servo servos_[kFlipperCount];
  Phase phases_[kFlipperCount] = {Phase::Idle, Phase::Idle};
  uint32_t phaseStartMs_[kFlipperCount] = {0, 0};
};
