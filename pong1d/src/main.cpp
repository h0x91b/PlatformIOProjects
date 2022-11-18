/*
  Игра на адресной светодиодной ленте "Понг"
  - Нажимай кнопку, когда шарик попал в твою зону
  - Чем ближе к краю ленты, тем сильнее будет отскок
  - Нажал не в своей зоне - проиграл
  - Пропустил - проиграл
*/
#include <Arduino.h>
#include <Tone32.h>

#define LED_PIN 17     // пин ленты
#define LED_NUM 144   // кол-во светодиодов
#define LED_BR 150     // яркость ленты
#define B1_PIN 18      // пин кнопки 1
#define B2_PIN 19      // пин кнопки 2
#define BUZZ_PIN 16    // пин пищалки
#define BUZZER_CHANNEL 0
#define ZONE_SIZE 10  // размер зоны
#define MIN_SPEED 5   // минимальная скорость
#define MAX_SPEED 20  // максимальная скорость
#define WIN_SCORE 5   // победный счёт
// =============================================
#include <FastLED.h>

CRGB leds[LED_NUM];

void newGame();
void pollButtons();
void gameRoutine();
void newRound();
void gameOver(byte player);

// ========== КНОПКА ==========
// мини класс кнопки
#define BTN_DEB 50    // дебаунс, мс
struct Button {
  public:
    Button (byte pin) {
      _pin = pin;
      pinMode(_pin, INPUT_PULLUP);
    }
    bool click() {
      bool btnState = digitalRead(_pin);
      if (!btnState && !_flag && millis() - _tmr >= BTN_DEB) {
        _flag = true;
        _tmr = millis();
        return true;
      }
      if (btnState && _flag && millis() - _tmr >= BTN_DEB) {
        _flag = false;
        _tmr = millis();
      }
      return false;
    }
    uint32_t _tmr;
    byte _pin;
    bool _flag;
};
Button b1(B1_PIN);
Button b2(B2_PIN);
// ============ SETUP =============
void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, LED_NUM);
  FastLED.setBrightness(LED_BR);
  pinMode(BUZZ_PIN, OUTPUT);
  newGame();
}
// ========== ПЕРЕМЕННЫЕ ==========
int pos = 0;
int spd;
byte score1 = 0, score2 = 0;
uint32_t tmr;
// ============= LOOP =============
void loop() {
  pollButtons();
  gameRoutine();
}
// ========= ОПРОС КНОПОК =========
void pollButtons() {
  if (b1.click()) {                             // произошёл клик игрока 1
    if (pos >= ZONE_SIZE * 10) gameOver(0);     // мячик вне зоны 1 игрока - проиграл
    else {                                      // мячик в зоне - отбил
      tone(BUZZ_PIN, 1200, 60, BUZZER_CHANNEL);
      spd = map(pos, ZONE_SIZE * 10, 0, MIN_SPEED, MAX_SPEED); // меняем скорость
    }
  }
  // аналогично для игрока 2
  if (b2.click()) {
    if (pos < (LED_NUM - ZONE_SIZE) * 10) gameOver(1);
    else {
      tone(BUZZ_PIN, 1200, 60, BUZZER_CHANNEL);
      spd = map(pos, (LED_NUM - ZONE_SIZE) * 10, LED_NUM * 10, -MIN_SPEED, -MAX_SPEED);
    }
  }
}
// ========= ЗАЛИВКА ЗОН =========
void fillZones(CRGB color1, CRGB color2) {
  // заливаем концы ленты переданными цветами
  for (int i = 0; i < ZONE_SIZE; i++) {
    leds[i] = color1;
    leds[LED_NUM - i - 1] = color2;
  }
}
// ========= МИГАТЬ И ПИЩАТЬ =========
// (цвет1, цвет2, частота, время задержки)
void blinkTone(CRGB color1, CRGB color2, int freq, int del) {
  fillZones(color1, color2);    // залить зоны
  FastLED.show();               // показать
  tone(BUZZ_PIN, freq, del, BUZZER_CHANNEL);         // пищать
  // delay(del);                   // ждём
  noTone(BUZZ_PIN);             // не пищать
  fillZones(0, 0);              // выключить зоны
  FastLED.show();               // показать
  delay(del);                   // ждать
}
// =========== ПРОИГРЫШ ===========
// (номер игрока, 0 или 1)
void gameOver(byte player) {
  newRound();    // новый раунд
  if (player == 0) {
    score2++;
    if (score2 == WIN_SCORE) {  // победил игрок 2
      score1 = score2 = 0;      // обнуляем счёт
      // победный бип бип бип игрока 2
      blinkTone(CRGB::Black, CRGB::Green, 600, 200);
      blinkTone(CRGB::Black, CRGB::Green, 600, 200);
      blinkTone(CRGB::Black, CRGB::Green, 600, 200);
      delay(500);
      newGame();  // новая игра
    } else blinkTone(CRGB::Red, CRGB::Green, 200, 500);   // красный бииип игрока 1
  } else {
    score1++;
    if (score1 == WIN_SCORE) {  // победил игрок 1
      score1 = score2 = 0;
      blinkTone(CRGB::Green, CRGB::Black, 600, 200);
      blinkTone(CRGB::Green, CRGB::Black, 600, 200);
      blinkTone(CRGB::Green, CRGB::Black, 600, 200);
      delay(500);
      newGame();  // новая игра
    } else blinkTone(CRGB::Green, CRGB::Red, 200, 500);
  }
}
// ============== НОВАЯ ИГРА ==============
void newGame() {
  blinkTone(CRGB::Red, CRGB::Red, 300, 300);
  blinkTone(CRGB::Yellow, CRGB::Yellow, 300, 300);
  blinkTone(CRGB::Green, CRGB::Green, 600, 300);
  fillZones(CRGB::Green, CRGB::Green);
  FastLED.show();
  randomSeed(millis()); // делаем случайные числа более случайными
  newRound();
}
// ============== НОВЫЙ РАУНД ==============
void newRound() {
  spd = random(0, 2) ? MIN_SPEED : -MIN_SPEED;  // случайное направление
  pos = (LED_NUM * 10) / 2;     // в центр ленты
}
// ============== ИГРА ==============
void gameRoutine() {
  if (millis() - tmr >= 10) {   // каждые 10 мс
    tmr = millis();
    pos += spd;     // двигаем мячик
    if (pos < 0) {  // вылетел слева
      gameOver(0);  // игрок 1 проиграл
      return;       // выходим
    }
    if (pos >= LED_NUM * 10) {  // вылетел справа
      gameOver(1);              // игрок 2 проиграл
      return;                   // выходим
    }
    FastLED.clear();
    fillZones(CRGB::Green, CRGB::Green);  // показываем зоны
    leds[pos / 10] = CRGB::Cyan;          // рисуем мячик
    FastLED.show();
  }
}