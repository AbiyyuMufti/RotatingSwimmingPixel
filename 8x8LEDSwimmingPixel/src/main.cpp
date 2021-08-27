#include <Arduino.h>
#include <EspMQTTClient.h>
#include <ArduinoJson.h>
#include "illumination.h"
#include <FastLED_NeoMatrix.h>

#define ESPPOSITION 3
// #define BROKER "test.mosquitto.org"
#define BROKER "192.168.188.100"
// #define SSID "FRITZ!Box 7590 VL"
// #define PASS "56616967766283031728"
#define SSID "JuergenWalter"
#define PASS "44873763559236747268"
// #define SSID "AbiyyuMufti"
// #define PASS "MaMh1234"
#define PORT 1883

#define MAX_POWER_MILLIAMPS 500
#define PIN_LED 5

int LED_Brightness = 50;

String clientnames[] = {"1_1", "1_2", "1_3", "1_4", 
                        "2_1", "2_2", "2_3", "2_4", 
                        "3_1", "3_2", "3_3", "3_4",
                        "4_1", "4_2", "4_3", "4_4"};

String clientname = clientnames[ESPPOSITION-1];
EspMQTTClient client(SSID, PASS, BROKER, clientname.c_str());

String activationTopics = "SwimmingPixel/Lightshow/" + clientname + "/Activation";
String iluminationTopics = "SwimmingPixel/Lightshow/" + clientname + "/Control";

LIGHT_SHOW led_routine = turn_off_led;
CRGBArray<64> ledArray;
FastLED_NeoMatrix matrix(ledArray, WIDTH, HEIGHT, 1, 1, 
  NEO_MATRIX_TOP + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE);

struct Color L_Color;


void setup() {
  L_Color.red = 200; L_Color.blue = 0; L_Color.green = 0;
  Serial.begin(115200);
  FastLED.addLeds<NEOPIXEL, PIN_LED>(ledArray, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(PIN_LED, MILL_AMPS);
  matrix.begin();
  matrix.setBrightness(LED_Brightness);
  client.setKeepAlive(1);
  client.enableLastWillMessage(activationTopics.c_str(), "0", true);
  client.enableDebuggingMessages(true);
}

void loop() {
  client.loop();
  led_routine();
}

void onIluminationCallback(const String& message)
{
  StaticJsonDocument<255> receivedMsg;
	deserializeJson(receivedMsg, message);
  static int old_cmd = 0;
  int cmd = receivedMsg["mode"];
  L_Color.red = receivedMsg["r"];
  L_Color.green = receivedMsg["g"];
  L_Color.blue = receivedMsg["b"];

  if (cmd != old_cmd)
  {
    matrix.setBrightness(LED_Brightness);
    old_cmd = cmd;
    switch (cmd)
    {
    case LightOff:
      led_routine = turn_off_led;
      break;
    case SingleColor:
      led_routine = single_color;
      break;
    case TheatherChase:
      led_routine = theather_chase;
      break;
    case GradientColor:
      led_routine = gradient_color;
      break;
    case RainbowColorfull:
      led_routine = rainbow_colorfull;
      break;
    case GenericRainbow:
      led_routine = rainbow_generic;
      break;
    case ShootingStar:
      led_routine = shooting_star;
      break;
    case DimBright:
      led_routine = dim_bright;
      break;
    default:
      break;
    }
  }
}

void onConnectionEstablished()
{
  client.publish(activationTopics, "1", true);
  client.subscribe(iluminationTopics, onIluminationCallback);
}