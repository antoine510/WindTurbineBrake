enum Pins {
  PIN_VOLTAGE = A0, // PC0
  PIN_MOSFET = 9    // PB1
};

constexpr const uint16_t brakeStepDuration_ms = 4ul; // 256 steps
constexpr const uint16_t triggerVoltage_mv = 45000;    // !TEST!

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
  pinMode(PIN_MOSFET, OUTPUT);
  digitalWrite(PIN_MOSFET, LOW);
  TCCR1A = 0x01;
  TCCR1B = 0x0b;
}

bool breaking = false;
void brake() {
  auto endTime = millis() + (brakeStepDuration_ms * 255);
  breaking = true;

  OCR1A = 0;
  TCCR1A = 0xc1;  // Enable inverted PWM output 
  do {
    auto now = millis();
    if(now > endTime) break;
    uint8_t duty = (endTime - now) / brakeStepDuration_ms;

    OCR1A = duty;
  } while(true);
  PORTB |= 0x02;  // Constant high on pin_mosfet
  TCCR1A = 0x01;
}

void loop() {
  if(!breaking && getTurbineVoltage_mv() > triggerVoltage_mv) brake();
}
