#include <analogWrite.h>
#include <Arduino.h>
#include "EspMQTTClient.h"
#include "ft_ESP32_IOobjects.h"

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
String activationTopics = "SwimmingPixel/" + clientname + "/Activation";
String controlTopics = "SwimmingPixel/" + clientname + "/Control";
String speedTopics = "SwimmingPixel/" + clientname + "/Speed";
enum direction{ stop=0, up, down, right, left };
String str_dir[] = { "stop", "up", "down", "right", "left"};
int controlDir = stop;
int speed = 0;

CMotor mMotor0(0);	//motor-object, represents right motor
CMotor mMotor1(1);	//motor-object, represents left motor

void setup() {
  
  Serial.begin(115200);
  Serial.println(clientname);
  Serial.println(activationTopics);
  Serial.println(speedTopics);
  Serial.println(controlTopics);
  // If ESP inactive after 5 seconds publish 0 as last will 
  client.setKeepAlive(5);
  client.enableLastWillMessage(activationTopics.c_str(), "0", true);
  client.enableDebuggingMessages();
}

enum Motor_Port {Motor_0, Motor_1};

void loop() {
  client.loop();
  if (client.isMqttConnected()){
    switch (controlDir)
      {
      case stop:
        mMotor0.setValues(0,0);
        mMotor1.setValues(0,0);
        break;
      case up:
        mMotor0.setValues(0,speed);
        mMotor1.setValues(0,speed);
        break;
      case down:
        mMotor0.setValues(1,speed);
        mMotor1.setValues(1,speed);
        break;
      case right:
        mMotor0.setValues(0,speed);
        mMotor1.setValues(1,speed);
        break;
      case left:
        mMotor0.setValues(1,speed);
        mMotor1.setValues(0,speed);
        break;
      default:
        break;
      }
  }
}

void onSpeedCallback(const String& message)
{
  speed = message.toInt();
}

void onControlCallback(const String& message)
{
  controlDir = message.toInt();
}

void onConnectionEstablished()
{
  // If ESP Active and Connected publish 1 and retain the message
  client.publish(activationTopics, "1", true);
  client.subscribe(speedTopics, onSpeedCallback);
  client.subscribe(controlTopics, onControlCallback);
}