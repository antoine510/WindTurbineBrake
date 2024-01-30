enum Pins {
  PIN_VOLTAGE = A0,
  PIN_MOSFET = 9
};

constexpr const unsigned long brakeDuration_ms = 5000ul;
constexpr const int triggerVoltage_v = 15;    // !TEST!

void setup() {
  digitalWrite(PIN_MOSFET, LOW);
}

void brake() {
  auto startTime = millis();

  do {
    float brakePct = (float)(millis() - startTime) / brakeDuration_ms;
    if(brakePct > 1.f) break;
    const uint8_t duty = brakePct * 255;
    analogWrite(PIN_MOSFET, duty);
  } while(true);
  digitalWrite(PIN_MOSFET, HIGH);
}

void loop() {
  if(analogRead(PIN_VOLTAGE) > triggerVoltage_v) brake();
}
