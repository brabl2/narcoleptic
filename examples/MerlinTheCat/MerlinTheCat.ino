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

/*
** Welcome to Narcoleptic. This library allows you to dramatically reduce
** the power consumption of your Arduino project.
**
** Whilst in a Narcoleptic delay, the CPU current consumption drops from
** around 15mA down to about 7µA.
**
** Note that Narcoleptic only shuts down the CPU. It does not shut down
** anything else consuming current - LEDs, or the USB to serial chip.
** For low current applications, consider using boards without those,
** such as the Sparkfun Arduino Pro with the power LED removed.
**
** The original version of Narcoleptic was available on: http://narcoleptic.googlecode.com
** Various forks can be found on GitHub: https://github.com/search?q=narcoleptic
*/

/* To use Narcoleptic, you will need the following line. Arduino will
** auto-insert it if you select Sketch > Import Library > Narcoleptic. */

#include <avr/wdt.h>
#include <Narcoleptic.h>

#define LED 13
#define BUTTON 2

void setup() {
  pinMode(BUTTON,INPUT_PULLUP);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);

  // Narcoleptic.disableMillis(); Do not disable millis - we need it for our delay() function.
  Narcoleptic.disableTimer1();
  Narcoleptic.disableTimer2();
  Narcoleptic.disableSerial();
//  Narcoleptic.disableADC();  // !!! enabling this causes >100uA consumption !!!
  Narcoleptic.disableWire();
  Narcoleptic.disableSPI();

  // Another tweaks to lower the power consumption
  ADCSRA &= ~(1<<ADEN); //Disable ADC
  ACSR = (1<<ACD); //Disable the analog comparator
  //
  // ATmega48/88/168/328
  DIDR0 = B00111111; //Disable digital input buffers on all ADC0-ADC5 pins
  DIDR1 = (1<<AIN1D)|(1<<AIN0D); //Disable digital input buffer on AIN1/0
  //
  // ATtiny25/45/85
  //Disable digital input buffers on all ADC0-ADC3 + AIN1/0 pins
  //DIDR0 = (1<<ADC0D)|(1<<ADC2D)|(1<<ADC3D)|(1<<ADC1D)|(1<<AIN1D)|(1<<AIN0D);
}

void loop() {

  // Merlin the cat is snoozing... Connect digital pin 2 to ground to wake him up.
  Narcoleptic.delay(500); // During this time power consumption is minimised

  // Instead of 'Narcoleptic.delay' function it is possible to use simple 'Narcoleptic.sleep' function 
  // with following fixed values from <avr/wdt.h>:
  // WDTO_8S, WDTO_4S, WDTO_2S, WDTO_1S, WDTO_500MS, WDTO_250MS, WDTO_120MS, WDTO_60MS, WDTO_30MS, WDTO_15MS 
  // Narcoleptic.sleep(WDTO_500MS); // During this time power consumption is minimised

  while (digitalRead(BUTTON) == LOW) {
    // Wake up CPU. Unfortunately, Merlin does not like waking up.

    // Swipe claws left
    digitalWrite(LED,HIGH);
    Narcoleptic.sleep(WDTO_60MS);//delay(50);

    // Swipe claws right
    digitalWrite(LED,LOW);
    Narcoleptic.sleep(WDTO_60MS);//delay(50);
  }

  // Merlin the cat goes to sleep...
}
