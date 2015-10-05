<b>Narcoleptic uses some features of the microcontroller to reduce power consumption.
These are:</b>

  <b>Sleep mode:</b>
    Freezes all microcontroller functions to dramatically reduce power.
    
  <b>128kHz internal oscillator:</b>
    This is a very low power oscillator, used to inform the microcontroller when to
    wake up.

  <b>Power reduction:</b>
    Unused microcontroller functions can be powered down to save additional power.


<b>Power saving tips:</b>
  Narcoleptic only reduces the current used by the microcontroller. It does not
    reduce the current of anything connected to it.
  A power-on LED can use 20mA.
  A voltage regulator can use 15mA.
  The FTDI USB to serial chip uses about 15mA, so Arduino Duemilanoves and
    Diecimilas are not suitable for low power use without modification.
  Running at 3.3V and 8MHz uses much less energy than 5V at 16MHz.
  Pull-up and pull-down resistors use energy if they are passing current. Prefer
    normally-open switches over normally-closed. Consider using software pullups,
    and only turning them on when you need them.
  The Atmel ATmega328P data sheet has a section on "Minimising Power Consumption".

<b>Narcoleptic tricks:</b>
  The 128kHz oscillator is not very accurate. It drifts by ±4.5% over the full
  voltage and temperature ranges. Narcoleptic measures its speed against the main
  clock, and bases all its timings on the corrected value. Provided the operating
  voltage and temperature remain stable, Narcoleptic should maintain < ±1% accuracy.

  The 128kHz oscillator only provides sleep periods of 8,4,2,1 seconds, 500, 250,
  125, 62, 31 and 15ms. Narcoleptic performs multiple sleeps to achieve the requested
  timing.

<b>Keeping track of time:</b>
  Narcoleptic shuts down almost everything - even the millis() counting routine.
  To get around this, Narcoleptic keeps track of the time in sleep mode.
  
  If you replace: millis()
  with:           (millis() + Narcoleptic.millis())

  your timing calculations will take into account the time in sleep mode.


<b>Narcoleptic gotchas:</b>
  When doing a Narcoleptic.delay(), the following systems are frozen:
  -  Serial communication
  -  PWM outputs
  -  millis() ticker
  -  All timer activity

  Narcoleptic is best used when nothing is going on anyway, to simulate a power-off
  state.

  In a well optimised circuit, Narcoleptic can run for several years off
  a couple of AAA batteries.
