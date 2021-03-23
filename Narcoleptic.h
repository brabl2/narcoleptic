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

#ifndef Narcoleptic_h
#define Narcoleptic_h

#include <inttypes.h>
#include <stdbool.h>
#include <avr/sleep.h>

#define enableTimer0 enableMillis
#define disableTimer0 disableMillis

class NarcolepticClass
{
  public:
    void delay(uint32_t milliseconds);                             // uncalibrated delay
    void delayCal(uint32_t milliseconds, uint8_t run_calibration); // calibrated delay
    void sleepAdv(uint8_t wdt_period,uint8_t sleep_mode=SLEEP_MODE_PWR_DOWN,uint8_t eimsk=0,uint8_t pcmsk0=0,uint8_t pcmsk1=0,uint8_t pcmsk2=0,uint8_t twie=0);
    void sleep(uint8_t wdt_period); // WDTO_8S, WDTO_4S, WDTO_2S, WDTO_1S, WDTO_500MS, WDTO_250MS, WDTO_120MS, WDTO_60MS, WDTO_30MS, WDTO_15MS
    uint32_t millis();

    void disableWire();
    void disableWire0();
    void disableWire1();
    void disableMillis();
    void disableTimer1();
    void disableTimer2();
    void disableTimer3();
    void disableTimer4();
    void disableSerial();
    void disableSerial0();
    void disableSerial1();
    void disableADC();
    void disableSPI();
    void disableSPI0();
    void disableSPI1();
    void disableTouch();

    void enableWire();
    void enableWire0();
    void enableWire1();
    void enableMillis();
    void enableTimer1();
    void enableTimer2();
    void enableTimer3();
    void enableTimer4();
    void enableSerial();
    void enableSerial0();
    void enableSerial1();
    void enableADC();
    void enableSPI();
    void enableSPI0();
    void enableSPI1();
    void enableTouch();

  private:
    uint16_t calibrate();
};
extern NarcolepticClass Narcoleptic;

#endif
