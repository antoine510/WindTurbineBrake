enum Pins {
  PIN_VOLTAGE = A0,
  PIN_MOSFET = 9
};

constexpr const unsigned long brakeDuration_ms = 5000ul;
constexpr const uint16_t triggerVoltage_mv = 12000;    // !TEST!
constexpr const uint16_t mosfetThreshold_mv = 4000;

uint16_t getTurbineVoltage_mv() {
  return (uint16_t)analogRead(PIN_VOLTAGE) * 54;
}

uint16_t getVCC_mv() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return (uint16_t)result;
}

void setup() {
  digitalWrite(PIN_MOSFET, LOW);
}

bool breaking = false;
void brake() {
  auto startTime = millis();
  breaking = true;

  do {
    float brakePct = (float)(millis() - startTime) / brakeDuration_ms;
    if(brakePct > 1.f) break;
    const uint8_t duty = brakePct * 255;
    analogWrite(PIN_MOSFET, duty);
  } while(true);
  digitalWrite(PIN_MOSFET, HIGH);
}

void loop() {
  if(breaking) digitalWrite(PIN_MOSFET, getVCC_mv() > mosfetThreshold_mv);
  else if(getTurbineVoltage_mv() > triggerVoltage_mv) brake();
}
