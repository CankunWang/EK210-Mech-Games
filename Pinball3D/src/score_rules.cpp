#include "score_rules.h"

ScoreRules::ScoreRules(const AppConfig& config) : config_(config) {}

void ScoreRules::reset() {
  state_.score = 0;
  state_.combo = 0;
  lastHitMs_ = 0;
  lastScoreMs_ = 0;
}

void ScoreRules::onSensorHit(uint8_t sensorIndex, uint32_t nowMs) {
  if (lastScoreMs_ > 0 &&
      (nowMs - lastScoreMs_) < config_.tcrtScoreCooldownMs) {
    return;
  }

  if (lastHitMs_ > 0 && (nowMs - lastHitMs_) <= config_.comboWindowMs) {
    if (state_.combo < 9) {
      state_.combo += 1;
    }
  } else {
    state_.combo = 1;
  }

  lastHitMs_ = nowMs;
  lastScoreMs_ = nowMs;
  uint16_t baseScore = 0;
  if (sensorIndex < kSensorCount) {
    baseScore = config_.tcrtScoreValue[sensorIndex];
  }
  state_.score += (static_cast<uint32_t>(baseScore) * state_.combo);
}

void ScoreRules::onBallLost() {
  state_.combo = 0;
}

const ScoreState& ScoreRules::state() const { return state_; }
