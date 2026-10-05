# Lessons learned — the traps, in the order we hit them

Video of the whole process (Turkish): https://youtu.be/Nj_T7Vg4zOc

This repair took one long day. Most of the time went into wrong
assumptions. If your LCR-T4 behaves strangely, check these first.

## 1. "It's an ATmega328P" — it wasn't

The chip marking was sanded off. The board looks like every other LCR-T4,
so we prepared an ATmega328P firmware (8 MHz first, then 16 MHz after
reading the crystal). Continuity tests on the MCU legs then made no sense:

- only one GND leg instead of three,
- the expected LCD pins (PD3 = pin 1, PD5 = pin 9) went nowhere,
- the 5-hole header had VCC, GND, RESET and two lines that are AVCC/AREF on
  an ATmega, and AVCC didn't even beep to VCC.

**Lesson:** an erased marking + a 5-pin header + no ISP = suspect the
**LGT8F328P** and read its SWD ID before doing anything else.

## 2. Multimeter probes and plated holes

"None of the 5 holes beeps, not even to GND." The probe was pushed into
the hole and didn't touch the plating. Pressing on the copper **ring** fixed
it. The same goes for 0.5 mm-pitch LQFP legs: missing a beep isn't proof of no
connection. That's why we built the pin-finder firmware (point 6).

## 3. Reading a locked LGT erases it

LarduinoISP runs `SWD_ChipErase()` inside `start_pmode()` when the SWD ID is
`0x3E` (locked), i.e. on **any** avrdude session, including a "backup" read.
The signature it reports is hard-coded (`1E 95 0F`/`0A`) and tells you
nothing. We wrote `tools/LGT_ID_READ`, which only reads the ID.

## 4. The Uno's bootloader answers instead of LarduinoISP

At 115200 baud, avrdude reset the Uno and talked to **Optiboot**:
`HW Version 3, FW Version 4.4`, `Device signature = 00 00 00`. Running the
programmer at **19200** fixes it without the usual 10 µF RESET capacitor.

## 5. "programmer is not responding" at 0 %

Sync, enter-programming-mode and load-address all worked, then the first
`STK_PROG_PAGE` killed the programmer. LarduinoISP erases the chip right
before that page, so it isn't reading the serial port while 133 bytes arrive.
The sketch's `#error` check for `SERIAL_RX_BUFFER_SIZE >= 250` passed, but
the **cached core** still had a 64-byte buffer. A `--clean` build with the
define for C and C++ fixed it. You can see it in RAM usage: 472 → 658 bytes.

## 6. Pin finder instead of tiny legs

To find the LCD pads, a 662-byte sketch puts a different PWM duty cycle on
PD0..PD5. A multimeter on DC volts then reads 0.5 / 1.5 / 2.5 / 3.5 / 4.5 / 5 V
on the pads. That gave the full LCD mapping in one pass, and PD3/PD5 weren't
on the flex at all.

## 7. First boot: "Bat 3.26V low … Bye"

Running from the Uno's 5 V, the battery input reads low and the firmware
powers off. That's not a bug. Test on the 9 V battery.

## 8. Capacitors read 140–170 pF for 100 nF — the comparator "bandgap"

Resistors were correct, small capacitors were ~600× too small. The
m-firmware measures small caps by timing the charge up to the analog
comparator, with `ACSR = (1 << ACBG) | (1 << ACIC)` for "bandgap as positive
input". On the LGT8F328P the same bit (`C0BG`) selects the **internal 8-bit
DAC output**, which is **off after reset** (LGT8FX8P databook v1.0.5, AC0
chapter). So the comparator fired at ~0 V.

Fix (in `main.c`, LGT startup):
```c
LGT_VCAL  = LGT_VCAL1;                        // 1.024 V reference calibration
LGT_DACON = (1 << LGT_DACEN) | LGT_DAVS_IVREF; // DAC on, internal reference
LGT_DALR  = 255;                              // V_DAO = VREF*(255+1)/256 = 1.024 V
```
That matches `Cfg.Bandgap = 1024` mV used by the firmware. After the fix:
100 nF ceramic → 91 nF, 1 µF film → 905–945 nF after calibration.

This fix is **not** in the CompuKit TC1 port, which may have the same issue
with small capacitors.

## 9. Menu and power

The m-firmware user interface with one button: **double short press** = menu,
**long press** = power off. In the menu: short = next, long = select.
Self-adjustment results are **not** saved until you choose *Save*.

## 10. Flashing wipes the calibration

The LGT's EEPROM is emulated in the last 2 KB of flash. Every flash erases
it, so finish all firmware changes first and calibrate last.
