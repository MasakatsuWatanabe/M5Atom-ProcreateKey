#include "M5Atom.h"
#include <BleKeyboard.h>
#include <FastLED.h>

static const int buttonPin = 32; // Grove ボタン
static const int ledPin = 26;    // Grove LED

BleKeyboard bleKeyboard("M5ATOM Presenter");

enum Color
{
    Yellow = 0xffff00,
    Red    = 0xff0000,
    Blue   = 0x0000ff,
    Green  = 0x00ff00,
    Black  = 0x000000
};

CRGB leds[1]; // LED、1灯

void showState(Color color);
void InputKey(char key);
void Proc();
void WaitingForConnection();

// 初期化
void setup()
{
    M5.begin(false, false, true);

    pinMode(buttonPin, INPUT_PULLUP);
    leds[0] = CRGB::Red;
    FastLED.addLeds<WS2812, 26, GRB>(leds, 1);
    FastLED.setBrightness(20);

    bleKeyboard.begin();

    WaitingForConnection();
}

// メインループ
void loop()
{
    M5.update();
    Proc();

    FastLED.show();
    delay(10);
}

// ステート表示
void showState(Color color)
{
    M5.dis.drawpix(0, color);
}

// キー入力
void InputKey(char key)
{
    bleKeyboard.press(key);
    delay(100);
    bleKeyboard.releaseAll();
}

// BLE接続待ち
void WaitingForConnection()
{
    bool b = true;
    while(!bleKeyboard.isConnected())
    {
        showState(b ? Color::Red : Color::Black);
        b = !b;
        delay(500); 
    }

    showState(Color::Blue);
    leds[0] = CRGB::Green;

    bleKeyboard.releaseAll();
}

bool blestate = true;
bool pressed = false;

// 処理本体
void Proc()
{
    if(!bleKeyboard.isConnected())
        WaitingForConnection();

    bool press = digitalRead(buttonPin) == LOW;

    if(pressed != press)
    {
        pressed = press;

        static const struct KEY
        {
            Color color;
            char key;
        } keys[] =
        {
            { Green,  'e' },
            { Yellow, 'b' },
        };

        const KEY& key = keys[press ? 0 : 1];

        leds[0] = key.color;
        InputKey(key.key);
    }
}
