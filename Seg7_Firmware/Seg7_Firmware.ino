#include <FastLED.h> // Install 'FastLED' by Daniel Garcia
#include <Wire.h>
#include <RTClib.h> // Install 'RTClib' by Adafruit

#define DIGITS 5
#define NUM_LEDS (DIGITS * 7 * 5)

#define DATA_PIN 3

CRGB leds[NUM_LEDS];
RTC_DS3231 rtc;
char inputLine[40];
uint8_t inputPos = 0;
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 250; // 500 ms

DateTime now;

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

// order: c, b, a, f, e, g, d
void segment(uint16_t seg, CRGB clr) {
  uint16_t base = seg * 5;
  if (base + 4 >= NUM_LEDS) return;

  for (uint8_t i = 0; i < 5; i++)
    leds[base + i] = clr;
}


void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(500);
  Serial.println("Hello World from Arduino!");

  if (!rtc.begin()) {
    Serial.println("RTC not found");
    while (1)
      ;
  }

  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
}


void loop() {

  // ---- Serial handling  ----
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      inputLine[inputPos] = '\0';
      processCommand(inputLine);
      inputPos = 0;
    } else if (c != '\r' && inputPos < sizeof(inputLine) - 1) {
      inputLine[inputPos++] = c;
    }
  }

  // ---- timed update ----
  if (millis() - lastUpdate >= updateInterval) {
    lastUpdate = millis();

    now = rtc.now();
    uint8_t seconds = now.second();

    uint8_t tens = seconds / 10;
    uint8_t ones = seconds % 10;

    FastLED.clear();

    display_digit(1, tens, CRGB::Red);
    display_digit(0, ones, CRGB::Red);

    FastLED.show();
  }
}



void processCommand(char *cmd) {
  // Find first space (command separator)
  char *arg = strchr(cmd, ' ');

  if (arg) {
    *arg = '\0';   // terminate command
    arg++;         // point to arguments
  }

  if (strcmp(cmd, "SET") == 0) {
    handle_set(arg);
  }
  else if (strcmp(cmd, "GET") == 0) {
    handle_get();
  }
  else {
    Serial.println("Unknown command");
  }
}

void handle_set(char *arg) {
  if (!arg || strlen(arg) < 19) {
    Serial.println("Bad SET length");
    return;
  }

  // Expected: YYYY-MM-DD HH:MM:SS
  if (!isdigit(arg[0])  || !isdigit(arg[5])  || !isdigit(arg[8]) ||
      !isdigit(arg[11]) || !isdigit(arg[14]) || !isdigit(arg[17])) {
    Serial.println("SET contains non-numeric fields");
    return;
  }

  int year   = atoi(arg + 0);
  int month  = atoi(arg + 5);
  int day    = atoi(arg + 8);
  int hour   = atoi(arg + 11);
  int minute = atoi(arg + 14);
  int second = atoi(arg + 17);

  if (month < 1 || month > 12 ||
      day   < 1 || day   > 31 ||
      hour  > 23 ||
      minute > 59 ||
      second > 59) {
    Serial.println("SET out of range");
    return;
  }

  rtc.adjust(DateTime(year, month, day, hour, minute, second));
  Serial.println("RTC updated");
}


void handle_get() {
  now = rtc.now();

  Serial.print("TIME ");
  Serial.print(now.year()); Serial.print("-");
  if (now.month() < 10) Serial.print("0");
  Serial.print(now.month()); Serial.print("-");
  if (now.day() < 10) Serial.print("0");
  Serial.print(now.day()); Serial.print(" ");

  if (now.hour() < 10) Serial.print("0");
  Serial.print(now.hour()); Serial.print(":");
  if (now.minute() < 10) Serial.print("0");
  Serial.print(now.minute()); Serial.print(":");
  if (now.second() < 10) Serial.print("0");
  Serial.println(now.second());
}





