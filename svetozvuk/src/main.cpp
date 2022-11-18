#include <Arduino.h>

#define STRIP_PIN 2     // пин ленты
#define SOUND_PIN A0    // пин звука

#define COLOR_STEP 151  // шаг цвета, интересные 151, 129
#define LEDS_AM 300     // количество светодиодов
#define P_SPEED 2       // скорость движения

#define IDLE_TIME 10000

#include "VolAnalyzer.h"
VolAnalyzer sound(SOUND_PIN);

#include <FastLED.h>

CRGB fadeTowardColor( CRGB& cur, const CRGB& target, uint8_t amount);
CRGB leds[LEDS_AM];

byte curColor = 0;      // текущий цвет

// rainbow
// red, orange, yellow, green, blue, indigo, violet
byte rainbow[7][3] = {
  {0, 255, 255},
  {32, 255, 255},
  {64, 255, 255},
  {96, 255, 255},
  {128, 255, 255},
  {160, 255, 255},
  {192, 255, 255}
};

void setRainbow() {
  const int trail = 10;
  for(int c=0;c<7;c++) {
    for(int i=0;i<trail;i++) {
      leds[i + c*trail] = CHSV(
        map(i, 0, trail, rainbow[c][0], rainbow[(c+1) % 7][0]),
        map(i, 0, trail, rainbow[c][1], rainbow[(c+1) % 7][1]),
        map(i, 0, trail, rainbow[c][2], rainbow[(c+1) % 7][2])
      );
    }
  }
}

void setup() {
  Serial.begin(9600);

  FastLED.addLeds<WS2812, STRIP_PIN, GRB>(leds, LEDS_AM);
  FastLED.setBrightness(255);

  // настройки анализатора звука
  sound.setVolK(15);        // снизим фильтрацию громкости (макс. 31)
  sound.setVolMax(255);     // выход громкости 0-255
  sound.setPulseMax(40);   // сигнал пульса
  sound.setPulseMin(5);   // перезагрузка пульса

  Serial.println("rainbow " + sizeof(RainbowColors_p));

  setRainbow();
  FastLED.show();
}

void loop() {
  static unsigned long idleTimer = 0;
  if (sound.tick()) {
    for (int k = 0; k < P_SPEED; k++) {
      for (int i = LEDS_AM - 1; i > 0; i--) {
        leds[i] = leds[i - 1];
      }
    }
    
    // резкий звук - меняем цвет
    if (sound.pulse()) {
      // curColor += COLOR_STEP;
      // curColor = random8(0, 255);
      curColor = (curColor+1) % 7;

      Serial.println(sound.getVol());
      // color = RainbowColors_p[random8(0, 8)];

      idleTimer = millis();
    }
    CRGB color = CHSV(rainbow[curColor][0], rainbow[curColor][1], rainbow[curColor][2]);
    
    // берём текущий цвет с яркостью по громкости (0-255)
    // CRGB color = CHSV(curColor, 255, sound.getVol());
    
    const int trail = 7;
    for(int i=0;i<trail;i++) {
      CRGB c = CHSV(rainbow[curColor][0], rainbow[curColor][1], map(i, 0, trail, 0, sound.getVol()));
      leds[i] = c;
    }

    if(millis() - idleTimer > IDLE_TIME) {
      idleTimer = millis();
      // curColor += COLOR_STEP;
      if(random8() > 191) {
        setRainbow();
      } else {
        curColor = (curColor+1) % 7;
        for(int i=0;i<40;i++) {
          CRGB color = CHSV(rainbow[curColor][0], rainbow[curColor][1], map(i, 0, 40, 100, 255));
          leds[i] = color;
        }
      }
    }
    
    // выводим
    FastLED.show();
  }
}

// Helper function that blends one uint8_t toward another by a given amount
void nblendU8TowardU8( uint8_t& cur, const uint8_t target, uint8_t amount)
{
  if( cur == target) return;
  
  if( cur < target ) {
    uint8_t delta = target - cur;
    delta = scale8_video( delta, amount);
    cur += delta;
  } else {
    uint8_t delta = cur - target;
    delta = scale8_video( delta, amount);
    cur -= delta;
  }
}

// Blend one CRGB color toward another CRGB color by a given amount.
// Blending is linear, and done in the RGB color space.
// This function modifies 'cur' in place.
CRGB fadeTowardColor( CRGB& cur, const CRGB& target, uint8_t amount)
{
  nblendU8TowardU8( cur.red,   target.red,   amount);
  nblendU8TowardU8( cur.green, target.green, amount);
  nblendU8TowardU8( cur.blue,  target.blue,  amount);
  return cur;
}