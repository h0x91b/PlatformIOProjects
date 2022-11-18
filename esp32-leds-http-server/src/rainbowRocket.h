#pragma once
#include "rocket.h"

class RainbowRocket : public Rocket {
private:
  int _transitionLength;
public:
  RainbowRocket(CRGB* leds, float speed);
  void tick(float dt);
};
