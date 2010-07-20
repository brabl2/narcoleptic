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
  
#ifdef BODSE
  // Turn off BOD in sleep (picopower devices only)
  MCUCR |= _BV(BODSE);
  MCUCR |= _BV(BODS);
#endif

  MCUSR = 0;
  WDTCSR &= ~_BV(WDE);
  WDTCSR = _BV(WDIF) | _BV(WDIE) | _BV(WDCE);
  
  wdt_enable(wdt_period);
  wdt_reset();
#ifdef WDTCSR
  WDTCSR |= _BV(WDIE);
#else
  WDTCR |= _BV(WDIE);
#endif
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Disable all interrupts
  uint8_t SREGcopy = SREG; cli();

#ifdef EECR
  uint8_t EECRcopy = EECR; EECR &= ~_BV(EERIE);
#endif
#ifdef EIMSK
  uint8_t EIMSKcopy = EIMSK; EIMSK = 0;
#endif
#ifdef PCMSK0
  uint8_t PCMSK0copy = PCMSK0; PCMSK0 = 0;
#endif
#ifdef PCMSK1
  uint8_t PCMSK1copy = PCMSK1; PCMSK1 = 0;
#endif
#ifdef PCMSK2
  uint8_t PCMSK2copy = PCMSK2; PCMSK2 = 0;
#endif
#ifdef TIMSK0
  uint8_t TIMSK0copy = TIMSK0; TIMSK0 = 0;
#endif
#ifdef TIMSK1
  uint8_t TIMSK1copy = TIMSK1; TIMSK1 = 0;
#endif
#ifdef TIMSK2
  uint8_t TIMSK2copy = TIMSK2; TIMSK2 = 0;
#endif
#ifdef SPCR
  uint8_t SPCRcopy = SPCR; SPCR &= ~_BV(SPIE);
#endif
#ifdef UCSR0B
  uint8_t UCSR0Bcopy = UCSR0B; UCSR0B &= ~(_BV(RXCIE0) | _BV(TXCIE0) | _BV(UDRIE0));
#endif
#ifdef TWCR
  uint8_t TWCRcopy = TWCR; TWCR &= ~_BV(TWIE);
#endif
#ifdef ACSR
  uint8_t ACSRcopy = ACSR; ACSR &= ~_BV(ACIE);
#endif
#ifdef ADCSRA
  uint8_t ADCSRAcopy = ADCSRA; ADCSRA &= ~_BV(ADIE);
#endif
#ifdef SPMCSR
  uint8_t SPMCSRcopy = SPMCSR; SPMCSR &= ~_BV(SPMIE);
#endif
  
  sei();
  sleep_mode();
  wdt_disable();

  // Reenable all interrupts
#ifdef SPMCSR
  SPMCSR = SPMCSRcopy;
#endif
#ifdef ADCSRA
  ADCSRA = ADCSRAcopy;
#endif
#ifdef ACSR
  ACSR = ACSRcopy;
#endif
#ifdef TWCR
  TWCR = TWCRcopy;
#endif
#ifdef UCSR0B
  UCSR0B = UCSR0Bcopy;
#endif
#ifdef SPCR
  SPCR = SPCRcopy;
#endif
#ifdef TIMSK2
  TIMSK2 = TIMSK2copy;
#endif
#ifdef TIMSK1
  TIMSK1 = TIMSK1copy;
#endif
#ifdef TIMSK0
  TIMSK0 = TIMSK0copy;
#endif
#ifdef PCMSK2
  PCMSK2 = PCMSK2copy;
#endif
#ifdef PCMSK1
  PCMSK1 = PCMSK1copy;
#endif
#ifdef PCMSK0
  PCMSK0 = PCMSK0copy;
#endif
#ifdef EIMSK
  EIMSK = EIMSKcopy;
#endif
#ifdef EECR
  EECR = EECRcopy;
#endif

  SREG = SREGcopy;
  
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
