#pragma once
#include "FastLED.h"
#include "globals.h"

class Rocket {
  public:
    virtual ~Rocket() = 0;
    virtual void tick(float dt) = 0;
    
    bool isDead() {
      return _isDead;
    }

  protected:
    float _location;
    float _speed;
    CRGB *_leds;
    bool _isDead = false;
};

extern CHSV rainbow[7];
