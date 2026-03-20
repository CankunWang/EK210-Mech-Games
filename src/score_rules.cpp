#include "score_rules.h"

void ScoreRules::reset() {
  state_.score = 0;
  state_.ballsLeft = 3;
  state_.combo = 0;
  lastHitMs_ = 0;
}

void ScoreRules::onSensorHit(uint8_t sensorIndex, uint32_t nowMs) {
  (void)sensorIndex;

  if (lastHitMs_ > 0 && (nowMs - lastHitMs_) <= 1000) {
    if (state_.combo < 9) {
      state_.combo += 1;
    }
  } else {
    state_.combo = 1;
  }

  lastHitMs_ = nowMs;
  state_.score += (100U * state_.combo);
}

void ScoreRules::onBallLost() {
  state_.combo = 0;
  if (state_.ballsLeft > 0) {
    state_.ballsLeft -= 1;
  }
}

const ScoreState& ScoreRules::state() const { return state_; }

