"""Prepare the Uno-side tools from the installed lgt8fx Arduino core.

The SWD driver (swd_lgt8fx8p.cpp/.h) and LarduinoISP.ino come from the
lgt8fx core by dbuezas (LarduinoISP by brother_yan, based on ArduinoISP).
They are not redistributed in this repo; this script copies them from your
own installation:

  tools/LGT_ID_READ/        <- swd_lgt8fx8p.cpp/.h
  tools/LarduinoISP_19200/  <- swd_lgt8fx8p.cpp/.h + LarduinoISP.ino
                               (renamed, serial speed patched 115200 -> 19200)

Install the core first (Arduino IDE Boards Manager URL):
  https://raw.githubusercontent.com/dbuezas/lgt8fx/master/package_lgt8fx_index.json
"""
from pathlib import Path
import os, shutil, sys

here = Path(__file__).resolve().parent
roots = [Path(os.environ.get('LOCALAPPDATA', '')) / 'Arduino15',
         Path.home() / '.arduino15',
         Path.home() / 'Library/Arduino15']
examples = []
for r in roots:
    examples += sorted(r.glob('packages/lgt8fx/hardware/avr/*/libraries/LarduinoISP/examples/LarduinoISP'))
if not examples:
    sys.exit('lgt8fx core not found - install it with the Boards Manager first.')
ex = examples[-1]
print('using', ex)

for dest in ('LGT_ID_READ', 'LarduinoISP_19200'):
    for f in ('swd_lgt8fx8p.cpp', 'swd_lgt8fx8p.h'):
        shutil.copy2(ex / f, here / dest / f)

ino = (ex / 'LarduinoISP.ino').read_text(encoding='utf-8', errors='replace')
old = 'Serial.begin(115200);'
if ino.count(old) != 1:
    sys.exit('unexpected LarduinoISP.ino - patch the serial speed by hand (see README.md there)')
ino = ino.replace(old, 'Serial.begin(19200);  // LCR-T4 repo: 19200, so the Uno bootloader (115200) ignores avrdude')
(here / 'LarduinoISP_19200' / 'LarduinoISP_19200.ino').write_text(ino, encoding='utf-8')
print('done')
