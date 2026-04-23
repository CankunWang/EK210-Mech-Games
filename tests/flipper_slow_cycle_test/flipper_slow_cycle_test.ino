#include <Arduino.h>

// Mega wiring
// Left flipper:  D5 / D6
// Right flipper: D7 / D8

const uint8_t LEFT_IN1 = 5;
const uint8_t LEFT_IN2 = 6;
const uint8_t RIGHT_IN1 = 7;
const uint8_t RIGHT_IN2 = 8;

const uint8_t SWING_SPEED = 250;
const uint8_t RESET_SPEED = 110;

const unsigned long SWING_TIME_MS = 2000;
const unsigned long RESET_TIME_MS = 2000;
const unsigned long PAUSE_TIME_MS = 300;

void stopMotor(uint8_t in1, uint8_t in2) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

void runMotor(uint8_t in1, uint8_t in2, bool forward, uint8_t speed) {
  if (forward) {
    analogWrite(in1, speed);
    digitalWrite(in2, LOW);
  } else {
    digitalWrite(in1, LOW);
    analogWrite(in2, speed);
  }
}

void setup() {
  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);
  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);

  stopMotor(LEFT_IN1, LEFT_IN2);
  stopMotor(RIGHT_IN1, RIGHT_IN2);
}

void loop() {
  // Swing
  runMotor(LEFT_IN1, LEFT_IN2, true, SWING_SPEED);
  runMotor(RIGHT_IN1, RIGHT_IN2, false, SWING_SPEED);
  delay(SWING_TIME_MS);

  stopMotor(LEFT_IN1, LEFT_IN2);
  stopMotor(RIGHT_IN1, RIGHT_IN2);
  delay(PAUSE_TIME_MS);

  // Reset
  runMotor(LEFT_IN1, LEFT_IN2, false, RESET_SPEED);
  runMotor(RIGHT_IN1, RIGHT_IN2, true, RESET_SPEED);
  delay(RESET_TIME_MS);

  stopMotor(LEFT_IN1, LEFT_IN2);
  stopMotor(RIGHT_IN1, RIGHT_IN2);
  delay(PAUSE_TIME_MS);
}


