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

uint32_t watchdogTime_us = 16000;
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

void NarcolepticClass::sleep(uint8_t wdt_period,uint8_t sleep_mode) {
  
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
  set_sleep_mode(sleep_mode);

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
  millisCounter += milliseconds;
  uint32_t microseconds = milliseconds * 1000L;
  
  calibrate();
  microseconds -= watchdogTime_us;
  
  if (microseconds > 0) {
    uint16_t sleep_periods = microseconds / watchdogTime_us;
    while (sleep_periods >= 512) {
      sleep(WDTO_8S,SLEEP_MODE_PWR_DOWN);
      sleep_periods -= 512;
    }
    if (sleep_periods & 256) sleep(WDTO_4S,SLEEP_MODE_PWR_DOWN);
    if (sleep_periods & 128) sleep(WDTO_2S,SLEEP_MODE_PWR_DOWN);
    if (sleep_periods & 64) sleep(WDTO_1S,SLEEP_MODE_PWR_DOWN);
    if (sleep_periods & 32) sleep(WDTO_500MS,SLEEP_MODE_PWR_DOWN);
    if (sleep_periods & 16) sleep(WDTO_250MS,SLEEP_MODE_PWR_DOWN);
    if (sleep_periods & 8) sleep(WDTO_120MS,SLEEP_MODE_PWR_DOWN);
    if (sleep_periods & 4) sleep(WDTO_60MS,SLEEP_MODE_PWR_DOWN);
    if (sleep_periods & 2) sleep(WDTO_30MS,SLEEP_MODE_PWR_DOWN);
    if (sleep_periods & 1) sleep(WDTO_15MS,SLEEP_MODE_PWR_DOWN);
  }
}

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
  // Set clock to /64 (should take 15625 cycles at 16MHz clock)
  TCCR1B = _BV(CS11) | _BV(CS10);
  sleep(WDTO_15MS,SLEEP_MODE_IDLE);
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
  
  watchdogTime_us = watchdogDuration * (64 * 1000000 / F_CPU);
}

uint32_t NarcolepticClass::millis() {
  return millisCounter;
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
