#pragma once
#include "rocket.h"
#include <vector>

class RandomColorsRocket : public Rocket {
private:
  int _transitionLength;
  std::vector<CHSV> _colors;
  size_t _colorsLength;
public:
  RandomColorsRocket(CRGB* leds, float speed);
  // ~RandomColorsRocket();
  void tick(float dt);
};
