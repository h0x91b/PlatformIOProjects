#include <Arduino.h>
#include <TM1638.h>

const int stb = 8;
const int clk = 9;
const int dio = 10;

TM1638 module(dio, clk, stb);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("TM1638 module test program");
  Serial.print("STB should be connected to ");
  Serial.println(stb);
  Serial.print("CLK should be connected to ");
  Serial.println(clk);
  Serial.print("DIO should be connected to ");
  Serial.println(dio);

  pinMode(stb, OUTPUT);
  pinMode(clk, OUTPUT);
  pinMode(dio, OUTPUT);

  // digitalWrite(stb, LOW);
  // shiftOut(dio, clk, LSBFIRST, 0x8f);
  // shiftOut(dio, clk, LSBFIRST, 0xff);
  // shiftOut(dio, clk, LSBFIRST, 0xff);
  // digitalWrite(stb, HIGH);
  module.setDisplayToHexNumber(0x1234ABCD, 0xF0);
}

void loop() {
  static unsigned long incr = 99999999;
  module.setDisplayToSignedDecNumber(incr--, 0, true);
  static byte lastKeys = 0;
  byte keys = module.getButtons();
  if(lastKeys != keys) {
    Serial.print("getButtons() -> ");
    Serial.println(keys, 16);

    for(int i=0; i<8; i++) {
      bool enabled = (keys & 1 << i) > 0;
      module.setLED(enabled, i);
    }

    lastKeys = keys;
  }
}