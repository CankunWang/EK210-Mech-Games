#pragma once

#include <Arduino.h>

#include "domain_types.h"

class ScoreRules {
 public:
  void reset();
  void onSensorHit(uint8_t sensorIndex, uint32_t nowMs);
  void onBallLost();

  const ScoreState& state() const;

 private:
  ScoreState state_;
  uint32_t lastHitMs_ = 0;
};

