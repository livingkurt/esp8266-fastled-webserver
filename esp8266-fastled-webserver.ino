/*
   ESP8266 FastLED WebServer: https://github.com/jasoncoon/esp8266-fastled-webserver
   Copyright (C) 2015-2018 Jason Coon

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//#define FASTLED_ALLOW_INTERRUPTS 1
//#define INTERRUPT_THRESHOLD 1
#define FASTLED_INTERRUPT_RETRY_COUNT 0

#include <FastLED.h>
FASTLED_USING_NAMESPACE

extern "C"
{
#include "user_interface.h"
}
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
//#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
//#include <WebSocketsServer.h>
#include <FS.h>
#include <EEPROM.h>
//#include <IRremoteESP8266.h>
#include "GradientPalettes.h"

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#include "Field.h"

//#define RECV_PIN D4
//IRrecv irReceiver(RECV_PIN);

//#include "Commands.h"

ESP8266WebServer webServer(80);
//WebSocketsServer webSocketsServer = WebSocketsServer(81);
ESP8266HTTPUpdateServer httpUpdateServer;

#include "FSBrowser.h"

// #define MY_NAME    "Thousand Petal Lotus"
#define MY_NAME "String Lights" //burn kitchen
//#define MY_NAME    "Garage Side"
//#define MY_NAME    "Ground Floor"
//#define MY_NAME    "1st Floor Roof"
// #define MY_NAME    "Wreath"
// #define MY_NAME    "Bed"
// #define MY_NAME    "Fire Circle"
// #define MY_NAME    "SmokeBreak"
// #define MY_NAME    "Fuel Depot"
// #define MY_NAME    "Backup"
//#define MY_NAME    "Safety"
//#define MY_NAME    "Phasing"

//MOM'S LIGHT STRINGS
// #define MY_NAME    "kitchen stove top right" //mom's kitchen (54 leds, SYSTEM_MAX_LEDS = 100)
// #define MY_NAME    "kitchen stove top left" //mom's kitchen (28 leds, SYSTEM_MAX_LEDS = 100)
// #define MY_NAME    "kitchen refrigerator top" //mom's kitchen (49 leds, SYSTEM_MAX_LEDS = 100)
// #define MY_NAME    "kitchen refrigerator bottom" //mom's kitchen (49 leds, SYSTEM_MAX_LEDS = 100)

#define DATA_PIN D2
// #define DATA_PIN2   D3

// if(MY_NAME == "Wreath"){
// #define SYSTEM_MAX_LEDS       144
// #define LED_TYPE              WS2812B //LED Strip
// #define COLOR_ORDER           GRB //LED Strip
// } else if(MY_NAME == "Lotus"){
// #define SYSTEM_MAX_LEDS       60
// #define LED_TYPE              WS2812B //LED Strip
// #define COLOR_ORDER           GRB //LED Strip
// } else if(MY_NAME == "Ground Floor" || MY_NAME == "1st Floor Roof" || MY_NAME == "Garage Side"){
//   #define SYSTEM_MAX_LEDS       8 * 50
//   #define LED_TYPE              WS2811 //LED String
//   #define COLOR_ORDER           RGB // LED String
// } else if(MY_NAME == "Phasing"){
//   #define SYSTEM_MAX_LEDS       186
//   #define LED_TYPE              WS2812B //LED Strip
//   #define COLOR_ORDER           GRB //LED Strip
// } else if(MY_NAME == "Fire Circle"){
#define SYSTEM_MAX_LEDS 9 * 50
#define LED_TYPE WS2811 //LED String
#define COLOR_ORDER RGB // LED String
// } else if (MY_NAME == "SmokeBreak"){
// #define SYSTEM_MAX_LEDS       5 * 50
// #define LED_TYPE              WS2811 //LED String
// #define COLOR_ORDER           RGB // LED String
// }
// } else if(MY_NAME == "Kitchen" || MY_NAME == "Backup" || MY_NAME == "Fuel Depot"){
// #define SYSTEM_MAX_LEDS       4 * 50
// #define LED_TYPE              WS2811 //LED String
// #define COLOR_ORDER           RGB // LED String

// } else if(MY_NAME == "Mom's Kitchen Light Strings"){
// #define SYSTEM_MAX_LEDS       100
// #define LED_TYPE              WS2813 //Mom's house LED Strips
// #define COLOR_ORDER           GRB //LED Strip
// } else if(MY_NAME == "kitchen stove top right"){
// #define SYSTEM_MAX_LEDS       54
// #define LED_TYPE              WS2813 //Mom's house LED Strips
// #define COLOR_ORDER           GRB //LED Strip
// } else if(MY_NAME == "kitchen stove top left"){
// #define SYSTEM_MAX_LEDS       28
// #define LED_TYPE              WS2813 //Mom's house LED Strips
// #define COLOR_ORDER           GRB //LED Strip
// } else if(MY_NAME == "kitchen refrigerator top"){
// #define SYSTEM_MAX_LEDS       49
// #define LED_TYPE              WS2813 //Mom's house LED Strips
// #define COLOR_ORDER           GRB //LED Strip
// }

//#define HALF_SYSTEM_MAX_LEDS  SYSTEM_MAX_LEDS / 2

//#define NUM_LEDS            60 // Lotus
//#define NUM_LEDS            426 // Bed
//#define NUM_LEDS            6 * 50 // First Floor
//#define NUM_LEDS              5 * 50 // Ground Floor
//#define NUM_LEDS            144 - 16// Wreath
#define NUM_LEDS SYSTEM_MAX_LEDS //try to sync everything

#define HALF_LEDS NUM_LEDS / 2
#define HALF_SYSTEM_MAX_LEDS HALF_LEDS

#define MILLI_AMPS 4000 // IMPORTANT: set the max milli-Amps of your power supply (4A = 4000mA) // standard usb
//#define MILLI_AMPS            30000 // 30 Amps

#define VOLTS 5 // IMPORTANT: LED Strip
//#define VOLTS           12 // IMPORTANT: LED String

#define FRAMES_PER_SECOND 120 // here you can control the speed. With the Access Point / Web Server the animations run a bit slower.

const bool apMode = false;

// #include "Secrets.h" // this file is intentionally not included in the sketch, so nobody accidentally commits their secret information.
// create a Secrets.h file with the following:

// AP mode password
// const char WiFiAPPSK[] = "";

// Wi-Fi network to connect to (if not in AP mode)
// char* ssid = "Laniakea";
// char* password = "2145467756";

// char ssid;
// char password;

// // AP mode password
const char WiFiAPPSK[] = "";

// // Wi-Fi network to connect to (if not in AP mode)
// char *ssid = "";
// char *password = "";

const char *ssid;
const char *password;

CRGB leds[NUM_LEDS];

const uint8_t brightnessCount = 5;
uint8_t brightnessMap[brightnessCount] = {16, 32, 64, 128, 255};
uint8_t brightnessIndex = 0;

// ten seconds per color palette makes a good demo
// 20-120 is better for deployment
uint8_t secondsPerPalette = 10;

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
uint8_t cooling = 50;

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
uint8_t sparking = 120;

uint8_t speed = 30;

///////////////////////////////////////////////////////////////////////

// Forward declarations of an array of cpt-city gradient palettes, and
// a count of how many there are.  The actual color palette definitions
// are at the bottom of this file.
extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];

uint8_t gCurrentPaletteNumber = 0;

CRGBPalette16 gCurrentPalette(CRGB::Black);
CRGBPalette16 gTargetPalette(gGradientPalettes[0]);

CRGBPalette16 IceColors_p = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);

uint8_t currentPatternIndex = 0; // Index number of which pattern is current
uint8_t autoplay = 0;

uint8_t autoplayDuration = 10;
unsigned long autoPlayTimeout = 0;

uint8_t currentPaletteIndex = 0;

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

CRGB solidColor = CRGB::Blue;

// scale the brightness of all pixels down
void dimAll(byte value)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i].nscale8(value);
  }
}

typedef void (*Pattern)();
typedef Pattern PatternList[];
typedef struct
{
  Pattern pattern;
  String name;
} PatternAndName;
typedef PatternAndName PatternAndNameList[];

#include "Twinkles.h"
#include "TwinkleFOX.h"

// List of patterns to cycle through.  Each is defined as a separate function below.

PatternAndNameList patterns = {
    {pride, "Pride"},
    //  { prideScaled,            "Pride Scaled" },
    {colorWaves, "Color Waves"},

    // twinkle patterns
    {rainbowTwinkles, "Rainbow Twinkles"},
    {snowTwinkles, "Snow Twinkles"},
    {cloudTwinkles, "Cloud Twinkles"},
    {incandescentTwinkles, "Incandescent Twinkles"},

    // TwinkleFOX patterns
    {retroC9Twinkles, "Retro C9 Twinkles"},
    {redWhiteTwinkles, "Red & White Twinkles"},
    {redWhiteGreenTwinkles, "Red, White, & Green Twinkles"},
    {blueWhiteTwinkles, "Blue & White Twinkles"},
    {redGreenWhiteTwinkles, "Red, Green & White Twinkles"},
    {whiteTwinkles, "White Twinkles"},
    {fairyLightTwinkles, "Fairy Light Twinkles"},
    {snow2Twinkles, "Snow 2 Twinkles"},
    {hollyTwinkles, "Holly Twinkles"},
    {iceTwinkles, "Ice Twinkles"},
    {partyTwinkles, "Party Twinkles"},
    {forestTwinkles, "Forest Twinkles"},
    {lavaTwinkles, "Lava Twinkles"},
    {fireTwinkles, "Fire Twinkles"},
    {cloud2Twinkles, "Cloud 2 Twinkles"},
    {oceanTwinkles, "Ocean Twinkles"},
    //
    //  { rainbow,                "Rainbow" },
    //  { rainbowWithGlitter,     "Rainbow With Glitter" },
    {rainbowSolid, "Solid Rainbow"},
    {confetti, "Confetti"},
    {sinelon, "Sinelon"},
    {bpm, "Beat"},
    {juggle, "Juggle"},
    {fire, "Fire"},
    {water, "Water"},

    {showSolidColor, "Solid Color"}};

const uint8_t patternCount = ARRAY_SIZE(patterns);

typedef struct
{
  CRGBPalette16 palette;
  String name;
} PaletteAndName;
typedef PaletteAndName PaletteAndNameList[];

const CRGBPalette16 palettes[] = {
    RainbowColors_p,
    RainbowStripeColors_p,
    CloudColors_p,
    LavaColors_p,
    OceanColors_p,
    ForestColors_p,
    PartyColors_p,
    HeatColors_p};

const uint8_t paletteCount = ARRAY_SIZE(palettes);

const String paletteNames[paletteCount] = {
    "Rainbow",
    "Rainbow Stripe",
    "Cloud",
    "Lava",
    "Ocean",
    "Forest",
    "Party",
    "Heat",
};

#include "Fields.h"

void (*resetFunc)(void) = 0;

void setup()
{
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  Serial.begin(115200);
  delay(100);
  Serial.setDebugOutput(true);

  // if(WS2815){
  //   FastLED.addLeds<LED_TYPE, DATA_PIN2, COLOR_ORDER>(leds, NUM_LEDS);         // for WS2815 (Neopixel)
  // }

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS); // for WS2812 (Neopixel)
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS); // for APA102 (Dotstar)
  FastLED.setDither(false);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(brightness);
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, MILLI_AMPS);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();

  EEPROM.begin(512);
  loadSettings();

  FastLED.setBrightness(brightness);

  //  irReceiver.enableIRIn(); // Start the receiver

  Serial.println();
  Serial.print(F("Heap: "));
  Serial.println(system_get_free_heap_size());
  Serial.print(F("Boot Vers: "));
  Serial.println(system_get_boot_version());
  Serial.print(F("CPU: "));
  Serial.println(system_get_cpu_freq());
  Serial.print(F("SDK: "));
  Serial.println(system_get_sdk_version());
  Serial.print(F("Chip ID: "));
  Serial.println(system_get_chip_id());
  Serial.print(F("Flash ID: "));
  Serial.println(spi_flash_get_id());
  Serial.print(F("Flash Size: "));
  Serial.println(ESP.getFlashChipRealSize());
  Serial.print(F("Vcc: "));
  Serial.println(ESP.getVcc());
  Serial.println();

  SPIFFS.begin();
  {
    Serial.println("SPIFFS contents:");

    Dir dir = SPIFFS.openDir("/");
    while (dir.next())
    {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), String(fileSize).c_str());
    }
    Serial.printf("\n");
  }

  //disabled due to https://github.com/jasoncoon/esp8266-fastled-webserver/issues/62
  //initializeWiFi();
  wifi_setup();
  run_server();
  autoPlayTimeout = millis() + (autoplayDuration * 1000);
}

void sendInt(uint8_t value)
{
  sendString(String(value));
}

void sendString(String value)
{
  webServer.send(200, "text/plain", value);
}

void broadcastInt(String name, uint8_t value)
{
  String json = "{\"name\":\"" + name + "\",\"value\":" + String(value) + "}";
  //  webSocketsServer.broadcastTXT(json);
}

void broadcastString(String name, String value)
{
  String json = "{\"name\":\"" + name + "\",\"value\":\"" + String(value) + "\"}";
  //  webSocketsServer.broadcastTXT(json);
}

void loop()
{
  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy(random(65535));

  //  dnsServer.processNextRequest();
  //  webSocketsServer.loop();
  webServer.handleClient();

  //  handleIrInput();

  if (power == 0)
  {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    // FastLED.delay(15);
    return;
  }

  static bool hasConnected = false;
  EVERY_N_SECONDS(1)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      //      Serial.printf("Connecting to %s\n", ssid);
      hasConnected = false;
    }
    else if (!hasConnected)
    {
      hasConnected = true;
      Serial.print("Connected! Open http://");
      Serial.print(WiFi.localIP());
      Serial.println(" in your browser");
    }
  }

  // EVERY_N_SECONDS(10) {
  //   Serial.print( F("Heap: ") ); Serial.println(system_get_free_heap_size());
  // }

  // change to a new cpt-city gradient palette
  EVERY_N_SECONDS(secondsPerPalette)
  {
    gCurrentPaletteNumber = addmod8(gCurrentPaletteNumber, 1, gGradientPaletteCount);
    gTargetPalette = gGradientPalettes[gCurrentPaletteNumber];
  }

  EVERY_N_MILLISECONDS(40)
  {
    // slowly blend the current palette to the next
    nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 8);
    gHue++; // slowly cycle the "base color" through the rainbow
  }

  if (autoplay && (millis() > autoPlayTimeout))
  {
    adjustPattern(true);
    autoPlayTimeout = millis() + (autoplayDuration * 1000);
  }

  // Call the current pattern function once, updating the 'leds' array
  patterns[currentPatternIndex].pattern();

  FastLED.show();

  // insert a delay to keep the framerate modest
  //  FastLED.delay(1000 / FRAMES_PER_SECOND);
}