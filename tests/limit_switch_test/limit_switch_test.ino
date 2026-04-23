/*
  Limit Switch Test (4 channels)
  Default mapping follows current project:
  - L0 -> D2
  - L1 -> D3
  - L2 -> D4
  - L3 -> D0

  Notes:
  - Uses INPUT_PULLUP, so ACTIVE means pin reads LOW.
  - D0 is shared with serial RX on Uno. Disconnect L3 from D0 when uploading
    if upload or serial is unstable.
*/

const uint8_t kLimitPins[4] = {2, 3, 4, 0};
const uint32_t kPrintIntervalMs = 120;
uint32_t gLastPrintMs = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  for (uint8_t i = 0; i < 4; ++i) {
    pinMode(kLimitPins[i], INPUT_PULLUP);
  }

  Serial.println(F("Limit switch test started."));
  Serial.println(F("Format: L0,L1,L2,L3 (1=ACTIVE, 0=inactive)"));
}

void loop() {
  const uint32_t now = millis();
  if ((now - gLastPrintMs) < kPrintIntervalMs) {
    return;
  }
  gLastPrintMs = now;

  for (uint8_t i = 0; i < 4; ++i) {
    const bool active = (digitalRead(kLimitPins[i]) == LOW);
    Serial.print(active ? '1' : '0');
    if (i < 3) {
      Serial.print(',');
    }
  }
  Serial.println();
}

