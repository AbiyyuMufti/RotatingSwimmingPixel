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
    TheatherChase,
    GradientColor,
    RainbowColorfull,
    GenericRainbow,
    ShootingStar,
    DimBright,
    WAMozart
};

struct Color{
    byte red;
    byte green;
    byte blue;
};

extern struct Color L_Color;
extern CRGBArray<64> ledArray;
extern FastLED_NeoMatrix matrix;

typedef void (*LIGHT_SHOW)();

void turn_off_led();
void single_color();
void dim_bright();
void theather_chase();
void gradient_color();
void rainbow_colorfull();
void shooting_star();
void rainbow_generic();
void mozart();


#endif // __ILLUMINATION_H__