#include "game_fsm.h"

namespace {
constexpr uint32_t kScoreDisplayAfterDrainMs = 5000;

uint8_t buttonIndex(ButtonId button) {
  if (button == ButtonId::None) {
    return 0;
  }
  return static_cast<uint8_t>(button) - 1;
}

bool isButtonDown(const InputSnapshot& inputs, ButtonId button) {
  return inputs.buttonDown[buttonIndex(button)];
}

bool isButtonPressed(const InputSnapshot& inputs, ButtonId button) {
  return inputs.buttonPressedEdge[buttonIndex(button)];
}
}

GameFsm::GameFsm(const AppConfig& config, InputManager& inputs,
                 ActuatorLink& actuators, ScoreRules& scoreRules, UiLcd& ui)
    : config_(config),
      inputs_(inputs),
      actuators_(actuators),
      scoreRules_(scoreRules),
      ui_(ui) {}

void GameFsm::begin() {
  resetGame(millis());
}

void GameFsm::tick(uint32_t nowMs) {
  const InputSnapshot& inputs = inputs_.poll(nowMs);

  processGlobalControls(inputs, nowMs);
  processScoring(inputs, nowMs);
  processState(inputs, nowMs);
  applyActuatorPolicy(inputs, nowMs);
  updateActuators(nowMs);
  ui_.render(state_, scoreRules_.state(), inputs, nowMs);
}

void GameFsm::enterState(GameState newState, uint32_t nowMs) {
  state_ = newState;
  stateEnterMs_ = nowMs;
}

void GameFsm::processGlobalControls(const InputSnapshot& inputs, uint32_t nowMs) {
  if (isButtonPressed(inputs, config_.servoButton)) {
    actuators_.pushBall();
  }

  if (isButtonPressed(inputs, config_.launchButton) &&
      (state_ == GameState::Idle || state_ == GameState::GameOver ||
       state_ == GameState::Ready)) {
    resetGame(nowMs);
    enterState(GameState::Playing, nowMs);
  }

  const bool controlsEnabled =
      (state_ == GameState::Ready || state_ == GameState::Playing);
  if (!controlsEnabled) {
    return;
  }

  if (isButtonPressed(inputs, config_.leftFlipperButton)) {
    actuators_.flipLeft();
  }

  if (isButtonPressed(inputs, config_.rightFlipperButton)) {
    actuators_.flipRight();
  }
}

void GameFsm::processState(const InputSnapshot& inputs, uint32_t nowMs) {
  const bool drainDetected = inputs.breakBeamEdge || inputs.ultrasonicEdge;

  switch (state_) {
    case GameState::Idle:
      return;

    case GameState::Ready:
      if (drainDetected) {
        enterState(GameState::GameOver, nowMs);
      }
      return;

    case GameState::Playing:
      if (drainDetected) {
        enterState(GameState::GameOver, nowMs);
      }
      return;

    case GameState::BallLost:
      enterState(GameState::GameOver, nowMs);
      return;

    case GameState::GameOver:
      if ((nowMs - stateEnterMs_) >= kScoreDisplayAfterDrainMs) {
        resetGame(nowMs);
      }
      return;
  }
}

void GameFsm::processScoring(const InputSnapshot& inputs, uint32_t nowMs) {
  if (state_ != GameState::Playing) {
    return;
  }

  for (uint8_t i = 0; i < kSensorCount; ++i) {
    if (inputs.tcrtPassEdge[i]) {
      scoreRules_.onSensorHit(i, nowMs);
    }
  }
}

void GameFsm::applyActuatorPolicy(const InputSnapshot& inputs, uint32_t nowMs) {
  const bool controlsEnabled =
      (state_ == GameState::Ready || state_ == GameState::Playing);
  if (controlsEnabled && isButtonDown(inputs, config_.launchButton)) {
    lastLauncherButtonDownMs_ = nowMs;
    launcherGraceArmed_ = true;
  }

  const bool withinReleaseGrace =
      launcherGraceArmed_ &&
      controlsEnabled &&
      ((nowMs - lastLauncherButtonDownMs_) <= config_.launcherReleaseGraceMs);
  const bool launcherRequested =
      controlsEnabled &&
      (isButtonDown(inputs, config_.launchButton) || withinReleaseGrace);
  actuators_.setLauncherRunning(launcherRequested);
}

void GameFsm::updateActuators(uint32_t nowMs) {
  actuators_.update(nowMs);
}

void GameFsm::resetGame(uint32_t nowMs) {
  scoreRules_.reset();
  actuators_.stopAll();
  lastLauncherButtonDownMs_ = 0;
  launcherGraceArmed_ = false;
  enterState(GameState::Idle, nowMs);
  ui_.showStart(scoreRules_.state(), nowMs);
}
