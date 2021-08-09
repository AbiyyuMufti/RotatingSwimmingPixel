#include <analogWrite.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
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

//display object with reset-pin
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

CMotor mMotor0(0);	//motor-object, represents right motor
CMotor mMotor1(1);	//motor-object, represents left motor

int buttonState;

void setup() {
  Serial.begin(115200);
  pinMode(35, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.print("SSID: ");
  display.println(SSID);
  display.print("BROKER: ");
  display.println(BROKER);
  display.display();
  delay(5000);
  display.clearDisplay();
  display.display();
  delay(1000);
  // If ESP inactive after 5 seconds publish 0 as last will 
  client.setKeepAlive(5);
  client.enableLastWillMessage(activationTopics.c_str(), "0", true);
  // client.enableDebuggingMessages();

}

enum Motor_Port {Motor_0, Motor_1};

void controlRoutine(){
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

bool connectionOk()
{
  static bool onceWifi = false;
  static bool onceMQTT = false;
  static bool onceShow = false;
  static String msgs;  
  bool ret = true;
  if(!client.isConnected()){
    ret = false;
    if (!client.isWifiConnected())
    {
      if(!onceWifi) {msgs = "WIFI NOT CONNECTED\n";}
      onceWifi = true;
    }
    if (!client.isMqttConnected())
    {
      if(!onceMQTT) {msgs += "MQTT NOT CONNECTED\n";}
      onceMQTT = true;
    }
    onceShow = false;
  }
  else
  {
    if(!onceShow) {
      Serial.println("print once ok");
      onceMQTT = false;
      onceWifi = false;
      msgs = "CONNECTION OK!";
      onceShow = true;
    }
  }

  static bool counterStarted = false;
  buttonState = digitalRead(35);
  if(buttonState == LOW || counterStarted){
    static long lastTime = millis();
    Serial.println(msgs);
    if(!counterStarted) {
      lastTime = millis();
      display.setCursor(0,0);
      display.setTextColor(WHITE);
      display.println(msgs);
      display.display();
    }
    counterStarted = true;
    if (millis() - lastTime > 5000)
    {
      display.clearDisplay();
      lastTime = millis();
      counterStarted = false;
      display.display();
    }
  }
  return ret;
}

void loop() {
  client.loop();
  if(connectionOk()){
    controlRoutine();
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