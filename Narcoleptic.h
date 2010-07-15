/**
 * Narcoleptic - A sleep library for Arduino
 *
 * Copyright (c) 2010 Cathedrow, All rights reserved.
 */

#ifndef Narcoleptic_h
#define Narcoleptic_h

#include <inttypes.h>
#include <stdbool.h>

class NarcolepticClass
{
  public:
    void delay(int milliseconds);
  private:
    void sleep(uint8_t);
};
extern NarcolepticClass Narcoleptic;

#endif
