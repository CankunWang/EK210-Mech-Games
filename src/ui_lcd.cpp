#include "ui_lcd.h"

UiLcd::UiLcd(const AppConfig& config)
#if PINBALL_LCD_AVAILABLE
    : config_(config), lcd_(config.lcdAddress, config.lcdCols, config.lcdRows) {}
#else
    : config_(config) {}
#endif

void UiLcd::begin() {
  if (config_.serialDebug) {
    Serial.begin(config_.serialBaud);
  }

#if PINBALL_LCD_AVAILABLE
  lcd_.init();
  lcd_.backlight();
  lcd_.clear();
  lcd_.setCursor(0, 0);
  lcd_.print(F("Pinball Booting"));
  lcd_.setCursor(0, 1);
  lcd_.print(F("A0 Btn / A1-3 IR"));
#endif
}

void UiLcd::render(GameState gameState, const ScoreState& score,
                   const InputSnapshot& inputs, uint32_t nowMs) {
  if ((nowMs - lastRenderMs_) < config_.lcdRefreshMs) {
    return;
  }
  lastRenderMs_ = nowMs;

#if PINBALL_LCD_AVAILABLE
  lcd_.setCursor(0, 0);
  lcd_.print(F("S:"));
  lcd_.print(score.score);
  lcd_.print(F(" B:"));
  lcd_.print(score.ballsLeft);
  lcd_.print(F("   "));

  lcd_.setCursor(0, 1);
  lcd_.print(stateToLabel(gameState));
  lcd_.print(F(" C:"));
  lcd_.print(score.combo);
  lcd_.print(F("     "));
#endif

  if (config_.serialDebug) {
    Serial.print(F("[UI] state="));
    Serial.print(static_cast<uint8_t>(gameState));
    Serial.print(F(" btn="));
    Serial.print(static_cast<uint8_t>(inputs.activeButton));
    Serial.print(F(" score="));
    Serial.print(score.score);
    Serial.print(F(" tcrt="));
    Serial.print(inputs.tcrtRaw[0]);
    Serial.print(',');
    Serial.print(inputs.tcrtRaw[1]);
    Serial.print(',');
    Serial.println(inputs.tcrtRaw[2]);
  }
}

const __FlashStringHelper* UiLcd::stateToLabel(GameState state) const {
  switch (state) {
    case GameState::Idle:
      return F("IDLE");
    case GameState::Ready:
      return F("READY");
    case GameState::Playing:
      return F("PLAY");
    case GameState::BallLost:
      return F("BALL LOST");
    case GameState::GameOver:
      return F("GAME OVER");
  }
  return F("UNKNOWN");
}

