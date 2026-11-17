#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <string>
#include <vector>
#include "display.hpp"
#include "engine.hpp"
#include "router.hpp"

constexpr uint8_t SDA_PIN = 25;
constexpr uint8_t SCL_PIN = 26;

constexpr uint8_t LED_PIN = 33;

Router router;

RTC_DS3231 rtc;

Display display(500);

Engine engine;

void setup()
{
  router.begin_serial(115200);
  router.begin_bluetooth("ESP32");
  delay(200);

  router.println("STATUS: Starting controller...");

  display.begin<LED_PIN>();

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);

  if (!rtc.begin())
  {
    router.println("ERROR: Couldn't find DS3231!");
  }
  else
  {
    router.println("STATUS: Detected DS3231");
  }
}

void loop()
{
  while (router.has_data())
  {
    std::string data = router.get_data();
    std::vector<std::string> reply = engine.handle_incoming_data(data);
    for (const std::string &line : reply)
    {
      router.println(line);
    }
  }
}