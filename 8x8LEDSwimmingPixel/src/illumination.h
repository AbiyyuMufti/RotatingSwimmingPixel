#ifndef __ILLUMINATION_H__
#define __ILLUMINATION_H__
#include <Arduino.h>
#include <Wire.h>
#include <FastLED_NeoMatrix.h>
#include <ArduinoJson.h>


#define WIDTH 8
#define HEIGHT 8
#define NUM_LEDS (WIDTH * HEIGHT)
#define MILL_AMPS 500

enum LEDState
{
	LightOff = 0,
    SingleColor,
    Gradient,
	TextGenerator,
    Random
};

extern CRGBArray<64> ledArray;
extern FastLED_NeoMatrix matrix;

typedef void (*LIGHT_SHOW)(JsonDocument&);

void turnOffLight();
void tapSingleColor(JsonDocument& doc);
// void gradientColor(JsonDocument& doc);
void DimBright();
void TheatherChase();
void GradientColor();
void mozart();
void rainbow_colorfull();
void meteor();
void GenericRainbow();
#endif // __ILLUMINATION_H__