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
#include "Narcoleptic.h"

#if !defined(PRR) && defined(PRR0)
#define PRR PRR0
#endif

#if NARCOLEPTIC_CALIBRATION_ENABLE
uint32_t watchdogTime_us = 16000;
#endif
uint32_t millisCounter = 0;

SIGNAL(WDT_vect) {
  wdt_disable();
  wdt_reset();
#ifdef WDTCSR
  WDTCSR &= ~_BV(WDIE);
#else
  WDTCR &= ~_BV(WDIE);
#endif
}

void NarcolepticClass::sleepAdv(uint8_t wdt_period,uint8_t sleep_mode,uint8_t eimsk,uint8_t pcmsk0,uint8_t pcmsk1,uint8_t pcmsk2,uint8_t twie) {
// Sleep with advanced wake-up: additional wake-up events can be enabled based on the following table:
//             7       6       5       4       3       2       1       0
// EIMSK  :                                                  INT1    INT0
// PCMSK0 :  PCINT7  PCINT6  PCINT5  PCINT4  PCINT3  PCINT2  PCINT1  PCINT0
// PCMSK1 :          PCINT14 PCINT13 PCINT12 PCINT11 PCINT10 PCINT9  PCINT8
// PCMSK2 :  PCINT23 PCINT22 PCINT21 PCINT20 PCINT19 PCINT18 PCINT17 PCINT16
// twie   :  enables TWIE bit in TWCR register
//

  MCUSR = 0;
#ifdef WDTCSR
  WDTCSR &= ~_BV(WDE);
  WDTCSR = _BV(WDIF) | _BV(WDIE) | _BV(WDCE);
#else
  WDTCR &= ~_BV(WDE);
  WDTCR = _BV(WDIF) | _BV(WDIE) | _BV(WDCE);
#endif

  wdt_enable(wdt_period);
  wdt_reset();
#ifdef WDTCSR
  WDTCSR |= _BV(WDIE);
#else
  WDTCR |= _BV(WDIE);
#endif
  set_sleep_mode(sleep_mode);

  // Disable all interrupts
  uint8_t SREGcopy = SREG; cli();

#ifdef EECR
  uint8_t EECRcopy = EECR; EECR &= ~_BV(EERIE);
#endif
#ifdef EIMSK
  uint8_t EIMSKcopy = EIMSK; EIMSK = eimsk;
#endif
#ifdef PCMSK0
  uint8_t PCMSK0copy = PCMSK0; PCMSK0 = pcmsk0;
#endif
#ifdef PCMSK1
  uint8_t PCMSK1copy = PCMSK1; PCMSK1 = pcmsk1;
#endif
#ifdef PCMSK2
  uint8_t PCMSK2copy = PCMSK2; PCMSK2 = pcmsk2;
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
  uint8_t TWCRcopy = TWCR;
  if (!twie) {
    TWCR &= ~_BV(TWIE);
  }
#endif
#ifdef ACSR
  uint8_t ACSRcopy = ACSR; ACSR &= ~_BV(ACIE);
#endif
#ifdef ADCSRA
  uint8_t ADCSRAcopy = ADCSRA; ADCSRA &= ~_BV(ADIE);
#endif
#if defined(SPMCSR) && defined(SPMIE)
  uint8_t SPMCSRcopy = SPMCSR; SPMCSR &= ~_BV(SPMIE);
#endif

  sei();
  sleep_mode();            // here the device is actually put to sleep!!
  wdt_disable();           // first thing after waking from sleep: disable watchdog...

  // Reenable all interrupts
#if defined(SPMCSR) && defined(SPMIE)
  SPMCSR = SPMCSRcopy;
#endif
#ifdef ADCSRA
  ADCSRA = ADCSRAcopy;
#endif
#ifdef ACSR
  ACSR = ACSRcopy;
#endif
#ifdef TWCR
  if (!twie) {
    TWCR = TWCRcopy;
  }
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

void NarcolepticClass::sleep(uint8_t wdt_period) {
  sleepAdv(wdt_period,SLEEP_MODE_PWR_DOWN,0,0,0,0,0);
}

void NarcolepticClass::delay(uint32_t milliseconds) {
  millisCounter += milliseconds;
#if NARCOLEPTIC_CALIBRATION_ENABLE
  uint32_t microseconds = milliseconds * 1000L;

  calibrate();
  if (microseconds > watchdogTime_us) {
    microseconds -= watchdogTime_us;
    uint32_t sleep_periods = microseconds / watchdogTime_us;
#else
    uint32_t sleep_periods = milliseconds / 16;
#endif
    while (sleep_periods >= 512) {
      sleep(WDTO_8S);
      sleep_periods -= 512;
    }
    if (sleep_periods & 256) sleep(WDTO_4S);
    if (sleep_periods & 128) sleep(WDTO_2S);
    if (sleep_periods & 64) sleep(WDTO_1S);
    if (sleep_periods & 32) sleep(WDTO_500MS);
    if (sleep_periods & 16) sleep(WDTO_250MS);
    if (sleep_periods & 8) sleep(WDTO_120MS);
    if (sleep_periods & 4) sleep(WDTO_60MS);
    if (sleep_periods & 2) sleep(WDTO_30MS);
    if (sleep_periods & 1) sleep(WDTO_15MS);
#if NARCOLEPTIC_CALIBRATION_ENABLE
  }
#endif
}

#if NARCOLEPTIC_CALIBRATION_ENABLE
void NarcolepticClass::calibrate() {
  // Calibration needs Timer 1. Ensure it is powered up.
  uint8_t PRRcopy = PRR;
  PRR &= ~_BV(PRTIM1);

  uint8_t TCCR1Bcopy = TCCR1B;
  TCCR1B &= ~(_BV(CS12) | _BV(CS11) | _BV(CS10)); // Stop clock immediately
  // Capture Timer 1 state
  uint8_t TCCR1Acopy = TCCR1A;
  uint16_t TCNT1copy = TCNT1;
  uint16_t OCR1Acopy = OCR1A;
  uint16_t OCR1Bcopy = OCR1B;
  uint16_t ICR1copy = ICR1;
  uint8_t TIMSK1copy = TIMSK1;
  uint8_t TIFR1copy = TIFR1;

  // Configure as simple count-up timer
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  TIMSK1 = 0;
  TIFR1 = 0;
  // Set clock to /64 (16ms should take approx. 4000 cycles at 16MHz clock)
  TCCR1B = _BV(CS11) | _BV(CS10);
  sleepAdv(WDTO_15MS,SLEEP_MODE_IDLE,0,0,0,0,0);
  uint16_t watchdogDuration = TCNT1;
  TCCR1B = 0; // Stop clock immediately

  // Restore Timer 1
  TIFR1 = TIFR1copy;
  TIMSK1 = TIMSK1copy;
  ICR1 = ICR1copy;
  OCR1B = OCR1Bcopy;
  OCR1A = OCR1Acopy;
  TCNT1 = TCNT1copy;
  TCCR1A = TCCR1Acopy;
  TCCR1B = TCCR1Bcopy;

  // Restore power reduction state
  PRR = PRRcopy;

  watchdogTime_us = watchdogDuration * (64 * 1000000 / F_CPU); // should be approx. 16000
}
#endif

uint32_t NarcolepticClass::millis() {
  return millisCounter;
}


void NarcolepticClass::disableWire() {
#ifdef PRTWI
  PRR |= _BV(PRTWI);
#endif
#ifdef PRTWI0
  PRR0 |= _BV(PRTWI0);
#endif
#ifdef PRTWI1
  PRR1 |= _BV(PRTWI1);
#endif
#ifdef PRUSI
  PRR |= _BV(PRUSI);
#endif
}
void NarcolepticClass::disableWire0() {
#ifdef PRTWI0
  PRR0 |= _BV(PRTWI0);
#endif
}
void NarcolepticClass::disableWire1() {
#ifdef PRTWI1
  PRR1 |= _BV(PRTWI1);
#endif
}
void NarcolepticClass::disableMillis() {
  PRR |= _BV(PRTIM0);
}
void NarcolepticClass::disableTimer1() {
  PRR |= _BV(PRTIM1);
}
void NarcolepticClass::disableTimer2() {
#ifdef PRTIM2
  PRR |= _BV(PRTIM2);
#endif
}
void NarcolepticClass::disableTimer3() {
#ifdef PRTIM3
  PRR1 |= _BV(PRTIM3);
#endif
}
void NarcolepticClass::disableTimer4() {
#ifdef PRTIM4
  PRR1 |= _BV(PRTIM4);
#endif
}
void NarcolepticClass::disableSerial() {
#ifdef PRUSART0
  PRR |= _BV(PRUSART0);
#endif
#ifdef PRUSART1
  PRR |= _BV(PRUSART1);
#endif
#ifdef PRUSART
  PRR |= _BV(PRUSART);
#endif
}
void NarcolepticClass::disableSerial0() {
#ifdef PRUSART0
  PRR |= _BV(PRUSART0);
#endif
#ifdef PRUSART
  PRR |= _BV(PRUSART);
#endif
}
void NarcolepticClass::disableSerial1() {
#ifdef PRUSART1
  PRR |= _BV(PRUSART1);
#endif
#ifdef PRUSART
  PRR |= _BV(PRUSART);
#endif
}
void NarcolepticClass::disableADC() {
#ifdef PRADC
  PRR |= _BV(PRADC);
#endif
}
void NarcolepticClass::disableSPI() {
#ifdef PRSPI
  PRR |= _BV(PRSPI);
#endif
#ifdef PRSPI0
  PRR0 |= _BV(PRSPI0);
#endif
#ifdef PRSPI1
  PRR1 |= _BV(PRSPI1);
#endif
}

void NarcolepticClass::disableSPI0() {
#ifdef PRSPI0
  PRR0 |= _BV(PRSPI0);
#endif
}
void NarcolepticClass::disableSPI1() {
#ifdef PRSPI1
  PRR1 |= _BV(PRSPI1);
#endif
}
void NarcolepticClass::disableTouch() {
#ifdef PRPTC
  PRR1 |= _BV(PRPTC);
#endif
}

void NarcolepticClass::enableWire() {
#ifdef PRTWI
  PRR &= ~_BV(PRTWI);
#endif
#ifdef PRTWI0
  PRR0 &= ~_BV(PRTWI0);
#endif
#ifdef PRTWI1
  PRR1 &= ~_BV(PRTWI1);
#endif
#ifdef PRUSI
  PRR &= ~_BV(PRUSI);
#endif
}
void NarcolepticClass::enableWire0() {
#ifdef PRTWI0
  PRR0 &= ~_BV(PRTWI0);
#endif
}
void NarcolepticClass::enableWire1() {
#ifdef PRTWI1
  PRR1 &= ~_BV(PRTWI1);
#endif
}
void NarcolepticClass::enableMillis() {
  PRR &= ~_BV(PRTIM0);
}
void NarcolepticClass::enableTimer1() {
  PRR &= ~_BV(PRTIM1);
}
void NarcolepticClass::enableTimer2() {
#ifdef PRTIM2
  PRR &= ~_BV(PRTIM2);
#endif
}
void NarcolepticClass::enableTimer3() {
#ifdef PRTIM3
  PRR1 &= ~_BV(PRTIM3);
#endif
}
void NarcolepticClass::enableTimer4() {
#ifdef PRTIM4
  PRR1 &= ~_BV(PRTIM4);
#endif
}
void NarcolepticClass::enableSerial() {
#ifdef PRUSART0
  PRR &= ~_BV(PRUSART0);
#endif
#ifdef PRUSART
  PRR &= ~_BV(PRUSART);
#endif
}
void NarcolepticClass::enableADC() {
#ifdef PRADC
  PRR &= ~_BV(PRADC);
#endif
}
void NarcolepticClass::enableSPI() {
#ifdef PRSPI
  PRR &= ~_BV(PRSPI);
#endif
#ifdef PRSPI0
  PRR0 &= ~_BV(PRSPI0);
#endif
#ifdef PRSPI1
  PRR1 &= ~_BV(PRSPI1);
#endif
}
void NarcolepticClass::enableSPI0() {
#ifdef PRSPI0
  PRR0 &= ~_BV(PRSPI0);
#endif
}
void NarcolepticClass::enableSPI1() {
#ifdef PRSPI1
  PRR1 &= ~_BV(PRSPI1);
#endif
}
void NarcolepticClass::enableTouch() {
#ifdef PRPTC
  PRR1 &= ~_BV(PRPTC);
#endif
}

NarcolepticClass Narcoleptic;
