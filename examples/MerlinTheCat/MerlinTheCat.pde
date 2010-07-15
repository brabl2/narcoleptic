/* Welcome to Narcoleptic. This library allows you to dramatically reduce
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
** Narcoleptic is available from: http://narcoleptic.googlecode.com
*/

/* To use Narcoleptic, you will need the following line. Arduino will
** auto-insert it if you select Sketch > Import Library > Narcoleptic. */

#include <Narcoleptic.h>

void setup() {
  pinMode(2,INPUT);
  digitalWrite(2,HIGH);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
}

void loop() {
  int a;

  // Merlin the cat is snoozing... Connect digital pin 2 to ground to wake him up.
  Narcoleptic.delay(500); // During this time power consumption is minimised

  while (digitalRead(2) == LOW) {
    // Wake up CPU. Unfortunately, Merlin does not like waking up.

    // Swipe claws left
    digitalWrite(13,HIGH);
    delay(50);
    
    // Swipe claws right
    digitalWrite(13,LOW);
    delay(50);
  }

  // Merlin the cat goes to sleep...
}