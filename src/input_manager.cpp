#include "input_manager.h"

InputManager::InputManager(const AppConfig& config) : config_(config) {}

void InputManager::begin() {
  for (uint8_t i = 0; i < kLimitCount; ++i) {
    if (config_.limitPins[i] == kUnassignedPin) {
      continue;
    }
    pinMode(config_.limitPins[i],
            config_.useInternalPullupsForLimits ? INPUT_PULLUP : INPUT);
  }

  for (uint8_t i = 0; i < kSensorCount; ++i) {
    const TcrtSensorConfig& s = config_.tcrt[i];
    if (s.detectMode == TcrtDetectMode::DigitalPin &&
        s.digitalPin != kUnassignedPin) {
      pinMode(s.digitalPin, INPUT);
    }
  }
}

const InputSnapshot& InputManager::poll(uint32_t nowMs) {
  snapshot_.buttonPressedEdge = false;

  snapshot_.buttonRaw = analogRead(config_.buttonAnalogPin);
  const ButtonId decoded = decodeButton(snapshot_.buttonRaw);

  if (decoded != rawButton_) {
    rawButton_ = decoded;
    lastRawButtonChangeMs_ = nowMs;
  }

  if ((nowMs - lastRawButtonChangeMs_) >= config_.buttonDebounceMs &&
      rawButton_ != stableButton_) {
    stableButton_ = rawButton_;
    if (stableButton_ != ButtonId::None) {
      snapshot_.buttonPressedEdge = true;
    }
  }

  snapshot_.activeButton = stableButton_;

  for (uint8_t i = 0; i < kSensorCount; ++i) {
    snapshot_.tcrtPassEdge[i] = false;
    snapshot_.tcrtRaw[i] = readTcrtRaw(i);
    snapshot_.tcrtDetected[i] = readTcrtDetected(i, snapshot_.tcrtRaw[i]);

    const bool rising =
        snapshot_.tcrtDetected[i] && !previousTcrtDetected_[i];
    if (rising &&
        (nowMs - lastTcrtEventMs_[i]) >= config_.tcrtEventLockoutMs) {
      snapshot_.tcrtPassEdge[i] = true;
      lastTcrtEventMs_[i] = nowMs;
    }

    previousTcrtDetected_[i] = snapshot_.tcrtDetected[i];
  }

  for (uint8_t i = 0; i < kLimitCount; ++i) {
    snapshot_.limitActive[i] = readLimitActive(i);
  }

  return snapshot_;
}

const InputSnapshot& InputManager::snapshot() const { return snapshot_; }

ButtonId InputManager::decodeButton(uint16_t analogValue) const {
  for (uint8_t i = 0; i < kButtonBands; ++i) {
    const ButtonBand& band = config_.buttonLadder[i];
    if (analogValue >= band.minValue && analogValue <= band.maxValue) {
      return band.button;
    }
  }
  return ButtonId::None;
}

bool InputManager::readLimitActive(uint8_t limitIndex) const {
  const uint8_t pin = config_.limitPins[limitIndex];
  if (pin == kUnassignedPin) {
    return false;
  }
  const bool pinHigh = digitalRead(pin) == HIGH;
  return config_.limitActiveLow ? !pinHigh : pinHigh;
}

uint16_t InputManager::readTcrtRaw(uint8_t sensorIndex) const {
  const TcrtSensorConfig& s = config_.tcrt[sensorIndex];
  if (s.analogPin == kUnassignedPin) {
    return 0;
  }
  return analogRead(s.analogPin);
}

bool InputManager::readTcrtDetected(uint8_t sensorIndex, uint16_t rawValue) const {
  const TcrtSensorConfig& s = config_.tcrt[sensorIndex];

  if (s.detectMode == TcrtDetectMode::DigitalPin &&
      s.digitalPin != kUnassignedPin) {
    const bool pinHigh = digitalRead(s.digitalPin) == HIGH;
    return s.activeLow ? !pinHigh : pinHigh;
  }

  if (s.activeLow) {
    return rawValue <= s.analogThreshold;
  }
  return rawValue >= s.analogThreshold;
}
