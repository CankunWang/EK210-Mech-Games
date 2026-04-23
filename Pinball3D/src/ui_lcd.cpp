#include "ui_lcd.h"

namespace {
void clearLine(LiquidCrystal_I2C& lcd, uint8_t row) {
  lcd.setCursor(0, row);
  lcd.print(F("                "));
  lcd.setCursor(0, row);
}
}

UiLcd::UiLcd(const AppConfig& config)
    : config_(config), lcd_(config.lcdAddress, config.lcdCols, config.lcdRows) {}

void UiLcd::showStart(const ScoreState& score, uint32_t nowMs) {
  clearLine(lcd_, 0);
  lcd_.print(F("S:"));
  lcd_.print(score.score);

  clearLine(lcd_, 1);
  lcd_.print(F("GAME START"));

  lastRenderMs_ = nowMs;
}

void UiLcd::begin() {
  if (config_.serialDebug) {
    Serial.begin(config_.serialBaud);
  }

  lcd_.init();
  lcd_.backlight();
  lcd_.clear();
  ScoreState bootScore;
  showStart(bootScore, 0);
}

void UiLcd::render(GameState gameState, const ScoreState& score,
                   const InputSnapshot& inputs, uint32_t nowMs) {
  if ((nowMs - lastRenderMs_) < config_.lcdRefreshMs) {
    return;
  }
  lastRenderMs_ = nowMs;

  if (gameState == GameState::Idle || gameState == GameState::Ready) {
    showStart(score, nowMs);
  } else {
    clearLine(lcd_, 0);
    lcd_.print(F("S:"));
    lcd_.print(score.score);

    clearLine(lcd_, 1);
    lcd_.print(stateToLabel(gameState));
  }

  if (config_.serialDebug) {
    uint8_t buttonMask = 0;
    for (uint8_t i = 0; i < 4; ++i) {
      if (inputs.buttonDown[i]) {
        buttonMask |= (1 << i);
      }
    }
    Serial.print(F("[UI] state="));
    Serial.print(static_cast<uint8_t>(gameState));
    Serial.print(F(" btnMask=0x"));
    Serial.print(buttonMask, HEX);
    Serial.print(F(" score="));
    Serial.print(score.score);
    Serial.print(F(" bb="));
    Serial.print(inputs.breakBeamActive);
    Serial.print(F(" us="));
    Serial.print(inputs.ultrasonicDistanceCm);
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
      return F("GAME START");
    case GameState::Ready:
      return F("GAME START");
    case GameState::Playing:
      return F("PLAYING");
    case GameState::BallLost:
      return F("GAME OVER");
    case GameState::GameOver:
      return F("GAME OVER");
  }
  return F("UNKNOWN");
}
