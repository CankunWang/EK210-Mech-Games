#include <Arduino.h>
#include <Servo.h>

namespace {
const uint32_t kSerialBaud = 9600;

const uint8_t kLeftFlipperPin = 7;
const uint8_t kRightFlipperPin = 8;
const uint8_t kPushServoPin = 10;

const uint8_t kLauncherAIn1 = 3;
const uint8_t kLauncherAIn2 = 5;
const uint8_t kLauncherBIn1 = 6;
const uint8_t kLauncherBIn2 = 11;

const uint8_t kLeftRest = 130;
const uint8_t kLeftStrike = 50;
const uint8_t kRightRest = 50;
const uint8_t kRightStrike = 130;
const uint8_t kPushRest = 10;
const uint8_t kPushStrike = 80;

const uint16_t kFlipStrikeHoldMs = 25;
const uint16_t kFlipReturnHoldMs = 260;
const uint16_t kPushStrikeHoldMs = 120;
const uint16_t kPushReturnHoldMs = 180;
const uint16_t kLauncherSpeed = 64;
const uint32_t kLauncherTimeoutMs = 3000;
const size_t kBufferSize = 32;
}

Servo gLeftFlipper;
Servo gRightFlipper;
Servo gPushServo;

enum class ServoPhase : uint8_t {
  Idle = 0,
  HoldStrike,
  HoldReturn,
};

ServoPhase gLeftPhase = ServoPhase::Idle;
ServoPhase gRightPhase = ServoPhase::Idle;
ServoPhase gPushPhase = ServoPhase::Idle;

uint32_t gLeftPhaseStartMs = 0;
uint32_t gRightPhaseStartMs = 0;
uint32_t gPushPhaseStartMs = 0;

bool gLauncherRunning = false;
uint32_t gLauncherLastOnMs = 0;

char gCommandBuffer[kBufferSize];
size_t gCommandLength = 0;

void writeLauncher(bool running) {
  if (running) {
    analogWrite(kLauncherAIn1, kLauncherSpeed);
    digitalWrite(kLauncherAIn2, LOW);
    digitalWrite(kLauncherBIn1, LOW);
    analogWrite(kLauncherBIn2, kLauncherSpeed);
  } else {
    digitalWrite(kLauncherAIn1, LOW);
    digitalWrite(kLauncherAIn2, LOW);
    digitalWrite(kLauncherBIn1, LOW);
    digitalWrite(kLauncherBIn2, LOW);
  }
}

void startLeftFlip(uint32_t nowMs) {
  if (gLeftPhase != ServoPhase::Idle) {
    return;
  }
  gLeftFlipper.write(kLeftStrike);
  gLeftPhase = ServoPhase::HoldStrike;
  gLeftPhaseStartMs = nowMs;
}

void startRightFlip(uint32_t nowMs) {
  if (gRightPhase != ServoPhase::Idle) {
    return;
  }
  gRightFlipper.write(kRightStrike);
  gRightPhase = ServoPhase::HoldStrike;
  gRightPhaseStartMs = nowMs;
}

void startPush(uint32_t nowMs) {
  gPushServo.write(kPushStrike);
  gPushPhase = ServoPhase::HoldStrike;
  gPushPhaseStartMs = nowMs;
}

void stopAll() {
  gLeftFlipper.write(kLeftRest);
  gRightFlipper.write(kRightRest);
  gPushServo.write(kPushRest);
  gLeftPhase = ServoPhase::Idle;
  gRightPhase = ServoPhase::Idle;
  gPushPhase = ServoPhase::Idle;
  gLauncherRunning = false;
  writeLauncher(false);
}

void processCommand(const char* cmd, uint32_t nowMs) {
  if (strcmp(cmd, "FLIP_L") == 0) {
    startLeftFlip(nowMs);
    return;
  }
  if (strcmp(cmd, "FLIP_R") == 0) {
    startRightFlip(nowMs);
    return;
  }
  if (strcmp(cmd, "PUSH") == 0) {
    startPush(nowMs);
    return;
  }
  if (strcmp(cmd, "LAUNCH_ON") == 0) {
    gLauncherRunning = true;
    gLauncherLastOnMs = nowMs;
    writeLauncher(true);
    return;
  }
  if (strcmp(cmd, "LAUNCH_OFF") == 0) {
    gLauncherRunning = false;
    writeLauncher(false);
    return;
  }
  if (strcmp(cmd, "STOP_ALL") == 0) {
    stopAll();
  }
}

void pollSerial(uint32_t nowMs) {
  while (Serial.available() > 0) {
    const char ch = static_cast<char>(Serial.read());
    if (ch == '\r') {
      continue;
    }
    if (ch == '\n') {
      gCommandBuffer[gCommandLength] = '\0';
      if (gCommandLength > 0) {
        processCommand(gCommandBuffer, nowMs);
      }
      gCommandLength = 0;
      continue;
    }
    if (gCommandLength < (kBufferSize - 1)) {
      gCommandBuffer[gCommandLength++] = ch;
    }
  }
}

void updateServos(uint32_t nowMs) {
  const bool leftStrikeDone =
      (gLeftPhase == ServoPhase::HoldStrike) &&
      ((nowMs - gLeftPhaseStartMs) >= kFlipStrikeHoldMs);
  if (leftStrikeDone) {
    gLeftFlipper.write(kLeftRest);
    gLeftPhase = ServoPhase::HoldReturn;
    gLeftPhaseStartMs = nowMs;
  }

  if (gLeftPhase == ServoPhase::HoldReturn &&
      (nowMs - gLeftPhaseStartMs) >= kFlipReturnHoldMs) {
    gLeftFlipper.write(kLeftRest);
    gLeftPhase = ServoPhase::Idle;
  } else if (gLeftPhase == ServoPhase::HoldReturn ||
             gLeftPhase == ServoPhase::Idle) {
    gLeftFlipper.write(kLeftRest);
  }

  const bool rightStrikeDone =
      (gRightPhase == ServoPhase::HoldStrike) &&
      ((nowMs - gRightPhaseStartMs) >= kFlipStrikeHoldMs);
  if (rightStrikeDone) {
    gRightFlipper.write(kRightRest);
    gRightPhase = ServoPhase::HoldReturn;
    gRightPhaseStartMs = nowMs;
  }

  if (gRightPhase == ServoPhase::HoldReturn &&
      (nowMs - gRightPhaseStartMs) >= kFlipReturnHoldMs) {
    gRightFlipper.write(kRightRest);
    gRightPhase = ServoPhase::Idle;
  } else if (gRightPhase == ServoPhase::HoldReturn ||
             gRightPhase == ServoPhase::Idle) {
    gRightFlipper.write(kRightRest);
  }

  if (gPushPhase == ServoPhase::HoldStrike &&
      (nowMs - gPushPhaseStartMs) >= kPushStrikeHoldMs) {
    gPushServo.write(kPushRest);
    gPushPhase = ServoPhase::HoldReturn;
    gPushPhaseStartMs = nowMs;
  }

  if (gPushPhase == ServoPhase::HoldReturn &&
      (nowMs - gPushPhaseStartMs) >= kPushReturnHoldMs) {
    gPushServo.write(kPushRest);
    gPushPhase = ServoPhase::Idle;
  }
}

void updateLauncherTimeout(uint32_t nowMs) {
  if (gLauncherRunning && (nowMs - gLauncherLastOnMs) > kLauncherTimeoutMs) {
    gLauncherRunning = false;
    writeLauncher(false);
  }
}

void setup() {
  Serial.begin(kSerialBaud);

  pinMode(kLauncherAIn1, OUTPUT);
  pinMode(kLauncherAIn2, OUTPUT);
  pinMode(kLauncherBIn1, OUTPUT);
  pinMode(kLauncherBIn2, OUTPUT);

  gLeftFlipper.attach(kLeftFlipperPin);
  gRightFlipper.attach(kRightFlipperPin);
  gPushServo.attach(kPushServoPin);

  stopAll();
}

void loop() {
  const uint32_t nowMs = millis();
  pollSerial(nowMs);
  updateServos(nowMs);
  updateLauncherTimeout(nowMs);
}
