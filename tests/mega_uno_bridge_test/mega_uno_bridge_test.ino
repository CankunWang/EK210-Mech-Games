#include <Arduino.h>

namespace {
const uint8_t kLeftButtonPin = A0;
const uint8_t kRightButtonPin = A1;
const uint8_t kPushButtonPin = A2;
const uint8_t kLaunchButtonPin = A3;
const uint32_t kSerialBaud = 9600;
}

bool gLastLeftPressed = false;
bool gLastRightPressed = false;
bool gLastPushPressed = false;
bool gLastLaunchPressed = false;

void setup() {
  pinMode(kLeftButtonPin, INPUT_PULLUP);
  pinMode(kRightButtonPin, INPUT_PULLUP);
  pinMode(kPushButtonPin, INPUT_PULLUP);
  pinMode(kLaunchButtonPin, INPUT_PULLUP);

  Serial.begin(115200);
  Serial1.begin(kSerialBaud);
}

void loop() {
  const bool leftPressed = digitalRead(kLeftButtonPin) == LOW;
  const bool rightPressed = digitalRead(kRightButtonPin) == LOW;
  const bool pushPressed = digitalRead(kPushButtonPin) == LOW;
  const bool launchPressed = digitalRead(kLaunchButtonPin) == LOW;

  if (leftPressed && !gLastLeftPressed) {
    Serial.println(F("TX FLIP_L"));
    Serial1.println(F("FLIP_L"));
  }
  if (rightPressed && !gLastRightPressed) {
    Serial.println(F("TX FLIP_R"));
    Serial1.println(F("FLIP_R"));
  }
  if (pushPressed && !gLastPushPressed) {
    Serial.println(F("TX PUSH"));
    Serial1.println(F("PUSH"));
  }
  if (launchPressed != gLastLaunchPressed) {
    if (launchPressed) {
      Serial.println(F("TX LAUNCH_ON"));
      Serial1.println(F("LAUNCH_ON"));
    } else {
      Serial.println(F("TX LAUNCH_OFF"));
      Serial1.println(F("LAUNCH_OFF"));
    }
  }

  gLastLeftPressed = leftPressed;
  gLastRightPressed = rightPressed;
  gLastPushPressed = pushPressed;
  gLastLaunchPressed = launchPressed;

  delay(10);
}
