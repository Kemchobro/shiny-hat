/// @file    RGBSetDemo.ino
/// @brief   Demonstrates how to create an LED group with CRGBArray
/// @example RGBSetDemo.ino

#include <FastLED.h>
#include <bluefruit.h>

#define NUM_LEDS 100
#define PIN 1
#define LED_OFF LOW
#define LED_ON HIGH
#define BUTTON_ACTIVE LOW

const int LED_PIN = 7;
const int BUTTON_PIN = 13;
int lastButtonState = -1;

BLEUart bleuart; // uart over ble

CRGBArray<NUM_LEDS> leds;

void rgbSequence()
{
  static uint8_t hue;
  for (int i = 0; i < NUM_LEDS / 2; i++)
  {
    // fade everything out
    leds.fadeToBlackBy(40);

    // let's set an led value
    leds[i] = CHSV(hue++, 255, 255);

    // now, let's first 20 leds to the top 20 leds,
    leds(NUM_LEDS / 2, NUM_LEDS - 1) = leds(NUM_LEDS / 2 - 1, 0);
    FastLED.delay(33);
  }
}

void setup()
{
  FastLED.addLeds<WS2812B, PIN>(leds, NUM_LEDS);
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
    uint8_t c;
    // use bleuart.read() to read a character sent over BLE
    c = (uint8_t)bleuart.read();
    // Print out the character for debug purposes:
    Serial.write(c);

    // If the character is one of our expected values,
    // do something:
    switch (c)
    {
    // 0 number or character, turn the LED off:
    case 0:
    case '0':
      digitalWrite(LED_PIN, LED_OFF);
      break;
    // 1 number or character, turn the LED on:
    case 1:
    case '1':
      digitalWrite(LED_PIN, LED_ON);
      break;
    default:
      break;
    }
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
