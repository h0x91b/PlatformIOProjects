#include "rocketWithTrail.h"

RocketWithTrail::RocketWithTrail(CRGB* leds, float speed, CHSV color, int trailLength) {
  _speed = speed;
  _color = color;
  _leds = leds;
  _location = 0;
  _isDead = false;
  _trailLength = trailLength;
}

RocketWithTrail::~RocketWithTrail() {
}

void RocketWithTrail::tick(float dt) {
  _location += _speed * dt;
  int index = _location * LEDS_PER_METER;
  if(index - _trailLength > NUM_LEDS) {
    _isDead = true;
    return;
  }
  if(index < NUM_LEDS) _leds[index] = CHSV(_color.hue, _color.sat, _color.val);
  for(int i=0;i<_trailLength;i++) {
    if(index - i < 0 || index - i > NUM_LEDS ) continue;
    _leds[index - i] = CHSV(
      _color.hue, 
      _color.sat, 
      map(i, 0, _trailLength, _color.val, 70)
    );
  }
}

