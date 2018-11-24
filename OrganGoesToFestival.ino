#include <Adafruit_NeoPixel.h>
#define NUM_LEDS 60
/* Pin assignments
    ===Colors===
    Red 2
    yellow 3
    green 4
    blue 5
    ===Additions===
    6 speed +
    7 Speed -
    ============
    22 r +
    23 R -
    24 G+
    25 G-
    26 B+
    27 B-
*/

#define PIN 13
int pinAssignments[18] = {2, 3, 4, 5, 6, 7, 22, 23, 24, 25, 26, 27};
#define speedMod 0
int currPat = 0;
int red = 0;
int blue = 0;
int green = 0;
int speedVal = 0;
int rate = 5;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 5; i++) { // uno doesn't have all the pins
    pinMode(pinAssignments[i], INPUT_PULLUP);
  }
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  //  red = random(255);
  //  blue = random(100);
  //  green = 0;
  //  speedVal = random(0, 60);
  selectPattern();

}

void readKeys() {
  if (digitalRead(pinAssignments[4] == LOW)) {
    if (checkIt(speedVal)) {
      speedVal += rate;
    }
  }
  if (digitalRead(pinAssignments[5] == LOW)) {
    if (speedVal > 0) {
      speedVal -= rate;
    }
  }
  if (digitalRead(pinAssignments[6] == LOW)) {
    if (checkIt(red)) {
      red += rate;
    }
  }
  if (digitalRead(pinAssignments[7] == LOW)) {
    if (red > 0) {
      red -= rate;
    }
  }
  if (digitalRead(pinAssignments[8] == LOW)) {
    if (checkIt(green)) {
      green += rate;
    }
  }
  if (digitalRead(pinAssignments[9] == LOW)) {
    if (green > 0) {
      green -= rate;
    }
  }
  if (digitalRead(pinAssignments[10] == LOW)) {
    if (checkIt(green)) {
      green += rate;
    }
  }
  if (digitalRead(pinAssignments[11] == LOW)) {
    if (green > 0) {
      green -= rate;
    }
  }
}
boolean checkIt(int which) {
  if (which > 255) {
    return false;
  } else {
    return true;
  }
}
void selectPattern() {
  switch (readPlungers()) {
    case 0:
      NewKITT(8, speedVal, speedVal);
      break;
    case 1:
      meteorRain(20, 64, true, speedVal);
      break;
    case 2:
      SnowSparkle( speedVal);
      break;
    case 3:
      Fire(speedVal, 120, 15);
      break;
  }
}
int readPlungers() {
  for (int i = 0; i < 4; i++) {
    if (!analogRead(pinAssignments[i])) {
      return (i);
    }
  }
}
void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUM_LEDS];
  int cooldown;

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);

    if (cooldown > heat[i]) {
      heat[i] = 0;
    } else {
      heat[i] = heat[i] - cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if ( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160, 255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for ( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature / 255.0) * 191);

  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252

  // figure out which third of the spectrum we're in:
  if ( t192 > 0x80) {                    // hottest
    setPixel(Pixel, red, 0, heatramp);
  } else if ( t192 > 0x40 ) {            // middle
    setPixel(Pixel, red, heatramp, blue);
  } else {                               // coolest
    setPixel(Pixel, heatramp, green, 0);
  }
}
void SnowSparkle( int SpeedDelay) {
  setAll(red, green, blue);

  int Pixel = random(NUM_LEDS);
  setPixel(Pixel, 0xff, 0xff, 0xff);
  showStrip();
  delay(SpeedDelay);
  setPixel(Pixel, red, green, blue);
  showStrip();
  delay(SpeedDelay);
}
void meteorRain(byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {
  setAll(0, 0, 0);

  for (int i = 0; i < NUM_LEDS + NUM_LEDS; i++) {


    // fade brightness all LEDs one step
    for (int j = 0; j < NUM_LEDS; j++) {
      if ( (!meteorRandomDecay) || (random(10) > 5) ) {
        fadeToBlack(j, meteorTrailDecay );
      }
    }

    // draw meteor
    for (int j = 0; j < meteorSize; j++) {
      if ( ( i - j < NUM_LEDS) && (i - j >= 0) ) {
        setPixel(i - j, red, green, blue);
      }
    }

    showStrip();
    delay(SpeedDelay);
  }
}

void fadeToBlack(int ledNo, byte fadeValue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  uint32_t oldColor;
  uint8_t r, g, b;
  int value;

  oldColor = strip.getPixelColor(ledNo);
  r = (oldColor & 0x00ff0000UL) >> 16;
  g = (oldColor & 0x0000ff00UL) >> 8;
  b = (oldColor & 0x000000ffUL);

  r = (r <= 10) ? 0 : (int) r - (r * fadeValue / 256 + random(5));
  g = (g <= 10) ? 0 : (int) g - (g * fadeValue / 256 + random(50));
  b = (b <= 10) ? 0 : (int) b - (b * fadeValue / 256 + random(50));

  strip.setPixelColor(ledNo, r, g, b);
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[ledNo].fadeToBlackBy( fadeValue );
#endif
}

void NewKITT(int EyeSize, int SpeedDelay, int ReturnDelay) {

  OutsideToCenter(EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(EyeSize, SpeedDelay, ReturnDelay);

}

void CenterToOutside(int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = ((NUM_LEDS - EyeSize) / 2); i >= 0; i--) {
    setAll(0, 0, 0);

    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);

    setPixel(NUM_LEDS - i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS - i - j, red, green, blue);
    }
    setPixel(NUM_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);

    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void OutsideToCenter(int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = 0; i <= ((NUM_LEDS - EyeSize) / 2); i++) {
    setAll(0, 0, 0);

    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);

    setPixel(NUM_LEDS - i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS - i - j, red, green, blue);
    }
    setPixel(NUM_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);

    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}


void showStrip() {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.show();
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  FastLED.show();
#endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
#endif
}

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}
