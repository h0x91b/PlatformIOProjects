#include "rainbowRocket.h"

RainbowRocket::RainbowRocket(CRGB* leds, float speed) {
  _leds = leds;
  _speed = speed;
  _location = 0;
  _isDead = false;
  _transitionLength = random8(3, 12);
}

void RainbowRocket::tick(float dt) {
  _location += _speed * dt;
  int index = _location * LEDS_PER_METER;
  // if(index < NUM_LEDS) _leds[index] = CHSV(
  //   _color.hue, 
  //   _color.sat, 
  //   map(index, 0, NUM_LEDS, _color.val, 0)
  // );
  int t;
  for(int i=0; i<7; i++) {
    for(int n=0; n<_transitionLength; n++) {
      t = index - i * _transitionLength + n;
      if(t < 0 || t >= NUM_LEDS ) continue;
      _leds[t] = CHSV(
        map(n, 0, _transitionLength, rainbow[i+1 % 7].hue, rainbow[i].hue),
        map(n, 0, _transitionLength, rainbow[i+1 % 7].val, rainbow[i].val),
        255
      );
    }
  }
  if(index - 7 * _transitionLength > NUM_LEDS) {
    _isDead = true;
    return;
  }
}
