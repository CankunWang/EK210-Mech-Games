#pragma once

#include <Arduino.h>

#include "domain_types.h"
#include "input_manager.h"
#include "launcher_servo.h"
#include "motor_driver.h"
#include "pins_config.h"
#include "score_rules.h"
#include "ui_lcd.h"

class GameFsm {
 public:
  GameFsm(const AppConfig& config, InputManager& inputs, MotorDriver& motors,
          LauncherServo& launcher, ScoreRules& scoreRules, UiLcd& ui);

  void begin();
  void tick(uint32_t nowMs);

 private:
  enum class FlipperCyclePhase : uint8_t {
    Idle = 0,
    Swing,
    Reset,
  };

  void enterState(GameState newState, uint32_t nowMs);
  void processGlobalControls(const InputSnapshot& inputs, uint32_t nowMs);
  void processState(const InputSnapshot& inputs, uint32_t nowMs);
  void processScoring(const InputSnapshot& inputs, uint32_t nowMs);
  void applyMotorPolicy(const InputSnapshot& inputs, uint32_t nowMs);
  MotorCommand makeForwardCmd(uint8_t motorIndex, uint8_t speed) const;
  MotorCommand makeReverseCmd(uint8_t motorIndex, uint8_t speed) const;
  void driveWithLimit(const InputSnapshot& inputs, uint8_t motorIndex,
                      uint8_t limitIndex, uint8_t speed, bool forward);
  bool isFlipperLimitHit(const InputSnapshot& inputs) const;
  void startFlipperCycle(uint32_t nowMs);

  const AppConfig& config_;
  InputManager& inputs_;
  MotorDriver& motors_;
  LauncherServo& launcher_;
  ScoreRules& scoreRules_;
  UiLcd& ui_;

  GameState state_ = GameState::Idle;
  uint32_t stateEnterMs_ = 0;
  FlipperCyclePhase flipperPhase_ = FlipperCyclePhase::Idle;
  uint32_t flipperPhaseStartMs_ = 0;
};
