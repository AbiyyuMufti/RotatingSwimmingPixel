#include <Arduino.h>
#include <EspMQTTClient.h>
#include <ArduinoJson.h>
#include "illumination.h"
#include <FastLED_NeoMatrix.h>

#define ESPPOSITION 1
#define BROKER "test.mosquitto.org"
// #define SSID "FRITZ!Box 7590 VL"
// #define PASS "56616967766283031728"
#define SSID "AbiyyuMufti"
#define PASS "MaMh1234"
#define PORT 1883
#define MAX_POWER_MILLIAMPS 500
#define PIN_LED 5

String clientnames[] = {"1_1", "2_1", "3_1", "4_1", 
                        "1_2", "2_2", "3_2", "4_2",
                        "1_3", "2_3", "3_3", "4_3",
                        "1_4", "2_4", "3_4", "4_4"};

String clientname = clientnames[ESPPOSITION-1];
EspMQTTClient client(SSID, PASS, BROKER, clientname.c_str());

String activationTopics = "SwimmingPixel/8x8LED/" + clientname + "/Activation";
String iluminationTopics = "SwimmingPixel/8x8LED/" + clientname + "/Illumination";
StaticJsonDocument<255> receivedMsg;

LIGHT_SHOW led_routine = tapSingleColor;
// void (*LIGHT_SHOW)(const JsonDocument& _doc)
CRGBArray<64> ledArray;
FastLED_NeoMatrix matrix(ledArray, WIDTH, HEIGHT, 1, 1, 
  NEO_MATRIX_TOP + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE);

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<NEOPIXEL, PIN_LED>(ledArray, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(PIN_LED, MILL_AMPS);
  matrix.begin();
  matrix.setBrightness(100);
  client.setKeepAlive(5);
  client.enableLastWillMessage(activationTopics.c_str(), "0", true);
  client.enableDebuggingMessages(true);
}

void loop() {
  client.loop();
  // led_routine(receivedMsg);
  // DimBright();
  GenericRainbow();
}

void onIluminationCallback(const String& message)
{
	deserializeJson(receivedMsg, message);
  int cmd = receivedMsg["cmd"];
  switch (cmd)
  {
  case LightOff:
    // led_routine = turnOffLight;
    break;
  case SingleColor:
    led_routine = tapSingleColor;
    break;
  case Gradient:
    break;
  case TextGenerator:
    break;
  // case LightShow1:
  //   break;
  // case LightShow2:
  //   break;
  // case LightShow3:
  //   break;
  case Random:
    break;
  default:
    break;
  }
  
}

void onConnectionEstablished()
{
  client.publish(activationTopics, "1", true);
  client.subscribe(iluminationTopics, onIluminationCallback);
}