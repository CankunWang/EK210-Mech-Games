#include "launcher_servo.h"

LauncherServo::LauncherServo(const AppConfig& config) : config_(config) {}

void LauncherServo::begin() {
  servo_.attach(config_.servoPin);
  servo_.write(config_.servoRestAngle);
  phase_ = Phase::Idle;
}

void LauncherServo::requestLaunch(uint32_t nowMs) {
  if (phase_ != Phase::Idle) {
    return;
  }
  servo_.write(config_.servoPushAngle);
  phase_ = Phase::HoldPush;
  phaseStartMs_ = nowMs;
}

void LauncherServo::resetToRest() {
  servo_.write(config_.servoRestAngle);
  phase_ = Phase::Idle;
  phaseStartMs_ = 0;
}

void LauncherServo::update(uint32_t nowMs) {
  switch (phase_) {
    case Phase::Idle:
      return;
    case Phase::HoldPush:
      if ((nowMs - phaseStartMs_) >= config_.servoPushHoldMs) {
        servo_.write(config_.servoRestAngle);
        phase_ = Phase::HoldReturn;
        phaseStartMs_ = nowMs;
      }
      return;
    case Phase::HoldReturn:
      if ((nowMs - phaseStartMs_) >= config_.servoReturnHoldMs) {
        phase_ = Phase::Idle;
      }
      return;
  }
}

bool LauncherServo::isBusy() const { return phase_ != Phase::Idle; }
