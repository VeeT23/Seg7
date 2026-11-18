#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include "display.hpp"
#include "engine.hpp"
#include "router.hpp"

constexpr uint8_t SDA_PIN = 25;
constexpr uint8_t SCL_PIN = 26;

constexpr uint8_t LED_PIN = 33;

Router router;

RTC_DS3231 rtc;

Display display(500);

Engine engine(rtc);

void setup()
{
  router.begin_serial(115200);
  router.begin_bluetooth("ESP32");
  delay(200);

  router.println(Router::BroadcastType::STATUS, "Starting controller...");

  display.begin<LED_PIN>();

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);

  if (!rtc.begin())
  {
    router.println(Router::BroadcastType::ERROR, "Couldn't find DS3231!");
  }
  else
  {
    router.println(Router::BroadcastType::STATUS, "Detected DS3231");
  }

  engine.set_clock(rtc);
}

void loop()
{
  if (router.has_data())
    engine.pump_input(router.get_data());

  engine.update();

  engine.drain_output(router);
}