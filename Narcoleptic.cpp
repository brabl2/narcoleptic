/**
 * Narcoleptic - A sleep library for Arduino
 * Copyright (C) 2010 Peter Knight (Cathedrow)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/common.h>
//#include <util/delay.h>

#include <avr/wdt.h>
#include <avr/sleep.h>
#include "Narcoleptic.h"

SIGNAL(WDT_vect) {
  wdt_disable();
  wdt_reset();
#ifdef WDTCSR
  WDTCSR &= ~_BV(WDIE);
#else
  WDTCR &= ~_BV(WDIE);
#endif
}

void NarcolepticClass::sleep(uint8_t wdt_period) {
  wdt_enable(wdt_period);
  wdt_reset();
#ifdef WDTCSR
  WDTCSR |= ~_BV(WDIE);
#else
  WDTCR |= ~_BV(WDIE);
#endif
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_mode();
  wdt_disable();
#ifdef WDTCSR
  WDTCSR &= ~_BV(WDIE);
#else
  WDTCR &= ~_BV(WDIE);
#endif
}

void NarcolepticClass::delay(int milliseconds) {
  while (milliseconds >= 8000) { sleep(WDTO_8S); milliseconds -= 8000; }
  if (milliseconds >= 4000)    { sleep(WDTO_4S); milliseconds -= 4000; }
  if (milliseconds >= 2000)    { sleep(WDTO_2S); milliseconds -= 2000; }
  if (milliseconds >= 1000)    { sleep(WDTO_1S); milliseconds -= 1000; }
  if (milliseconds >= 500)     { sleep(WDTO_500MS); milliseconds -= 500; }
  if (milliseconds >= 250)     { sleep(WDTO_250MS); milliseconds -= 250; }
  if (milliseconds >= 120)     { sleep(WDTO_120MS); milliseconds -= 120; }
  if (milliseconds >= 60)      { sleep(WDTO_60MS); milliseconds -= 60; }
  if (milliseconds >= 30)      { sleep(WDTO_30MS); milliseconds -= 30; }
  if (milliseconds >= 15)      { sleep(WDTO_15MS); milliseconds -= 15; }
}

void NarcolepticClass::begin() {
#ifdef BODSE
  // Turn off BOD in sleep (picopower devices only)
  MCUCR |= _BV(BODSE);
  MCUCR |= _BV(BODS);
#endif
}

void NarcolepticClass::disableWire() {
  PRR |= _BV(PRTWI);
}
void NarcolepticClass::disableTimer2() {
  PRR |= _BV(PRTIM2);
}
void NarcolepticClass::disableTimer1() {
  PRR |= _BV(PRTIM1);
}
void NarcolepticClass::disableMillis() {
  PRR |= _BV(PRTIM0);
}
void NarcolepticClass::disableSerial() {
  PRR |= _BV(PRUSART0);
}
void NarcolepticClass::disableADC() {
  PRR |= _BV(PRADC);
}
void NarcolepticClass::disableSPI() {
  PRR |= _BV(PRSPI);
}

void NarcolepticClass::enableWire() {
  PRR &= ~_BV(PRTWI);
}
void NarcolepticClass::enableTimer2() {
  PRR &= ~_BV(PRTIM2);
}
void NarcolepticClass::enableTimer1() {
  PRR &= ~_BV(PRTIM1);
}
void NarcolepticClass::enableMillis() {
  PRR &= ~_BV(PRTIM0);
}
void NarcolepticClass::enableSerial() {
  PRR &= ~_BV(PRUSART0);
}
void NarcolepticClass::enableADC() {
  PRR &= ~_BV(PRADC);
}
void NarcolepticClass::enableSPI() {
  PRR &= ~_BV(PRSPI);
}

NarcolepticClass Narcoleptic;
