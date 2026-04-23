#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "domain_types.h"
#include "pins_config.h"

class UiLcd {
 public:
  explicit UiLcd(const AppConfig& config);

  void begin();
  void showStart(const ScoreState& score, uint32_t nowMs);
  void render(GameState gameState, const ScoreState& score,
              const InputSnapshot& inputs, uint32_t nowMs);

 private:
  const __FlashStringHelper* stateToLabel(GameState state) const;

  const AppConfig& config_;
  uint32_t lastRenderMs_ = 0;
  LiquidCrystal_I2C lcd_;
};
