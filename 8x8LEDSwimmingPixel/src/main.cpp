#include <Arduino.h>
#include "EspMQTTClient.h"

#define ESPPOSITION 1
#define BROKER "test.mosquitto.org"
#define SSID "FRITZ!Box 7590 VL"
#define PASS "56616967766283031728"
#define PORT 1883


String clientnames[] = {"1_1", "2_1", "3_1", "4_1", 
                        "1_2", "2_2", "3_2", "4_2",
                        "1_3", "2_3", "3_3", "4_3",
                        "1_4", "2_4", "3_4", "4_4"};

String clientname = clientnames[ESPPOSITION-1];
EspMQTTClient client(SSID, PASS, BROKER, clientname.c_str());

String activationTopics = "SwimmingPixel/8x8LED/" + clientname + "/Activation";
String iluminationTopics = "SwimmingPixel/8x8LED/" + clientname + "/Ilumination";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  client.setKeepAlive(5);
  client.enableLastWillMessage(activationTopics.c_str(), "0", true);
}

void loop() {
  client.loop();
}

void onIluminationCallback(const String& message)
{
}

void onConnectionEstablished()
{
  // If ESP Active and Connected publish 1 and retain the message
  client.publish(activationTopics, "1", true);
  client.subscribe(iluminationTopics, onIluminationCallback);
}