#include "M5Atom.h"
#include <BleKeyboard.h>
#include <FastLED.h>
#include <Ticker.h>

static const gpio_num_t groveKeyPin = GPIO_NUM_32;   // Grove Button
static const gpio_num_t groveLedPin = GPIO_NUM_26;  // Grove LED
static const gpio_num_t buttonPin = GPIO_NUM_39;
static const int SecondsToSuspend = 180;

static BleKeyboard bleKeyboard("M5ATOM Presenter");
static CRGB leds[1]; // LED, 1 light
static Ticker watcher;
static int seconds = 0;

void InputKey(char key);
void Proc();
void WaitingForConnection();
void ShowState(CRGB color);
void Watch();

void setup()
{
    M5.begin(false, false, true);

    watcher.attach(1, Watch);

    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(groveKeyPin, INPUT_PULLUP);

    ShowState(CRGB::Red);
    leds[0] = CRGB::Red;
    FastLED.addLeds<WS2812, groveLedPin, GRB>(leds, 1);
    FastLED.setBrightness(10);

    // Setting to wake up sleep when Button A and Grove Key are pressed
    esp_sleep_enable_ext0_wakeup(buttonPin, LOW);
    esp_sleep_enable_ext1_wakeup(
        (uint64_t)1U<<((uint64_t)groveKeyPin),
        ESP_EXT1_WAKEUP_ALL_LOW);
 
    bleKeyboard.begin();

    WaitingForConnection();
}

void loop()
{
    M5.update();
    Proc();

    FastLED.show();
    delay(10);
}

void ShowState(CRGB color)
{
    M5.dis.drawpix(0, color);
}

void InputKey(char key)
{
    bleKeyboard.press(key);
    delay(100);
    bleKeyboard.releaseAll();
}

void WaitingForConnection()
{
    bool b = true;
    while(!bleKeyboard.isConnected())
    {
        ShowState(b ? CRGB::Red : CRGB::Black);
        b = !b;
        delay(500); 
    }

    ShowState(CRGB::Blue);
    leds[0] = CRGB::Green;

    bleKeyboard.releaseAll();
}

bool pressed = false;

void Proc()
{
    if(!bleKeyboard.isConnected())
        WaitingForConnection();

    bool press = digitalRead(groveKeyPin) == LOW;

    if(pressed != press)
    {
        pressed = press;

        static const struct KEY
        {
            CRGB color;
            char key;
        } keys[] =
        {
            { CRGB::Green,  'e' },
            { CRGB::Yellow, 'b' },
        };

        const KEY& key = keys[press ? 0 : 1];

        leds[0] = key.color;
        InputKey(key.key);
        
        seconds = 0;
    }
}

void Watch()
{
    seconds++;

    if(seconds< SecondsToSuspend)
        return;

    // Ready for sleep.
    bleKeyboard.releaseAll();

    ShowState(CRGB::OrangeRed);
    leds[0] = CRGB::Black;

    M5.update();
    delay(200);

    // Enter deep sleep, restarted on resume.
    esp_deep_sleep_start();
}
