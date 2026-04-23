#include "actuator_link.h"

ActuatorLink::ActuatorLink(const AppConfig& config, HardwareSerial& serial)
    : config_(config), serial_(serial) {}

void ActuatorLink::begin() {
  serial_.begin(config_.actuatorSerialBaud);
}

void ActuatorLink::update(uint32_t nowMs) {
  if (!launcherRunning_) {
    return;
  }

  if ((nowMs - lastLauncherKeepAliveMs_) >= config_.actuatorKeepAliveMs) {
    sendLine(F("LAUNCH_ON"));
    lastLauncherKeepAliveMs_ = nowMs;
  }
}

void ActuatorLink::flipLeft() { sendLine(F("FLIP_L")); }

void ActuatorLink::flipRight() { sendLine(F("FLIP_R")); }

void ActuatorLink::pushBall() { sendLine(F("PUSH")); }

void ActuatorLink::setLauncherRunning(bool enabled) {
  if (enabled == launcherRunning_) {
    return;
  }

  launcherRunning_ = enabled;
  if (launcherRunning_) {
    sendLine(F("LAUNCH_ON"));
    lastLauncherKeepAliveMs_ = millis();
  } else {
    sendLine(F("LAUNCH_OFF"));
  }
}

void ActuatorLink::stopAll() {
  launcherRunning_ = false;
  sendLine(F("STOP_ALL"));
}

void ActuatorLink::sendLine(const __FlashStringHelper* command) {
  serial_.println(command);
}
