#include "Effects.h"

int dir = 1;

const int TotalSteps = 50;

// fades from black to colour, then back again
void heartbeat(Adafruit_NeoPixel *strip, uint8_t red, uint8_t green, uint8_t blue) {
  uint8_t r, g, b;
  static int Index = 0;
  
  // work out the new intensity
  switch (dir) {
    case 1: // increasing brightness
        r = (red * Index) / TotalSteps;
        g = (green * Index) / TotalSteps;
        b = (blue * Index) / TotalSteps;
      break;

    case -1: // decreasing brightness
        r = (red * (TotalSteps - Index)) / TotalSteps;
        g = (green * (TotalSteps - Index)) / TotalSteps;
        b = (blue * (TotalSteps - Index)) / TotalSteps;
        break;
  }

  Index++;

  // set all pixels to the new colour
  for (int i = 0; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, r, g, b);
  }

  // check for limits:
  if (Index == TotalSteps) {
    Index = 0;
    dir = -dir;
  } 

  // update the strip
  strip->show();
  delay(10);
}

