#include "game_fsm.h"

namespace {
constexpr uint32_t kLostPauseMs = 900;
}

GameFsm::GameFsm(const AppConfig& config, InputManager& inputs, MotorDriver& motors,
                 LauncherServo& launcher, ScoreRules& scoreRules, UiLcd& ui)
    : config_(config),
      inputs_(inputs),
      motors_(motors),
      launcher_(launcher),
      scoreRules_(scoreRules),
      ui_(ui) {}

void GameFsm::begin() {
  scoreRules_.reset();
  motors_.stopAll();
  flipperPhase_ = FlipperCyclePhase::Idle;
  flipperPhaseStartMs_ = 0;
  enterState(GameState::Idle, millis());
}

void GameFsm::tick(uint32_t nowMs) {
  const InputSnapshot& inputs = inputs_.poll(nowMs);

  processGlobalControls(inputs, nowMs);
  processScoring(inputs, nowMs);
  processState(inputs, nowMs);
  applyMotorPolicy(inputs, nowMs);

  launcher_.update(nowMs);
  ui_.render(state_, scoreRules_.state(), inputs, nowMs);
}

void GameFsm::enterState(GameState newState, uint32_t nowMs) {
  state_ = newState;
  stateEnterMs_ = nowMs;
}

void GameFsm::processGlobalControls(const InputSnapshot& inputs, uint32_t nowMs) {
  if (!inputs.buttonPressedEdge) {
    return;
  }

  if (inputs.activeButton == ButtonId::Btn1) {
    scoreRules_.reset();
    enterState(GameState::Ready, nowMs);
    return;
  }

  if (inputs.activeButton == config_.launchButton) {
    launcher_.requestLaunch(nowMs);
    if (state_ == GameState::Ready || state_ == GameState::Idle) {
      enterState(GameState::Playing, nowMs);
    }
  }

  if (inputs.activeButton == config_.flipperButton) {
    startFlipperCycle(nowMs);
  }
}

void GameFsm::processState(const InputSnapshot& inputs, uint32_t nowMs) {
  switch (state_) {
    case GameState::Idle:
      return;

    case GameState::Ready:
      return;

    case GameState::Playing:
      if (inputs.tcrtPassEdge[2]) {
        scoreRules_.onBallLost();
        enterState(GameState::BallLost, nowMs);
      }
      return;

    case GameState::BallLost:
      if (scoreRules_.state().ballsLeft == 0) {
        enterState(GameState::GameOver, nowMs);
        return;
      }
      if ((nowMs - stateEnterMs_) >= kLostPauseMs) {
        enterState(GameState::Ready, nowMs);
      }
      return;

    case GameState::GameOver:
      // Btn1 (handled globally) starts a new game.
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

void GameFsm::applyMotorPolicy(const InputSnapshot& inputs, uint32_t nowMs) {
  // Default: stop all motors every cycle, then enable demanded actions.
  for (uint8_t i = 0; i < kMotorCount; ++i) {
    motors_.setMotor(static_cast<MotorId>(i), {});
  }

  if (state_ != GameState::Playing && state_ != GameState::Ready) {
    return;
  }

  // Launcher motors: hold-to-run, release-to-stop.
  if (inputs.activeButton == config_.launchButton) {
    driveWithLimit(inputs, config_.launcherMotorIds[0],
                   config_.launcherMotorIds[0], config_.launcherMotorSpeed,
                   true);
    driveWithLimit(inputs, config_.launcherMotorIds[1],
                   config_.launcherMotorIds[1], config_.launcherMotorSpeed,
                   true);
  }

  // Flippers: one button press triggers one swing + reset cycle.
  if (flipperPhase_ == FlipperCyclePhase::Swing) {
    driveWithLimit(inputs, config_.flipperMotorIds[0], config_.flipperMotorIds[0],
                   config_.flipperSwingSpeed, true);
    driveWithLimit(inputs, config_.flipperMotorIds[1], config_.flipperMotorIds[1],
                   config_.flipperSwingSpeed, true);

    const bool swingTimedOut =
        (nowMs - flipperPhaseStartMs_) >= config_.flipperSwingMaxMs;
    if (swingTimedOut || isFlipperLimitHit(inputs)) {
      flipperPhase_ = FlipperCyclePhase::Reset;
      flipperPhaseStartMs_ = nowMs;
    }
    return;
  }

  if (flipperPhase_ == FlipperCyclePhase::Reset) {
    motors_.setMotor(static_cast<MotorId>(config_.flipperMotorIds[0]),
                     makeReverseCmd(config_.flipperMotorIds[0],
                                    config_.flipperResetSpeed));
    motors_.setMotor(static_cast<MotorId>(config_.flipperMotorIds[1]),
                     makeReverseCmd(config_.flipperMotorIds[1],
                                    config_.flipperResetSpeed));

    if ((nowMs - flipperPhaseStartMs_) >= config_.flipperResetMs) {
      flipperPhase_ = FlipperCyclePhase::Idle;
      flipperPhaseStartMs_ = nowMs;
    }
  }
}

MotorCommand GameFsm::makeForwardCmd(uint8_t motorIndex, uint8_t speed) const {
  const bool inverted = config_.motorForwardIsReverse[motorIndex];
  return {inverted ? MotorDirection::Reverse : MotorDirection::Forward, speed};
}

MotorCommand GameFsm::makeReverseCmd(uint8_t motorIndex, uint8_t speed) const {
  const bool inverted = config_.motorForwardIsReverse[motorIndex];
  return {inverted ? MotorDirection::Forward : MotorDirection::Reverse, speed};
}

void GameFsm::driveWithLimit(const InputSnapshot& inputs, uint8_t motorIndex,
                             uint8_t limitIndex, uint8_t speed, bool forward) {
  if (motorIndex >= kMotorCount || limitIndex >= kLimitCount) {
    return;
  }

  if (inputs.limitActive[limitIndex]) {
    motors_.setMotor(static_cast<MotorId>(motorIndex), {});
    return;
  }

  motors_.setMotor(static_cast<MotorId>(motorIndex),
                   forward ? makeForwardCmd(motorIndex, speed)
                           : makeReverseCmd(motorIndex, speed));
}

bool GameFsm::isFlipperLimitHit(const InputSnapshot& inputs) const {
  const uint8_t l0 = config_.flipperMotorIds[0];
  const uint8_t l1 = config_.flipperMotorIds[1];
  if (l0 >= kLimitCount || l1 >= kLimitCount) {
    return false;
  }
  return inputs.limitActive[l0] || inputs.limitActive[l1];
}

void GameFsm::startFlipperCycle(uint32_t nowMs) {
  if (flipperPhase_ != FlipperCyclePhase::Idle) {
    return;
  }
  flipperPhase_ = FlipperCyclePhase::Swing;
  flipperPhaseStartMs_ = nowMs;
}
