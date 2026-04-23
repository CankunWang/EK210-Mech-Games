/*
  Launcher Hold Test
  Goal:
  - Hold button -> M2 + M3 rotate
  - Release button -> both motors stop
  - Easy A/B compare with old no-button mode

  Wiring assumptions:
  - Button -> D6 (INPUT_PULLUP, active LOW)
  - M2 (motor index 2): IN1=D9,  IN2=D10
  - M3 (motor index 3): IN1=D11, IN2=D12

  Notes:
  - This sketch tests launcher motor behavior only.
*/

struct MotorPins {
  uint8_t in1;
  uint8_t in2;
};

const MotorPins kM2 = {9, 10};
const MotorPins kM3 = {11, 12};
const uint8_t kButtonPin = 6;  // One side to D6, other side to GND.
const uint8_t kMotorSpeed = 150;
const bool kBypassButton = true;  // true = old behavior (always run)

void setMotor(const MotorPins& m, bool forward, uint8_t speed) {
  if (speed == 0) {
    digitalWrite(m.in1, LOW);
    digitalWrite(m.in2, LOW);
    return;
  }

  if (forward) {
    analogWrite(m.in1, speed);
    digitalWrite(m.in2, LOW);
  } else {
    digitalWrite(m.in1, LOW);
    analogWrite(m.in2, speed);
  }
}

void stopBothMotors() {
  setMotor(kM2, true, 0);
  setMotor(kM3, true, 0);
}

void runBothMotorsForward() {
  // Opposite rotation directions:
  // M2 reverse, M3 forward.
  setMotor(kM2, true, kMotorSpeed);
  setMotor(kM3, true, kMotorSpeed);
}

void setup() {
  pinMode(kButtonPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(kM2.in1, OUTPUT);
  pinMode(kM2.in2, OUTPUT);
  pinMode(kM3.in1, OUTPUT);
  pinMode(kM3.in2, OUTPUT);
  stopBothMotors();
}

void loop() {
  const bool pressed = kBypassButton ? true : (digitalRead(kButtonPin) == LOW);
  digitalWrite(LED_BUILTIN, pressed ? HIGH : LOW);
  if (pressed) {
    runBothMotorsForward();
  } else {
    stopBothMotors();
  }
}
