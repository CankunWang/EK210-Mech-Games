/*
  TCRT5000 AO Calibration (3 channels)
  Wiring:
  - Sensor1 AO -> A1
  - Sensor2 AO -> A2
  - Sensor3 AO -> A3
  - All sensors VCC -> 5V, GND -> GND
*/

const uint8_t kSensorPins[3] = {A1, A2, A3};
const uint32_t kPrintIntervalMs = 80;

uint32_t gLastPrintMs = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;  // For boards with native USB; harmless on Uno.
  }
  Serial.println(F("TCRT5000 calibration started."));
  Serial.println(F("Columns: millis, A1, A2, A3"));
}

void loop() {
  const uint32_t now = millis();
  if ((now - gLastPrintMs) < kPrintIntervalMs) {
    return;
  }
  gLastPrintMs = now;

  const int v1 = analogRead(kSensorPins[0]);
  const int v2 = analogRead(kSensorPins[1]);
  const int v3 = analogRead(kSensorPins[2]);

  Serial.print(now);
  Serial.print(',');
  Serial.print(v1);
  Serial.print(',');
  Serial.print(v2);
  Serial.print(',');
  Serial.println(v3);
}

