/* ************************************************************************
 *
 *   LGT8F328P specific global configuration, setup and settings
 *
 *   (c) 2021 by Arnaud Durand
 *   based on code from Markus Reschke, Markus Frejek and Karl-Heinz Kübbeler
 *
 * ************************************************************************ */


/* source management */
#define CONFIG_LGT8F328P_H



/* ************************************************************************
 *   display module
 * ************************************************************************ */


/*
 *  display module / controller
 *
 *  Please uncomment the package matching your LCD/OLED module and adjust
 *  settings. And comment out the default package if not used.
 *
 *  To uncomment, remove the enclosing "#if 0" and "#endif" or put
 *  a "//" in front of both. To comment out, remove the "//" in front
 *  of the "#if 0" and "#endif".
 *
 *  Individual settings can be enabled by removing the leading "//", or
 *  disabled by placing a "//" in front of the setting.
 */


/*
 *  LCR-T4 repair (LGT8F328P board, 2026-10-04):
 *  0.96" SSD1306 OLED module GM009605, 4 pins, I2C (addr 0x3C), no /RES pin.
 *  The broken ST7565 flex used PD0/PD1/PD2/PD4 (measured with a pin-finder
 *  firmware). OLED wiring: SDA -> PD1 (flex pad 3), SCK -> PD2 (flex pad 5),
 *  VDD -> ISP header VCC, GND -> ISP header GND. Bus pull-ups are on the OLED.
 */

#define LCD_SSD1306                     /* display controller SSD1306 */
#define LCD_GRAPHIC                     /* graphic display */
#define LCD_I2C                         /* I2C interface */
#define LCD_I2C_ADDR     0x3c           /* SSD1306's I2C address */
/* control lines */
#define LCD_PORT         PORTD          /* port data register */
#define LCD_DDR          DDRD           /* port data direction register */
/* display settings */
#define LCD_DOTS_X       128            /* number of horizontal dots */
#define LCD_DOTS_Y       64             /* number of vertical dots */
#define LCD_FLIP_X                      /* enable horizontal flip */
#define LCD_FLIP_Y                      /* enable vertical flip */
#define LCD_CONTRAST     127            /* default contrast (0-255) */
/* font and symbols: vertically aligned & flipped, bank-wise grouping */
#define FONT_8X8_VF                     /* 8x8 font */
#define SYMBOLS_24X24_VFP               /* 24x24 symbols */
/* I2C bus */
#define I2C_BITBANG                     /* bit-bang I2C */
#define I2C_PORT         PORTD          /* I2C port data register */
#define I2C_DDR          DDRD           /* I2C port data direction register */
#define I2C_PIN          PIND           /* I2C port input pins register */
#define I2C_SDA          PD1            /* port pin used for SDA */
#define I2C_SCL          PD2            /* port pin used for SCL */



/*
 *  check if a LCD module is specified
 */

#if !defined(LCD_TEXT) && !defined(LCD_GRAPHIC)
  #error <<< No LCD module specified! >>>
#endif



/* ************************************************************************
 *   port and pin assignments
 * ************************************************************************ */


/*
 *  test pins / probes:
 *  - Must be an ADC port
 *  - Lower 3 pins of the port must be used for probes.
 *  - Please don't change the definitions of TP1, TP2 and TP3!
 *  - Don't share this port with POWER_CTRL or TEST_BUTTON!
 */

#define ADC_PORT         PORTC     /* port data register */
#define ADC_DDR          DDRC      /* port data direction register */
#define ADC_PIN          PINC      /* port input pins register */
#define TP1              PC0       /* test pin / probe #1 */
#define TP2              PC1       /* test pin / probe #2 */
#define TP3              PC2       /* test pin / probe #3 */

#define TP_ZENER         PC3       /* test pin for 10:1 voltage divider */
#define TP_REF           PC4       /* test pin for 2.5V reference and relay */
#define TP_BAT           PC5       /* test pin for battery (4:1 voltage divider) */


/*
 *  probe resistors
 *  - For PWM/squarewave output via probe #2 R_RL_2 has to be PB2/OC1B.
 *  - Don't share this port with POWER_CTRL or TEST_BUTTON!
 */

#define R_PORT           PORTB     /* port data register */
#define R_DDR            DDRB      /* port data direction register */
#define R_RL_1           PB0       /* Rl (680R) for test pin #1 */
#define R_RH_1           PB1       /* Rh (470k) for test pin #1 */
#define R_RL_2           PB2       /* Rl (680R) for test pin #2 */
#define R_RH_2           PB3       /* Rh (470k) for test pin #2 */
#define R_RL_3           PB4       /* Rl (680R) for test pin #3 */
#define R_RH_3           PB5       /* Rh (470k) for test pin #3 */


/*
 *  dedicated signal output via OC1B
 *  - don't change this!
 */

#define SIGNAL_PORT      PORTB     /* port data register */
#define SIGNAL_DDR       DDRB      /* port data direction register */
#define SIGNAL_OUT       PB2       /* MCU's OC1B pin */


/*
 *  power control
 *  - can't be same port as ADC_PORT or R_PORT
 */

#define POWER_PORT       PORTD     /* port data register */
#define POWER_DDR        DDRD      /* port data direction register */
#define POWER_CTRL       PD6       /* control pin (1: on / 0: off) */


/*
 *  test push button 
 *  - can't be same port as ADC_PORT or R_PORT
 */

#define BUTTON_PORT      PORTD     /* port data register */
#define BUTTON_DDR       DDRD      /* port data direction register */
#define BUTTON_PIN       PIND      /* port input pins register */
#define TEST_BUTTON      PD7       /* test/start push button (low active) */


/*
 *  rotary encoder
 */

#define ENCODER_PORT     PORTD     /* port data register */
#define ENCODER_DDR      DDRD      /* port data direction register */
#define ENCODER_PIN      PIND      /* port input pins register */
#define ENCODER_A        PD3       /* rotary encoder A signal */
#define ENCODER_B        PD2       /* rotary encoder B signal */


/*
 *  increase/decrease push buttons
 */

#define KEY_PORT         PORTD     /* port data register */
#define KEY_DDR          DDRD      /* port data direction register */
#define KEY_PIN          PIND      /* port input pins register */
#define KEY_INC          PD2       /* increase push button (low active) */
#define KEY_DEC          PD3       /* decrease push button (low active) */


/*
 *  frequency counter
 *  - input must be pin PD4/T0
 */

#define COUNTER_PORT     PORTD     /* port data register */
#define COUNTER_DDR      DDRD      /* port data direction register */
#define COUNTER_IN       PD4       /* signal input T0 */


/*
 *  TTL serial interface
 *  - hardware USART uses
 *    USART0: Rx PD0 and Tx PD1
 */

/* for hardware RS232 */
#define SERIAL_USART     0         /* use USART0 */
/* for bit-bang RS232 */
#define SERIAL_PORT      PORTD     /* port data register */
#define SERIAL_DDR       DDRD      /* port data direction register */
#define SERIAL_PIN       PIND      /* port input pins register */
#define SERIAL_TX        PD1       /* pin for Tx (transmit) */
#define SERIAL_RX        PD0       /* pin for Rx (receive) */
#define SERIAL_PCINT     16        /* PCINT# for Rx pin */



/* ************************************************************************
 *   internal stuff
 * ************************************************************************ */


/* ADC reference selection: AVcc */
#define ADC_REF_VCC           (1 << REFS0)

/* ADC reference selection: internal 1.1V bandgap */
#define ADC_REF_BANDGAP       ((1 << REFS1) | (1 << REFS0))

/* ADC reference selection: filter mask for register bits */
#define ADC_REF_MASK          ((1 << REFS1) | (1 << REFS0))

/* ADC MUX channel: internal 1.1V bandgap reference */
#define ADC_CHAN_BANDGAP      0x0e      /* 1110 */

/* ADC MUX channel: filter mask for register bits */
#define ADC_CHAN_MASK         0b00001111     /* MUX0-3 */



/* LGT8F328P board-start registers (from CompuKit TC1 v19.15) */
#ifndef LGT_PMCR
#define LGT_PMCR   (*((volatile uint8_t *)0xF2))
#define LGT_ECCR   (*((volatile uint8_t *)0x56))
#endif

/*
 *  LGT8F328P analog comparator (databook v1.0.5, AC0 + DAC chapters):
 *  C0SR.C0BG (= ACBG) does NOT select a bandgap. With C0XR.C0PS0 = 0 it
 *  selects the internal 8-bit DAC output (DAO) as positive input. The DAC is
 *  off after reset, so the comparator would trip at ~0V and small caps read
 *  as a few 100pF. Run the DAC from the internal reference (1.024V via VCAL1)
 *  at full scale: V_DAO = VREF * (DALR + 1) / 256 = 1.024V = LGT_IVREF.
 */
#define LGT_DACON  _SFR_MEM8(0xA0)
#define LGT_DALR   _SFR_MEM8(0xA1)
#define LGT_DACEN  3                    /* DACON: DAC enable */
#define LGT_DAVS_IVREF  0x02            /* DACON: DAVS = 10, internal ref */

/*
 *  LGT8F328P ADC differs from ATmega328 (from CompuKit TC1 v19.15):
 *  - 12 bit result (0-4095) instead of 10 bit
 *  - REFS1:0 = 11 selects the internal 1.024V reference (not a 1.1V
 *    bandgap); it's calibrated by copying VCAL1 into VCAL
 *  - MUX channel 0x0e is AGND, so the reference can't be measured;
 *    Cfg.Bandgap is set to LGT_IVREF instead
 */
#define ADC_DIV               4096           /* full scale of 12 bit ADC */
#define ADC_RAW10             (ADCW >> 2)    /* raw reading scaled to 10 bit */
#define LGT_IVREF             1024           /* internal reference (mV) */
#define LGT_VCAL              _SFR_MEM8(0xC8)     /* reference calibration */
#define LGT_VCAL1             _SFR_MEM8(0xCD)     /* factory value for 1.024V */



/* ************************************************************************
 *   MCU specific setup to support different AVRs
 * ************************************************************************ */


/*
 *  LGT8F328P
 */

#if defined(__LGT8FX8P__)

  /* estimated internal resistance of port to GND (in 0.1 Ohms) */
  #define R_MCU_LOW           200

  /* estimated internal resistance of port to VCC (in 0.1 Ohms) */
  #define R_MCU_HIGH          220

  /* voltage offset of MCU's analog comparator (in mV): -50 up to 50 */
  #define COMPARATOR_OFFSET   0

  /* this MCU has 32kB Flash, 1kB EEPROM and 2kB RAM (enable extra features) */
  #define RES_FLASH           32
  #define RES_EEPROM          1
  #define RES_RAM             2


/*
 *  missing or unsupported MCU
 */

#else
  #error <<< No or wrong MCU type selected! >>>
#endif



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
