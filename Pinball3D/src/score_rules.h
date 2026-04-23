#pragma once

#include <Arduino.h>

#include "domain_types.h"
#include "pins_config.h"

class ScoreRules {
 public:
  explicit ScoreRules(const AppConfig& config);

  void reset();
  void onSensorHit(uint8_t sensorIndex, uint32_t nowMs);
  void onBallLost();

  const ScoreState& state() const;

 private:
  const AppConfig& config_;
  ScoreState state_;
  uint32_t lastHitMs_ = 0;
  uint32_t lastScoreMs_ = 0;
};
