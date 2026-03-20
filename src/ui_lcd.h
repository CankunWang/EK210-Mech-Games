#pragma once

#include <Arduino.h>

#include "domain_types.h"
#include "pins_config.h"

#if defined(__has_include)
#if __has_include(<LiquidCrystal_I2C.h>)
#include <LiquidCrystal_I2C.h>
#define PINBALL_LCD_AVAILABLE 1
#else
#define PINBALL_LCD_AVAILABLE 0
#endif
#else
#define PINBALL_LCD_AVAILABLE 0
#endif

class UiLcd {
 public:
  explicit UiLcd(const AppConfig& config);

  void begin();
  void render(GameState gameState, const ScoreState& score,
              const InputSnapshot& inputs, uint32_t nowMs);

 private:
  const __FlashStringHelper* stateToLabel(GameState state) const;

  const AppConfig& config_;
  uint32_t lastRenderMs_ = 0;

#if PINBALL_LCD_AVAILABLE
  LiquidCrystal_I2C lcd_;
#endif
};

