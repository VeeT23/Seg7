#include <Arduino.h>
#include <Wire.h>

#include "display.hpp"
#include <RTClib.h>

constexpr uint8_t SDA_PIN = 25;
constexpr uint8_t SCL_PIN = 26;

constexpr uint8_t LED_PIN = 33;

RTC_DS3231 rtc;

Display display(500);


void setup()
{
  Serial.begin(115200);
  display.begin<LED_PIN>();

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);

  if (!rtc.begin())
  {
    Serial.println("Couldn't find DS3231!");
  }
  else
  {
    Serial.println("DS3231 detected!");
  }

  PanelID id = display.add_panel(Panel::PanelType::SegmentPanel);
  SegmentPanel& segment =
    static_cast<SegmentPanel&>(display.get_panel(id));
  segment.set_digit(5);

  display.get_matrix().fill(CRGB::Red);
  display.show();
  delay(350);
  display.get_matrix().fill(CRGB::Green);
  display.show();
  delay(350);
  display.get_matrix().fill(CRGB::Blue);
  display.show();
  delay(350);

  display.get_matrix().fill(CRGB::Red);
  display.show();
}

void loop()
{
  DateTime current_time = rtc.now();
 
  delay(500);
}