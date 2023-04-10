/// @file    RGBSetDemo.ino
/// @brief   Demonstrates how to create an LED group with CRGBArray
/// @example RGBSetDemo.ino

#include <FastLED.h>
#include <bluefruit.h>

#define NUM_LEDS 100
#define PIN 1
#define BRIGHTNESS 64
#define LED_OFF LOW
#define LED_ON HIGH
#define BUTTON_ACTIVE LOW
#define LED_TYPE WS2812B
#define COLOR_ORDER RGB
#define UPDATES_PER_SECOND 100

const int LED_PIN = 7;
const int BUTTON_PIN = 13;
int lastButtonState = -1;

BLEUart bleuart; // uart over ble

CRGBArray<NUM_LEDS> leds;

CRGBPalette16 currentPalette;
TBlendType currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
extern CRGBPalette16 myTripleStrange;
extern const TProgmemPalette16 myTripleStrange_p PROGMEM;

void setup()
{
  delay(3000); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
  // Initialize hardware:
  Serial.begin(9600);       // Serial is the USB serial port
  pinMode(LED_PIN, OUTPUT); // Turn on-board blue LED off
  digitalWrite(LED_PIN, LED_OFF);
  pinMode(BUTTON_PIN, INPUT);

  // Uncomment the code below to disable sharing
  // the connection LED on pin 7.
  // Bluefruit.autoConnLed(false);

  // Initialize Bluetooth:
  Bluefruit.begin();
  // Set max power. Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setTxPower(4);
  Bluefruit.setName("SparkFun_nRF52840");
  bleuart.begin();

  // Start advertising device and bleuart services
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(bleuart);
  Bluefruit.ScanResponse.addName();

  Bluefruit.Advertising.restartOnDisconnect(true);
  // Set advertising interval (in unit of 0.625ms):
  Bluefruit.Advertising.setInterval(32, 244);
  // number of seconds in fast mode:
  Bluefruit.Advertising.setFastTimeout(30);
  Bluefruit.Advertising.start(0);
}

void loop()
{
  // If data has come in via BLE:
  if (bleuart.available())
  {
    uint8_t lightChooser;
    // use bleuart.read() to read a character sent over BLE
    lightChooser = (uint8_t)bleuart.read();
    // Print out the character for debug purposes:
    Serial.write(lightChooser);

    // If the character is one of our expected values,
    // do something:
    // ChangePalettePeriodically();
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */

    // FillLEDsFromPaletteColors(startIndex);
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
  }

  // If our button state has changed:
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState != lastButtonState)
  {
    lastButtonState = buttonState;
    // Write the new button state to the bleuart TX char
    bleuart.write(!buttonState);
  }
}

void FillLEDsFromPaletteColors(uint8_t colorIndex)
{
  uint8_t brightness = 255;

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}

// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically()
{
  uint8_t secondHand = (millis() / 1000) % 60;
  static uint8_t lastSecond = 99;

  if (lastSecond != secondHand)
  {
    lastSecond = secondHand;

    if (secondHand == 49)
    {
      currentPalette = myTripleStrange_p;
      currentBlending = LINEARBLEND;
    }
  }
}

const TProgmemPalette16 myTripleStrange_p PROGMEM =
    {
        CRGB::Blue,
        CRGB::Gray,
        CRGB::Orange,
        CRGB::Gray,
        CRGB::Blue,
        CRGB::Gray,
        CRGB::Orange,
        CRGB::Gray,
        CRGB::Blue,
        CRGB::Gray,
        CRGB::Orange,
        CRGB::Gray,
        CRGB::Blue,
        CRGB::Gray,
        CRGB::Orange,
        CRGB::Gray};
