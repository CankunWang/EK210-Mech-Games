#pragma once

#include <Arduino.h>

#include "actuator_link.h"
#include "domain_types.h"
#include "input_manager.h"
#include "pins_config.h"
#include "score_rules.h"
#include "ui_lcd.h"

class GameFsm {
 public:
  GameFsm(const AppConfig& config, InputManager& inputs, ActuatorLink& actuators,
          ScoreRules& scoreRules, UiLcd& ui);

  void begin();
  void tick(uint32_t nowMs);

 private:
  void enterState(GameState newState, uint32_t nowMs);
  void processGlobalControls(const InputSnapshot& inputs, uint32_t nowMs);
  void processState(const InputSnapshot& inputs, uint32_t nowMs);
  void processScoring(const InputSnapshot& inputs, uint32_t nowMs);
  void applyActuatorPolicy(const InputSnapshot& inputs, uint32_t nowMs);
  void updateActuators(uint32_t nowMs);
  void resetGame(uint32_t nowMs);

  const AppConfig& config_;
  InputManager& inputs_;
  ActuatorLink& actuators_;
  ScoreRules& scoreRules_;
  UiLcd& ui_;

  GameState state_ = GameState::Idle;
  uint32_t stateEnterMs_ = 0;
  uint32_t lastLauncherButtonDownMs_ = 0;
  bool launcherGraceArmed_ = false;
};
