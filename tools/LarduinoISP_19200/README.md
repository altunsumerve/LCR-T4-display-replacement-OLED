# LarduinoISP_19200 — Arduino Uno as LGT8F328P programmer

This is the stock **LarduinoISP** sketch from the
[lgt8fx core](https://github.com/dbuezas/lgt8fx) with **one change**: the
serial speed is 19200 instead of 115200. The files are not stored here.
Run `python tools/setup_lgt_tools.py` to copy and patch them from your own
Arduino installation.

## Why 19200

When avrdude opens the port, the Uno resets and its own bootloader
(Optiboot, 115200 baud) answers first. At 115200 avrdude then talks to the
**Uno's bootloader** instead of LarduinoISP. In the log you see
`HW Version 3 / FW Version 4.4` and `Device signature = 00 00 00`. At 19200 the
bootloader can't understand avrdude, times out, and LarduinoISP answers.

## RX buffer must be 250 bytes

LarduinoISP refuses to compile unless `SERIAL_RX_BUFFER_SIZE >= 250`. The
`#error` only checks the **sketch**. If the Arduino **core** was built and
cached earlier with the default 64 bytes, the sketch compiles fine but the
core still has a 64-byte buffer. The programmer then hangs on the first flash
page ("programmer is not responding" at 0 %). While the target is being erased,
the 128-byte page arrives and overflows the small buffer.

Build it **clean**, with the define for both C and C++ files:

```
arduino-cli compile --clean -b arduino:avr:uno ^
  --build-property "compiler.cpp.extra_flags=-DSERIAL_RX_BUFFER_SIZE=250" ^
  --build-property "compiler.c.extra_flags=-DSERIAL_RX_BUFFER_SIZE=250" ^
  --build-path <empty folder> --output-dir build LarduinoISP_19200
arduino-cli upload -b arduino:avr:uno -p COM3 --input-dir build LarduinoISP_19200
```

Check: the reported global variables must grow by about 186 bytes
(472 → 658 bytes in our build). If they don't, the core still has 64 bytes.

In the Arduino IDE you can instead select an lgt8fx board's menu
*Arduino as ISP → 250* only when the **programmer** itself is an LGT board.
For a Uno, use the command line above.

## Wiring

| Uno | LGT8F328P |
|---|---|
| D10 | RESET |
| D12 | SWD |
| D13 | SWC |
| 5V | VCC |
| GND | GND |

## Warning: connecting erases a locked chip

With a locked chip (SWD ID `3E A2 50 E9`), LarduinoISP runs a **full chip erase**
in `start_pmode()`, i.e. as soon as avrdude connects. That's also true for read
operations like `-U flash:r:...`. The signature it reports is hard-coded and
proves nothing. To check a chip without destroying anything, use
`tools/LGT_ID_READ` first.
