// LGT8F328P SWD ID reader — runs on an Arduino Uno, WRITES NOTHING to the target.
//
// Why: LarduinoISP (lgt8fx core) performs a full CHIP ERASE as soon as avrdude
// enters programming mode on a locked chip (SWD ID 0x3E). Even a "read" attempt
// with avrdude destroys the factory firmware. This sketch only reads the SWD ID,
// so you can tell (a) whether the MCU is really an LGT8F328P and (b) whether it
// is locked, before deciding anything.
//
// No unlock, no erase, no SWDEN and no SWD_exit (which writes a flash-lock
// command) are called.
//
// Wiring (fixed by swd_lgt8fx8p.h): D13 = SWC, D12 = SWD, D10 = RESET, 5V, GND.
// swd_lgt8fx8p.cpp/.h are NOT in this repo: run tools/setup_lgt_tools.py to
// copy them from your installed lgt8fx Arduino core (LarduinoISP example).
//
// Serial monitor: 115200 baud. Send 'r' to read again.
#include "swd_lgt8fx8p.h"

void SWD_ReadSWDID(char *pdata);  // defined in swd_lgt8fx8p.cpp, not declared in the header

static void printHex(uint8_t v) {
  if (v < 0x10) Serial.print('0');
  Serial.print(v, HEX);
}

static void readOnce() {
  char id[4] = {0, 0, 0, 0};

  digitalWrite(10, HIGH);
  pinMode(10, OUTPUT);
  delay(20);
  digitalWrite(10, LOW);          // hold the target in reset

  SWD_init();
  SWD_Idle(10);
  SWD_ReadSWDID(id);

  SWDIF_DIR &= ~(SWDIF_CLK | SWDIF_DAT);  // release SWC/SWD
  SWDIF_PORT &= ~(SWDIF_CLK | SWDIF_DAT);
  pinMode(10, INPUT);                       // release reset

  Serial.print(F("SWDID: "));
  for (uint8_t i = 0; i < 4; i++) { printHex((uint8_t)id[i]); Serial.print(' '); }
  uint8_t a = (uint8_t)id[0];
  if ((a == 0x3E || a == 0x3F) && (uint8_t)id[1] == 0xA2 && (uint8_t)id[2] == 0x50 && (uint8_t)id[3] == 0xE9) {
    Serial.println(a == 0x3E ? F("-> LGT8F328P, LOCKED (reading it requires a chip erase)")
                             : F("-> LGT8F328P, already unlocked (readable)"));
  } else {
    Serial.println(F("-> no LGT answer (SWC/SWD swapped? not an LGT chip?)"));
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println(F("LGT SWD ID reader. Read-only, writes nothing to the target."));
  readOnce();
}

void loop() {
  if (Serial.available()) {
    if (Serial.read() == 'r') readOnce();
  }
}
