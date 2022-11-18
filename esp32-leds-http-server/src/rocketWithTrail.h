#pragma once
#include "rocket.h"

class RocketWithTrail : public Rocket
{
private:
  CHSV _color;
  int _trailLength;
public:
  RocketWithTrail(CRGB* leds, float speed, CHSV color, int trailLength=10);
  ~RocketWithTrail();
  void tick(float dt);
  bool isDead();
};
