#include <Arduino.h>
#include <FastLED.h>

constexpr uint8_t DATA_PIN = 12;

constexpr uint8_t DIGITS = 4;
constexpr uint8_t NUM_LEDS = 35 * DIGITS;

CRGB leds[NUM_LEDS];

// order: c, b, a, f, e, g, d
void segment(uint16_t seg, CRGB clr) {
  uint16_t base = seg * 5;
  if (base + 4 >= NUM_LEDS) return;

  for (uint8_t i = 0; i < 5; i++)
    leds[base + i] = clr;
}

void display_digit(uint16_t digit, uint8_t value, CRGB clr) {

  if (digit > DIGITS - 1) { return; }
  const uint8_t digit_map[10] = {
    // c b a f e g d
    0b01011111,  // 0  (a,b,c,d,e,f)
    0b00000011,  // 1  (b,c)
    0b01110110,  // 2  (a,b,d,e,g)
    0b01100111,  // 3  (a,b,c,d,g)
    0b00101011,  // 4  (b,c,f,g)
    0b01101101,  // 5  (a,c,d,f,g)
    0b01111101,  // 6  (a,c,d,e,f,g)
    0b00000111,  // 7  (a,b,c)
    0b01111111,  // 8  (all)
    0b01101111   // 9  (a,b,c,d,f,g)
  };

  if (value > 9) return;

  uint8_t mask = digit_map[value];

  for (uint8_t seg = 0; seg < 7; seg++) {
    if (mask & (1 << seg)) {
      segment(digit * 7 + seg, clr);
    }
  }
}

void setup()
{
    Serial.begin(115200);

    FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);

    
}

void loop()
{
    Serial.println("Running.");
    FastLED.showColor(CRGB::Blue);
    //display_digit(0,9,CRGB::Red);
    delay(1000);
}