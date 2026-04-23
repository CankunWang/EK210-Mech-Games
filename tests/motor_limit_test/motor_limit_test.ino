/*
  Motor + Limit Test (DRV8833 style)
  Motor map:
  - M0: IN1=D5,  IN2=D6,  limit=D2
  - M1: IN1=D7,  IN2=D8,  limit=D3
  - M2: IN1=D9,  IN2=D10, limit=D4
  - M3: IN1=D11, IN2=D12, limit=D0

  Serial commands (115200):
  - "0f", "1f", "2f", "3f": run motor forward (blocked by its limit)
  - "0r", "1r", "2r", "3r": run motor reverse for 200 ms
  - "s": stop all motors

  Note: D0 shares serial RX on Uno. Disconnect that limit wire when uploading
  if needed.
*/

struct MotorPins {
  uint8_t in1;
  uint8_t in2;
};

const MotorPins kMotors[4] = {
    {5, 6},
    {7, 8},
    {9, 10},
    {11, 12},
};

const uint8_t kLimits[4] = {2, 3, 4, 0};
const bool kForwardIsReverse[4] = {false, false, false, true};
const uint8_t kRunSpeed = 255;

void setMotorRaw(uint8_t id, bool forward, uint8_t speed) {
  if (id > 3) {
    return;
  }
  const bool actualForward = kForwardIsReverse[id] ? !forward : forward;
  const uint8_t hi = (speed > 0) ? HIGH : LOW;

  if (actualForward) {
    digitalWrite(kMotors[id].in1, hi);
    digitalWrite(kMotors[id].in2, LOW);
  } else {
    digitalWrite(kMotors[id].in1, LOW);
    digitalWrite(kMotors[id].in2, hi);
  }
}

void stopMotor(uint8_t id) {
  if (id > 3) {
    return;
  }
  digitalWrite(kMotors[id].in1, LOW);
  digitalWrite(kMotors[id].in2, LOW);
}

void stopAll() {
  for (uint8_t i = 0; i < 4; ++i) {
    stopMotor(i);
  }
}

bool isLimitActive(uint8_t id) {
  if (id > 3) {
    return false;
  }
  return digitalRead(kLimits[id]) == LOW;  // INPUT_PULLUP active-low
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  for (uint8_t i = 0; i < 4; ++i) {
    pinMode(kMotors[i].in1, OUTPUT);
    pinMode(kMotors[i].in2, OUTPUT);
    pinMode(kLimits[i], INPUT_PULLUP);
  }
  stopAll();

  Serial.println(F("Motor + limit test ready."));
  Serial.println(F("Commands: 0f/0r ... 3f/3r, s"));
}

void loop() {
  if (Serial.available() < 1) {
    return;
  }

  const char c0 = static_cast<char>(Serial.read());
  if (c0 == 's') {
    stopAll();
    Serial.println(F("STOP ALL"));
    return;
  }

  while (Serial.available() < 1) {
    ;  // Wait for second command char.
  }
  const char c1 = static_cast<char>(Serial.read());

  if (c0 < '0' || c0 > '3') {
    Serial.println(F("Bad motor id"));
    return;
  }

  const uint8_t id = static_cast<uint8_t>(c0 - '0');

  if (c1 == 'f') {
    if (isLimitActive(id)) {
      stopMotor(id);
      Serial.print(F("M"));
      Serial.print(id);
      Serial.println(F(" blocked by limit"));
      return;
    }
    setMotorRaw(id, true, kRunSpeed);
    Serial.print(F("M"));
    Serial.print(id);
    Serial.println(F(" FORWARD"));
    return;
  }

  if (c1 == 'r') {
    setMotorRaw(id, false, kRunSpeed);
    delay(200);
    stopMotor(id);
    Serial.print(F("M"));
    Serial.print(id);
    Serial.println(F(" REVERSE PULSE"));
    return;
  }

  Serial.println(F("Bad command"));
}

