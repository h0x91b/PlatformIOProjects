#include "randomColorsRocket.h"

RandomColorsRocket::RandomColorsRocket(CRGB* leds, float speed) {
  _leds = leds;
  _speed = speed;
  _location = 0;
  _isDead = false;
  _transitionLength = random8(5, 7);
  _colorsLength = random8(2, 6);
  for(int i=0; i<_colorsLength; i++) {
    _colors.push_back(CHSV(random8(), 255, 255));
  }
}

void RandomColorsRocket::tick(float dt) {
  if(_location < 0) _location = 0;
  _location += _speed * dt;
  int index = _location * LEDS_PER_METER;

  int t;
  for(int i=0; i<_colorsLength; i++) {
    for(int n=0; n<_transitionLength; n++) {
      t = index - i * _transitionLength + n;
      if(t < 0 || t >= NUM_LEDS ) continue;
      _leds[t] = CHSV(
        _colors[i].hue, // map(n, 0, _transitionLength, _colors[i+1 % _colorsLength].hue, _colors[i].hue),
        _colors[i].val, // map(n, 0, _transitionLength, _colors[i+1 % _colorsLength].val, _colors[i].val),
        map(n, 0, _transitionLength, 255, 150)
      );
    }
  }
  if(
    index > NUM_LEDS * 2
  ) {
    _isDead = true;
    return;
  }
}

// RandomColorsRocket::~RandomColorsRocket() {
//   for(int i=0; i<_colorsLength; i++) {
//     delete _colors[i];
//   }
// }
