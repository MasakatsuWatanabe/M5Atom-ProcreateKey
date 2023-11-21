#include "M5Atom.h"
#include <BleKeyboard.h>
#include <FastLED.h>
#include <Ticker.h>

static const gpio_num_t GROVE_KEY_PIN = GPIO_NUM_32;   // Grove Button
static const gpio_num_t GROVE_LED_PIN = GPIO_NUM_26;  // Grove LED
static const gpio_num_t BUTTON_PIN = GPIO_NUM_39;
static const int SECONDS_TO_SUSPEND = 180;

static BleKeyboard bleKeyboard("M5ATOM Presenter");
static CRGB leds[1]; // LED, 1 light
static Ticker watcher;
static int seconds = 0;

void InputKey(char key);
void Proc();
void WaitingForConnection();
void ShowState(CRGB color);
void Watch();
void Sleep();

// Initialization on startup.
void setup()
{
    M5.begin(false, false, true);

    watcher.attach(1, Watch);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(GROVE_KEY_PIN, INPUT_PULLUP);

    ShowState(CRGB::Red);
    leds[0] = CRGB::Red;
    FastLED.addLeds<WS2812, GROVE_LED_PIN, GRB>(leds, 1);
    FastLED.setBrightness(20);

    // Setting to wake up sleep when Button A and Grove Key are pressed
    esp_sleep_enable_ext0_wakeup(BUTTON_PIN, LOW);
    esp_sleep_enable_ext1_wakeup(
        (uint64_t)1U<<((uint64_t)GROVE_KEY_PIN),
        ESP_EXT1_WAKEUP_ALL_LOW);
 
    bleKeyboard.begin();

    WaitingForConnection();
}

// Main loop.
void loop()
{
    M5.update();
    Proc();

    FastLED.show();
    delay(20);
}

// Indicate the state with the LED.
void ShowState(CRGB color)
{
    M5.dis.drawpix(0, color);
}

// Send a key to the connected device.
void InputKey(char key)
{
    bleKeyboard.press(key);
    delay(100);
    bleKeyboard.releaseAll();
}

// Wait for BLE connection.
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

// Process the button.
void Proc()
{
    if(!bleKeyboard.isConnected())
        WaitingForConnection();

    static bool pressed = false;
    bool press = digitalRead(GROVE_KEY_PIN) == LOW;

    if(pressed != press)
    {
        pressed = press;

        static const struct KEY
        {
            CRGB color;
            char key;
        }
        KeyErase = { CRGB::Green,  'e' },
        KeyBrush = { CRGB::Yellow, 'b' };

        const KEY& key = press ? KeyErase : KeyBrush;
        leds[0] = key.color;
        InputKey(key.key);
        
        seconds = 0;
    }
}

// Watch the time to suspend.
void Watch()
{
    seconds++;

    if(seconds< SECONDS_TO_SUSPEND)
        return;

    Sleep();
}

// Enter deep sleep.
void Sleep()
{
    // Ready for sleep.
    bleKeyboard.releaseAll();

    ShowState(CRGB::OrangeRed);
    leds[0] = CRGB::Black;

    M5.update();
    delay(200);

    // Enter deep sleep, restarted on resume.
    esp_deep_sleep_start();
}
