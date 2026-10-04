"""Build-only runner for the LCR-T4 / LGT8F328P / SSD1306 firmware in ./src.

Mirrors the Makefile of CompuKit TC1 v19.15 for TARGET=lgt8f328p, FREQ=16,
OSCILLATOR=Crystal, without the TC1 UI. Uses the AVR-GCC bundled with the
installed Arduino AVR core (avr-gcc 7.3.0 tested). Windows paths;
set AVR_GCC_BIN to the folder containing avr-gcc to override. Never touches a device.
"""
from pathlib import Path
import os, subprocess, sys

base = Path(__file__).resolve().parent
src = base / 'src'
out = base / 'build'
out.mkdir(parents=True, exist_ok=True)
if os.environ.get('AVR_GCC_BIN'):
    tc = Path(os.environ['AVR_GCC_BIN'])
else:
    tool_root = Path(os.environ['LOCALAPPDATA']) / 'Arduino15/packages/arduino/tools/avr-gcc'
    tc = sorted(tool_root.glob('*/bin/avr-gcc.exe'))[-1].parent
exe = '.exe' if os.name == 'nt' else ''

FLASH_LIMIT = 30720  # LGT8F328P: last 2 KB of flash emulate the EEPROM

cflags = ['-mmcu=atmega328', '-Wall', '-I.', '-Ibitmaps',
          '-DF_CPU=16000000UL', '-DOSC_STARTUP=16384',
          '-gdwarf-2', '-std=gnu99', '-Os', '-mcall-prologues',
          '-funsigned-char', '-funsigned-bitfields', '-fpack-struct',
          '-fshort-enums', '-mrelax', '-flto', '-D__LGT8FX8P__']
objs_c = '''main user pause adjust ADC probes display resistor cap semi inductor
tools_misc tools_signal tools_LC_Meter SPI I2C serial commands OneWire
IR_RX IR_TX DHTxx ADS7843 HD44780 ILI9163 ILI9341 ILI9481 ILI9486 ILI9488
PCD8544 PCF8814 SH1106 SSD1306 ST7036 ST7565R ST7735 ST7920 STE2007 VT100
TestDisplay'''.split()

log = []
def run(args):
    p = subprocess.run([str(a) for a in args], cwd=src, text=True,
                       capture_output=True, errors='replace')
    log.append(' '.join(map(str, args)) + '\n' + p.stdout + p.stderr)
    (out / 'build.log').write_text('\n'.join(log), encoding='utf-8')
    if p.returncode:
        print(p.stdout + p.stderr)
        sys.exit(p.returncode)
    return p.stdout + p.stderr

objects = []
for n in objs_c:
    o = out / (n + '.o')
    run([tc / ('avr-gcc' + exe), *cflags, '-c', n + '.c', '-o', o])
    objects.append(o)
o = out / 'wait.o'
run([tc / ('avr-gcc' + exe), *cflags, '-c', 'wait.S', '-o', o])
objects.append(o)

elf = out / 'ComponentTester.elf'
run([tc / ('avr-gcc' + exe), '-mmcu=atmega328', '-mrelax', '-flto',
     '-Wl,-Map=' + str(out / 'ComponentTester.map'), *objects, '-o', elf])
run([tc / ('avr-objcopy' + exe), '-O', 'ihex', '-R', '.eeprom', '-R', '.fuse',
     '-R', '.lock', '-R', '.signature', elf, out / 'ComponentTester.hex'])
size = run([tc / ('avr-size' + exe), elf])
print(size)
text, data = (int(x) for x in size.splitlines()[1].split()[:2])
flash = text + data
print(f'flash used: {flash} / {FLASH_LIMIT}')
if flash > FLASH_LIMIT:
    print('TOO BIG: would overwrite the emulated EEPROM area')
    sys.exit(2)
print('BUILD ONLY: no device was connected, erased or flashed.')
