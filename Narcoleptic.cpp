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
  uint8_t 
    SREGcopy,EECRcopy,EIMSKcopy,PCMSK0copy,PCMSK1copy, 
    PCMSK2copy,TIMSK0copy,TIMSK1copy,TIMSK2copy,
    SPCRcopy,UCSR0Bcopy,TWCRcopy,ACSRcopy,
    ADCSRAcopy,SPMCSRcopy;
  
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
  SREGcopy = SREG; cli();
  EECRcopy = EECR; EECR &= ~_BV(EERIE);
  EIMSKcopy = EIMSK; EIMSK = 0;
  PCMSK0copy = PCMSK0; PCMSK0 = 0;
  PCMSK1copy = PCMSK1; PCMSK1 = 0;
  PCMSK2copy = PCMSK2; PCMSK2 = 0;
  TIMSK0copy = TIMSK0; TIMSK0 = 0;
  TIMSK1copy = TIMSK1; TIMSK1 = 0;
  TIMSK2copy = TIMSK2; TIMSK2 = 0;
  SPCRcopy = SPCR; SPCR &= ~_BV(SPIE);
  UCSR0Bcopy = UCSR0B; UCSR0B &= ~(_BV(RXCIE0) | _BV(TXCIE0) | _BV(UDRIE0));
  TWCRcopy = TWCR; TWCR &= ~_BV(TWIE);
  ACSRcopy = ACSR; ACSR &= ~_BV(ACIE);
  ADCSRAcopy = ADCSRA; ADCSRA &= ~_BV(ADIE);
  SPMCSRcopy = SPMCSR; SPMCSR &= ~_BV(SPMIE);
  
  sei();
  sleep_mode();
  wdt_disable();

  // Reenable all interrupts
  SPMCSR = SPMCSRcopy;
  ADCSRA = ADCSRAcopy;
  ACSR = ACSRcopy;
  TWCR = TWCRcopy;
  UCSR0B = UCSR0Bcopy;
  SPCR = SPCRcopy;
  TIMSK2 = TIMSK2copy;
  TIMSK1 = TIMSK1copy;
  TIMSK0 = TIMSK0copy;
  PCMSK2 = PCMSK2copy;
  PCMSK1 = PCMSK1copy;
  PCMSK0 = PCMSK0copy;
  EIMSK = EIMSKcopy;
  EECR = EECRcopy;

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
