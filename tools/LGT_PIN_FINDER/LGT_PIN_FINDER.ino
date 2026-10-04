// LGT8F328P pin finder (LCR-T4 repair)
//
// Find which PCB pad belongs to which MCU pin WITHOUT probing the tiny LQFP legs.
// Every PORTD pin PD0..PD5 outputs a software PWM (~1 kHz) with a different duty
// cycle. A multimeter on DC volts shows the average, so each pad gets a
// "voltage fingerprint" (at 5 V supply):
//
//   PD0 ~0.5 V | PD1 ~1.5 V | PD2 ~2.5 V | PD3 ~3.5 V | PD4 ~4.5 V | PD5 ~5.0 V (steady)
//
// PD6 (power latch on the LCR-T4) and PD7 (test button) are left untouched.
//
// Build: Arduino IDE / arduino-cli with the lgt8fx core (board "LGT8F328",
// any clock), upload with the LarduinoISP_19200 programmer (see docs/FLASHING.md).
// Writing this sketch ERASES the tester firmware.
#include <avr/io.h>

void setup() {
  DDRD |= 0x3F;                       // PD0..PD5 as outputs
}

void loop() {
  static const uint8_t duty[6] = {1, 3, 5, 7, 9, 10};  // out of 10 steps
  for (uint8_t step = 0; step < 10; step++) {
    uint8_t out = 0;
    for (uint8_t p = 0; p < 6; p++)
      if (step < duty[p]) out |= (1 << p);
    PORTD = (PORTD & 0xC0) | out;
    delayMicroseconds(100);
  }
}
