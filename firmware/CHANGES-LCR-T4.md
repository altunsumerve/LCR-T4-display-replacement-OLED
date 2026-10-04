# Modifications in this derivative work

`src/` is the **Component Tester m-firmware v1.42m** by Markus Reschke (EUPL
v1.1), in the LGT8F328P port by Arnaud Durand
([DurandA/transistor-tester-lgt328p](https://github.com/DurandA/transistor-tester-lgt328p),
branch `lgt328p`, commit `85f38c0`), modified in October 2026 for an LCR-T4
clone with LGT8F328P and an SSD1306 OLED. The full diff against the DurandA
base is in `LCR-T4-changes-vs-DurandA.patch`.

## Taken from CompuKit TC1 v19.15 (EUPL v1.1)

From [compukit007/CompuKit-TC1](https://github.com/compukit007/CompuKit-TC1),
LGT-specific parts only (no TC1 user interface):

- `ADC.c`, `resistor.c`: `ADC_DIV` (12-bit ADC full scale 4096)
- `cap.c`, `semi.c`: `ADC_RAW10` (12-bit reading scaled to 10 bit)
- `pause.c`: busy-wait `MilliSleep()` instead of Timer2 sleep
- `common.h`: packed `Adjust_Type`
- `config.h`: ADC clock fixed at 125 kHz for LGT, `ADC_DIV`/`ADC_RAW10` defaults
- `config_lgt8f328p.h`: `LGT_PMCR`, `LGT_ECCR`, `ADC_DIV`, `ADC_RAW10`,
  `LGT_IVREF`, `LGT_VCAL`, `LGT_VCAL1`
- `main.c`: watchdog disable in `.init3` (naked), emulated EEPROM enable
  (`ECCR`), switch to the external 16 MHz crystal (`PMCR`, `CLKPR`),
  500 ms cold-boot delay, `Cfg.Bandgap = 1024 mV + offset` from `VCAL1`

## New in this project

- `config_lgt8f328p.h`: display = SSD1306 128×64 over bit-bang I²C,
  SDA = PD1, SCL = PD2, address 0x3C, no reset pin, X/Y flipped, 8×8 font
- `config_lgt8f328p.h` + `main.c`: **AC0 reference fix**. The internal DAC is
  enabled with the internal 1.024 V reference at full scale, because
  `C0BG` (= `ACBG`) selects the DAC output on the LGT8F328P, not a bandgap.
  Without it, small capacitors read ~150 pF.
- `config.h`: `DATA_FLASH` (texts in flash, flash-only image), `UI_AUTOHOLD`
  (result stays until key press), `POWER_OFF_TIMEOUT 60`

## Not changed / not tested

Transistor, diode, inductor, ESR accuracy and battery voltage reading were
**not** checked systematically. Resistors and capacitors (100 nF, 1 µF) were.
