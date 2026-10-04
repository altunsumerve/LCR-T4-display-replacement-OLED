/* ************************************************************************
 *
 *   pause functions
 *
 *   (c) 2012-2020 by Markus Reschke
 *
 * ************************************************************************ */


/*
 *  local constants
 */

/* source management */
#define PAUSE_C


/*
 *  include header files
 */

/* local includes */
#include "config.h"           /* global configuration */
#include "common.h"           /* common header file */
#include "variables.h"        /* global variables */
#include "functions.h"        /* external functions */



/* ************************************************************************
 *   sleep functions
 * ************************************************************************ */


/*
 *  enter MCU sleep mode for a specific time in ms
 *  - valid time 0 - 65535ms
 *  - uses timer2
 *  - don't use this function for time critical stuff!
 */

void MilliSleep(uint16_t Time)
{
#ifdef __LGT8FX8P__
  /*
   * TC1/LGT8F328P: use a plain busy-wait instead of Timer2.
   * The LCD-only cold-boot diagnostic proved this delay method works
   * reliably from a true power-on. Reset the watchdog every millisecond
   * so long UI delays (1.5-3 s) remain safe after the watchdog is enabled.
   */
  while (Time > 0)
  {
    wdt_reset();
    _delay_ms(1);
    Time--;
  }
#else
  uint32_t               Cycles;        /* timer cycles */
  uint8_t                Timeout;       /* compare value */
  uint8_t                Flag = 0;      /* interrupt flag */
  #ifdef SAVE_POWER
  uint8_t                Mode;          /* sleep mode */
  #endif

  #ifdef SAVE_POWER
  Mode = Cfg.SleepMode;                 /* get current sleep mode */
  #endif

  Cycles = Time;
  Cycles *= MCU_CYCLES_PER_US;

  #ifdef SAVE_POWER
  if (Mode == SLEEP_MODE_PWR_SAVE)
  {
    uint32_t Value;
    Value = Cycles / 256;
    Value++;
    Value *= (OSC_STARTUP / 1024);
    if (Cycles > Value) Cycles -= Value;
    else Mode = SLEEP_MODE_IDLE;
  }
  #endif

  TCCR2B = 0;
  TCNT2 = 0;
  TCCR2A = (1 << WGM21);
  TIMSK2 = (1 << OCIE2A);

  #ifdef SAVE_POWER
  set_sleep_mode(Mode);
  #endif

  if (SREG & (1 << SREG_I)) Flag = 1;
  else sei();

  while (Cycles > 0)
  {
    wdt_reset();
    if (Cycles > 255) Timeout = 255;
    else Timeout = (uint8_t)Cycles;
    Cycles -= Timeout;
    Timeout--;
    OCR2A = Timeout;
    TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
    while (TCCR2B != 0)
    {
      #ifdef SAVE_POWER
      sleep_mode();
      #else
      asm volatile(
        "nop\n\t"
        "nop\n\t"
        ::
      );
      #endif
    }
  }

  if (Flag == 0) cli();
#endif
}



/*
 *  ISR for match of Timer2's OCR2A (Output Compare Register A)
 */

ISR(TIMER2_COMPA_vect, ISR_BLOCK)
{
  /*
   *  hints:
   *  - the OCF2A interrupt flag is cleared automatically
   *  - interrupt processing is disabled while this ISR runs
   *    (no nested interrupts)
   */

  TCCR2B = 0;                 /* stop Timer2 */
}



/* ************************************************************************
 *   clean-up of local constants
 * ************************************************************************ */


/* source management */
#undef PAUSE_C



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
