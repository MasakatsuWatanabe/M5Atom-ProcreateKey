#include "M5Atom.h"
#include <BleKeyboard.h>
#include <FastLED.h>

BleKeyboard bleKeyboard("M5ATOM Presenter");

namespace Color
{
  static const int Yellow = 0xffff00;
  static const int Red    = 0xff0000;
  static const int Blue   = 0x0000ff;
  static const int Green  = 0x00ff00;
}

static const int buttonPin = 32; // Grove ボタン
static const int ledPin = 26;    // Grove LED

CRGB leds[1]; // LED、1灯

void showState(int color)
{
  M5.dis.drawpix(0, color);
}

void setup() {

  M5.begin(true, false, true);

  delay(50);

  pinMode(buttonPin, INPUT_PULLUP);
  
  leds[0] = CRGB::Red;
  FastLED.addLeds<WS2812, 26, GRB>(leds, 1);
  FastLED.setBrightness(20);
  
  bleKeyboard.begin();
}

bool blestate = false;
bool pressed = false;

void loop()
{
    while(!bleKeyboard.isConnected())
    {
        showState(Color::Red);
        bleKeyboard.releaseAll();
        delay(500); 
    }

    showState(Color::Blue);

    bool press = digitalRead(buttonPin) == LOW;

    if(pressed != press)
    {
      if(press)
      {
        leds[0] = CRGB::Green;

        bleKeyboard.press('e');
        delay(100);
        bleKeyboard.releaseAll();
      }
      else
      {
        leds[0] = CRGB::Yellow;

        bleKeyboard.press('b');
        delay(100);
        bleKeyboard.releaseAll();
      }

      pressed = press;
    }

    M5.update();
    FastLED.show();
}
