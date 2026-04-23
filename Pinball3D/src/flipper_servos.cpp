#include "flipper_servos.h"

FlipperServos::FlipperServos(const AppConfig& config) : config_(config) {}

void FlipperServos::begin() {
  for (uint8_t i = 0; i < kFlipperCount; ++i) {
    const uint8_t pin = config_.flipperServoPins[i];
    if (pin == kUnassignedPin) {
      continue;
    }
    servos_[i].attach(pin);
    writeRest(i);
    phases_[i] = Phase::Idle;
    phaseStartMs_[i] = 0;
  }
}

void FlipperServos::update(uint32_t nowMs) {
  for (uint8_t i = 0; i < kFlipperCount; ++i) {
    switch (phases_[i]) {
      case Phase::Idle:
        break;
      case Phase::HoldStrike:
        if ((nowMs - phaseStartMs_[i]) >= config_.flipperServoStrikeHoldMs) {
          writeRest(i);
          phases_[i] = Phase::HoldReturn;
          phaseStartMs_[i] = nowMs;
        }
        break;
      case Phase::HoldReturn:
        if ((nowMs - phaseStartMs_[i]) >= config_.flipperServoReturnHoldMs) {
          phases_[i] = Phase::Idle;
        }
        break;
    }
  }
}

void FlipperServos::requestFlip(uint8_t flipperIndex, uint32_t nowMs) {
  if (flipperIndex >= kFlipperCount) {
    return;
  }
  if (config_.flipperServoPins[flipperIndex] == kUnassignedPin) {
    return;
  }
  if (phases_[flipperIndex] != Phase::Idle) {
    return;
  }

  writeStrike(flipperIndex);
  phases_[flipperIndex] = Phase::HoldStrike;
  phaseStartMs_[flipperIndex] = nowMs;
}

void FlipperServos::resetToRest() {
  for (uint8_t i = 0; i < kFlipperCount; ++i) {
    if (config_.flipperServoPins[i] == kUnassignedPin) {
      continue;
    }
    writeRest(i);
    phases_[i] = Phase::Idle;
    phaseStartMs_[i] = 0;
  }
}

void FlipperServos::writeRest(uint8_t flipperIndex) {
  servos_[flipperIndex].write(config_.flipperServoRestAngles[flipperIndex]);
}

void FlipperServos::writeStrike(uint8_t flipperIndex) {
  servos_[flipperIndex].write(config_.flipperServoStrikeAngles[flipperIndex]);
}
