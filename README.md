# LCR-T4 display replacement — LGT8F328P firmware + 0.96" OLED

[Türkçe](README.tr.md)

*Keywords: LCR-T4 display replacement / broken LCD / screen repair, transistor tester, component tester, M328, GM328, LGT8F328P, SSD1306 OLED, m-firmware, Arduino Uno programmer, LarduinoISP, SWD.*

Repair notes, firmware and tools for a cheap **LCR-T4 component tester**
whose LCD broke. Under the sanded-off chip marking there was **no ATmega328P
but an LGT8F328P** (read-protected). This repo documents how the board was
identified, how to flash it with a plain **Arduino Uno**, and gives a
working m-firmware build that drives a **0.96" SSD1306 I²C OLED** in place of
the original LCD.

> ⚠️ Flashing **permanently erases** the factory firmware. It is locked and
> can't be read back. Only do this if the tester is useless anyway (e.g.
> broken display) or you accept losing the original.

## Does this apply to my tester?

Likely, if your LCR-T4 has:

- an MCU with **no readable marking** (LQFP-32),
- a **5-hole header** with RESET / GND / VCC and two data lines, but **no**
  MOSI/MISO/SCK,
- confusing continuity results when you assume an ATmega328P pinout.

Check with `tools/LGT_ID_READ` (read-only). The ID `3E A2 50 E9` means
LGT8F328P, locked. If you have a real ATmega328P, use the
[normal m-firmware](https://github.com/madires/Transistortester-Warehouse)
instead.

## Status

| | |
|---|---|
| OLED display | ✅ works |
| Resistors | ✅ correct values |
| Capacitors | ✅ 100 nF ceramic → 91 nF, 1 µF film → 905–945 nF (after calibration) |
| Self-adjustment + save (emulated EEPROM) | ✅ works, no checksum error after power-cycle |
| Menu, auto-hold, power off | ✅ |
| Transistors, diodes, inductors, ESR, battery reading | ⚠️ not systematically tested |

## Repository

| Path | Content |
|---|---|
| [docs/HARDWARE.md](docs/HARDWARE.md) | Board parts, **SWD header pinout**, **LCD flex pad → MCU pin** map, OLED wiring |
| [docs/FLASHING.md](docs/FLASHING.md) | Step by step: Uno as programmer, flashing, first start, self-adjustment |
| [docs/LESSONS.md](docs/LESSONS.md) | Every trap we hit, and the fixes |
| `firmware/src/` | m-firmware 1.42m (LGT port), modified, EUPL v1.1 |
| `firmware/prebuilt/*.hex` | Ready-to-flash image |
| `firmware/build.py` | Build script (avr-gcc from the Arduino AVR core) |
| `firmware/CHANGES-LCR-T4.md` | What was changed and where it came from |
| `tools/LGT_ID_READ/` | Uno sketch: reads the LGT SWD ID **without erasing** |
| `tools/LGT_PIN_FINDER/` | LGT sketch: voltage "fingerprint" on PD0..PD5 to map pads with a multimeter |
| `tools/LarduinoISP_19200/` | How to build the Uno programmer so it actually works |
| `tools/setup_lgt_tools.py` | Copies the SWD driver / LarduinoISP from your installed lgt8fx core |

## Quick facts

**SWD header** (hole 1 = square pad … 5): `1 SWC` · `2 SWD` · `3 RESET` · `4 GND` · `5 VCC`
→ Uno `D13` · `D12` · `D10` · `GND` · `5V`

**OLED** (SSD1306, I²C 0x3C): `SDA → PD1`, `SCK → PD2` (pads 3 and 5 of the
old LCD flex), `VDD → 5 V`, `GND → GND`.

**Old LCD used** PD0 (RES), PD1 (A0), PD2 (SCL), PD4 (SI). PD3/PD5 are not
connected to it.

**The key firmware fix:** on the LGT8F328P, the comparator bit `ACBG`/`C0BG`
selects the **internal DAC**, not a bandgap. The DAC must be switched on
(1.024 V internal reference, full scale), or small capacitors read as a
few hundred pF. Details in [docs/LESSONS.md](docs/LESSONS.md#8-capacitors-read-140170-pf-for-100-nf--the-comparator-bandgap).

## Programming in one paragraph

Install the lgt8fx core, run `python tools/setup_lgt_tools.py`, build
`tools/LarduinoISP_19200` **clean** with a 250-byte RX buffer and upload it to
the Uno. Wire D10/D12/D13/5V/GND to the header (battery out), then:

```
avrdude -p atmega328p -c stk500v1 -P COM3 -b 19200 -U flash:w:firmware/prebuilt/LCR-T4_LGT8F328P_SSD1306-I2C_m1.42m.hex:i
```

Then calibrate. See [docs/FLASHING.md](docs/FLASHING.md).

SHA-256 of the prebuilt image:
`e71115d16babd809903fdb7b1461a6ca836043e29b1738d8a5c4f29e30f4bc06`

## Credits

- **Component Tester m-firmware** © Markus Reschke, based on the work of
  Markus Frejek and Karl-Heinz Kübbeler — EUPL v1.1
- **LGT8F328P port** © 2021 Arnaud Durand —
  [DurandA/transistor-tester-lgt328p](https://github.com/DurandA/transistor-tester-lgt328p)
- **LGT fixes** (clock, EEPROM, 12-bit ADC, startup) from
  [CompuKit TC1 v19.15](https://github.com/compukit007/CompuKit-TC1) by Compukit
- **lgt8fx Arduino core / LarduinoISP** by dbuezas and brother_yan —
  [dbuezas/lgt8fx](https://github.com/dbuezas/lgt8fx) (not redistributed here)
- **LGT8FX8P databook** via
  [watterott/LGT8F328P-Testing](https://github.com/watterott/LGT8F328P-Testing)
- LCR-T4 repair, OLED adaptation, AC0/DAC fix, tools and docs: 2026

## License

The firmware in `firmware/src/` is a derivative work of the m-firmware and is
licensed under the **EUPL v1.1** (see `LICENSE` and
`firmware/src/EUPL-v1.1.pdf`). All original copyright notices are kept. The
documentation and tools in this repository are released under the same
license.

**No warranty.** The tester's probes are not protected. Never connect them
to a live circuit or a charged capacitor.
