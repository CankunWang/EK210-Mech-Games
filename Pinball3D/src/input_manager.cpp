#include "input_manager.h"

InputManager::InputManager(const AppConfig& config) : config_(config) {}

void InputManager::begin() {
  for (uint8_t i = 0; i < kButtonCount; ++i) {
    pinMode(config_.buttonPins[i],
            config_.useInternalPullupsForButtons ? INPUT_PULLUP : INPUT);
  }

  for (uint8_t i = 0; i < kLimitCount; ++i) {
    if (config_.limitPins[i] == kUnassignedPin) {
      continue;
    }
    pinMode(config_.limitPins[i],
            config_.useInternalPullupsForLimits ? INPUT_PULLUP : INPUT);
  }

  if (config_.breakBeamPin != kUnassignedPin) {
    pinMode(config_.breakBeamPin,
            config_.useInternalPullupForBreakBeam ? INPUT_PULLUP : INPUT);
  }

  if (config_.ultrasonic.trigPin != kUnassignedPin &&
      config_.ultrasonic.echoPin != kUnassignedPin) {
    pinMode(config_.ultrasonic.trigPin, OUTPUT);
    digitalWrite(config_.ultrasonic.trigPin, LOW);
    pinMode(config_.ultrasonic.echoPin, INPUT);
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
  for (uint8_t i = 0; i < kButtonCount; ++i) {
    snapshot_.buttonPressedEdge[i] = false;
    snapshot_.buttonReleasedEdge[i] = false;

    const bool rawDown = readButtonActive(i);
    if (rawDown != rawButtonDown_[i]) {
      rawButtonDown_[i] = rawDown;
      lastRawButtonChangeMs_[i] = nowMs;
    }

    if ((nowMs - lastRawButtonChangeMs_[i]) >= config_.buttonDebounceMs &&
        rawButtonDown_[i] != stableButtonDown_[i]) {
      stableButtonDown_[i] = rawButtonDown_[i];
      snapshot_.buttonPressedEdge[i] = stableButtonDown_[i];
      snapshot_.buttonReleasedEdge[i] = !stableButtonDown_[i];
    }

    snapshot_.buttonDown[i] = stableButtonDown_[i];
  }

  snapshot_.breakBeamEdge = false;
  snapshot_.breakBeamActive = readBreakBeamActive();
  if (snapshot_.breakBeamActive && !previousBreakBeamActive_) {
    snapshot_.breakBeamEdge = true;
  }
  previousBreakBeamActive_ = snapshot_.breakBeamActive;

  snapshot_.ultrasonicEdge = false;
  if ((config_.ultrasonic.trigPin != kUnassignedPin) &&
      (config_.ultrasonic.echoPin != kUnassignedPin) &&
      ((nowMs - lastUltrasonicSampleMs_) >= config_.ultrasonic.sampleIntervalMs ||
       lastUltrasonicSampleMs_ == 0)) {
    snapshot_.ultrasonicDistanceCm = readUltrasonicDistanceCm();
    lastUltrasonicSampleMs_ = nowMs;
  }
  snapshot_.ultrasonicActive =
      detectUltrasonicChange(snapshot_.ultrasonicDistanceCm);
  if (snapshot_.ultrasonicActive && !previousUltrasonicActive_) {
    snapshot_.ultrasonicEdge = true;
  }
  previousUltrasonicActive_ = snapshot_.ultrasonicActive;

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

bool InputManager::readButtonActive(uint8_t buttonIndex) const {
  const bool pinHigh = digitalRead(config_.buttonPins[buttonIndex]) == HIGH;
  return config_.buttonActiveLow ? !pinHigh : pinHigh;
}

bool InputManager::readLimitActive(uint8_t limitIndex) const {
  const uint8_t pin = config_.limitPins[limitIndex];
  if (pin == kUnassignedPin) {
    return false;
  }
  const bool pinHigh = digitalRead(pin) == HIGH;
  return config_.limitActiveLow ? !pinHigh : pinHigh;
}

bool InputManager::readBreakBeamActive() const {
  if (config_.breakBeamPin == kUnassignedPin) {
    return false;
  }
  const bool pinHigh = digitalRead(config_.breakBeamPin) == HIGH;
  return config_.breakBeamActiveLow ? !pinHigh : pinHigh;
}

uint16_t InputManager::readUltrasonicDistanceCm() const {
  if (config_.ultrasonic.trigPin == kUnassignedPin ||
      config_.ultrasonic.echoPin == kUnassignedPin) {
    return 0;
  }

  digitalWrite(config_.ultrasonic.trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(config_.ultrasonic.trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(config_.ultrasonic.trigPin, LOW);

  const unsigned long durationUs = pulseIn(
      config_.ultrasonic.echoPin, HIGH, config_.ultrasonic.echoTimeoutUs);
  if (durationUs == 0) {
    return 0;
  }

  return static_cast<uint16_t>(durationUs / 58UL);
}

bool InputManager::detectUltrasonicChange(uint16_t distanceCm) {
  if (config_.ultrasonic.trigPin == kUnassignedPin ||
      config_.ultrasonic.echoPin == kUnassignedPin) {
    return false;
  }

  if (distanceCm == 0) {
    return false;
  }

  if (!hasPreviousUltrasonicDistance_) {
    previousUltrasonicDistanceCm_ = distanceCm;
    hasPreviousUltrasonicDistance_ = true;
    return false;
  }

  const uint16_t previous = previousUltrasonicDistanceCm_;
  previousUltrasonicDistanceCm_ = distanceCm;

  const uint16_t deltaCm =
      (distanceCm > previous) ? (distanceCm - previous) : (previous - distanceCm);
  const uint16_t deltaMm = deltaCm * 10;
  return deltaMm >= config_.ultrasonic.detectDeltaMm;
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
