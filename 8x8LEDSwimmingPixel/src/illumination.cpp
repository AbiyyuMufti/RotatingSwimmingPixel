#include "illumination.h"

void turn_off_led() 
{
    EVERY_N_MILLISECONDS(50){
        matrix.clear();
        matrix.show();
    }
    
}

void single_color() 
{
    EVERY_N_MILLISECONDS(100){
        matrix.fillScreen(matrix.Color(L_Color.red,L_Color.green,L_Color.blue));
        matrix.show();
    }
}

void dim_bright() 
{
    static int br = 0;
    static int steady_ctr = 0;
    enum rampDir{ rampUp, rampDown, steady_on, steady_off};
    static int light_dir = rampUp;  
    EVERY_N_MILLISECONDS(100){
        matrix.fillScreen(matrix.Color(L_Color.red,L_Color.green,L_Color.blue));
        matrix.show();
        switch (light_dir)
        {
        case rampUp:
            br++;
            if (br >= 50) light_dir = steady_on;
            break;
        case rampDown:
            br--;
            if (br <= 0) light_dir = steady_off;
            break;
        case steady_on:
            steady_ctr++;
            if (steady_ctr >= 10){ 
                light_dir = rampDown;
                steady_ctr = 0;
            }
            break;
        case steady_off:
            br = 0;
            steady_ctr++;
            if (steady_ctr >= 10){ 
                light_dir = rampUp;
                steady_ctr = 0;
            }
        default:
            break;
        }
        matrix.setBrightness(br);
    }
}

float max3point(float a, float b, float c) {
	if (a > b)
	{
		if (a > c) return a;
		else return c;
	}
	else
	{
		if (b > c) return b;
		else return c;
	}
}

float min3point(float a, float b, float c) {
	if (a < b)
	{
		if (a < c) return a;
		else return c;
	}
	else
	{
		if (b < c) return b;
		else return c;
	}
}


void calculateHSV(const byte& r, const byte& g, const byte& b, uint32_t& h, uint8_t& s, uint8_t& v) {
	float R = (float)r / 255, G = (float)g / 255, B = (float)b / 255;
	float cmin = min3point(R, G, B);
	float cmax = max3point(R, G, B);
	float diff = cmax - cmin;
	int hue = 0;
	if (diff == 0) { hue = 0; }
	else
	{
		if (cmax == R) { hue = 60 * ((G - B) / diff); }
		if (cmax == G) { hue = 60 * (((B - R) / diff) + 2); }
		if (cmax == B) { hue = 60 * (((R - G) / diff) + 4); }
	}
	if (hue < 0) { hue += 360; }
	v = cmax * 255;
	s = (v == 0) ? 0 : 255 * diff / cmax;
	h = map(hue, 0, 360, 0, 65536);
}


void theather_chase() 
{
    // theater chase
	static byte steps = 0;
    EVERY_N_MILLISECONDS(75){
        matrix.clear();
		for (byte c = steps; c < 64; c += 3)
		{
			ledArray[c] = CRGB(L_Color.red,L_Color.green,L_Color.blue);
		}
		steps++;
		if (steps >= 3) { steps = 0; }
		matrix.show();
    }
}

void gradient_color() 
{
    // Gradient Color
	uint32_t hue;
	uint8_t saturation, value;
	static int32_t steps = 0;
	static bool dir = false;
	calculateHSV(L_Color.red,L_Color.green,L_Color.blue, hue, saturation, value);
	EVERY_N_MILLISECONDS(20)
	{
		for (int i = -32; i < NUM_LEDS - 32; i++)
		{
			uint32_t pixelHue = steps + hue + i;
			ledArray[i + 32] = CHSV(pixelHue, saturation, value);
		}
		matrix.show();
		if (dir)
		{
			steps++;
			if (steps >= 32){dir = !dir;}
		}
		else
		{
			steps--;
			if (steps <= -1*32){dir = !dir;}
		}
	} 
}


void setPixel(int r, int g, int b){
    matrix.fillScreen(matrix.Color(r, g, b));
    matrix.show();
}


uint16_t XY(uint8_t x, uint8_t y)
{
	uint16_t i;
	if (y & 0x01) {
		// Odd rows run backwards
		uint8_t reverseX = (WIDTH - 1) - x;
		i = (y * WIDTH) + reverseX;
	}
	else {
		// Even rows run forwards
		i = (y * WIDTH) + x;
	}
	return i;
}

// Utilty function for lightshow 1
void DrawOneFrame(byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
	byte lineStartHue = startHue8;
	for (byte y = 0; y < HEIGHT; y++) {
		lineStartHue += yHueDelta8;
		byte pixelHue = lineStartHue;
		for (byte x = 0; x < WIDTH; x++) {
			pixelHue += xHueDelta8;
			ledArray[XY(x, y)] = CHSV(pixelHue, 255, 255);
		}
	}
}

///////////////////////////////////////////////
// Light Show 1st Pattern : Colorfull drawing rainbow
void rainbow_colorfull()
{
    uint32_t ms = millis();
    int32_t yHueDelta32 = ((int32_t)cos16(ms * (27 / 1)) * (350 / WIDTH));
    int32_t xHueDelta32 = ((int32_t)cos16(ms * (39 / 1)) * (310 / HEIGHT));
    DrawOneFrame(ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
    if (ms < 10000) {
        matrix.setBrightness(scale8(100, (ms * 256) / 10000));
    }
    else {
        matrix.setBrightness(100);
    }
    matrix.show();
}

void shooting_star()
{
    static long last = millis();
    static uint8_t hue;
    static int steps;
    if (millis() - last >= 33)
    {
        // fade everything out
        ledArray.fadeToBlackBy(40);
        // let's set an led value
        ledArray[steps] = CHSV(hue++, 255, 255);
        // now, let's first 20 leds to the top 20 leds, 
        ledArray(NUM_LEDS / 2, NUM_LEDS - 1) = ledArray(NUM_LEDS / 2 - 1, 0);
        steps++;
        if (steps > NUM_LEDS/2){steps = 0;}
        last = millis();
        matrix.show();
    }
}

// Light Show 3rd Pattern: Colorfull and circeling in each pixel
void rainbow_generic()
{
    EVERY_N_MILLIS(8) {
        // draw a generic, no-name rainbow
        static uint8_t starthue = 0;
        fill_rainbow(ledArray , NUM_LEDS, --starthue, 20);
        const int DisplayTime = 10;
        // // Choose which 'color temperature' profile to enable.
        uint8_t secs = (millis() / 1000) % (DisplayTime * 2);
        if (secs < DisplayTime) {
            FastLED.setTemperature(Candle); // first temperature
        //     ledArray[0] = Tungsten100W; // show indicator pixel
        }
        else {
            FastLED.setTemperature(OvercastSky); // second temperature
        //     ledArray[0] = OvercastSky; // show indicator pixel
        }

        // // Black out the LEDs for a few secnds between color changes
        // // to let the eyes and brains adjust
        if ((secs % DisplayTime) < 1) {
            memset8(ledArray, 0, NUM_LEDS * sizeof(CRGB));
        }
        matrix.show();
    }
}

void mozart(){
    byte LED_COUNT = 64;
    clock_t start;
    double D = 0;
    start=clock();
    while(D<30.7)
    {
        gradient_color();
        D=(clock()-start)/(double)CLOCKS_PER_SEC;
    }
    delay(2200);
    setPixel(255, 0, 0);
    delay(2000);
    while(D<65.7)
    {
        gradient_color();
        D=(clock()-start)/(double)CLOCKS_PER_SEC;
    }
    delay(2300);
    setPixel(255, 0, 0);
    delay(2000);
    while(D<82.5)
    {
        gradient_color();
        D=(clock()-start)/(double)CLOCKS_PER_SEC;
    }
    delay(2200);
    setPixel(255, 0, 0);
    delay(2000);
    while(D<135)
    {
    gradient_color();
    D=(clock()-start)/(double)CLOCKS_PER_SEC;
    }
    while(D<143.5)
    {
    dim_bright();
    D=(clock()-start)/(double)CLOCKS_PER_SEC;
    }
    delay(1000);
    while(D<153)
    {
    gradient_color();
    D=(clock()-start)/(double)CLOCKS_PER_SEC;
    }
    delay(100);
    while(D<161)
    {
    dim_bright();
    D=(clock()-start)/(double)CLOCKS_PER_SEC;
    }
    delay(250);
    while(D<169.5)
    {
    gradient_color();
    D=(clock()-start)/(double)CLOCKS_PER_SEC;
    }
    delay(2000);
    while(D<198)                                             
    {
    for(int i=0;i<3;i++)                                    // Dreimal  Roter Wellenfront links<->rechts
    {
        if(D>=198) {break;}
        setPixel(255, 0, 0);
        delay(100);
        turn_off_led();
        delay(100);
        turn_off_led();
        delay(100);
        turn_off_led();
        delay(100);
        turn_off_led();
        delay(100);
        turn_off_led();
        delay(100);
        D=(clock()-start)/(double)CLOCKS_PER_SEC;
    }
    for(int j=0;j<3;j++)                                      // Dreimal Blauer Wellenfront unten <-> oben
    {
        if(D>=198)
            break;
        turn_off_led();
        delay(100);
        turn_off_led();
        delay(100);
        turn_off_led();
        delay(100);
        setPixel(0, 0, 255);
        delay(100);
        turn_off_led();
        delay(100);
        turn_off_led();
        delay(100);
        D=(clock()-start)/(double)CLOCKS_PER_SEC;

    }
    for(int j=0;j<3;j++)                                      // Dreimal Gr??ner Wellenfront links <-> rechts
    {
        if(D>=198)
        {break;}
        setPixel(0, 255, 0);
        delay(100);
        turn_off_led();
        delay(100);
        turn_off_led();
        delay(100);
        turn_off_led();
        delay(100);
        turn_off_led();
        delay(100);
        turn_off_led();
        delay(100);
        D=(clock()-start)/(double)CLOCKS_PER_SEC;
    }
    for(int j=0;j<3;j++)                                      // Dreimal Gelber Wellenfront unten <-> oben
    {
        if(D>=198) {break;}
        turn_off_led();
        delay(100);
        turn_off_led();
        delay(100);
        turn_off_led();
        delay(100);
        setPixel(255,255,0);
        delay(100);
        turn_off_led();
        delay(100);
        turn_off_led();
        delay(100);
        D=(clock()-start)/(double)CLOCKS_PER_SEC;
    }

    }
    for(int i=0; i<LED_COUNT; i++) 
    { 
        ledArray[i] = CRGB(250,250,250);
        // strip.setPixelColor(i, strip.Color(250,250,250));         //  Set pixel's color (in RAM)
        matrix.show();
    }
    delay(2000);
    turn_off_led();
}
