#include <Arduino.h>
#include <Tone32.h>

#define BUZZER_PIN 16
#define BUZZER_CHANNEL 0

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Init!");
  pinMode(21, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  // auto val = analogRead(34);
  // Serial.print(val);
  // Serial.print("\n");
  // delay(50);
  // digitalWrite(21, HIGH);
  // delay(2000);
  // digitalWrite(21, LOW);
  // delay(500);

  tone(BUZZER_PIN, NOTE_C4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_D4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_E4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_F4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_G4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_A4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_B4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);

  delay(5000);
}
