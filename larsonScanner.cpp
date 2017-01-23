#include "Effects.h"


void larsonScanner(Adafruit_NeoPixel *strip) {
  // static vars to hold where we are in the step
  static int pos = 0;
  static int dir = 1;
 
  // Draw 5 pixels centered on pos.  setPixelColor() will clip any
  // pixels off the ends of the strip, we don't need to watch for that.
  strip->setPixelColor(pos - 1, 0x100000); // Dark red
  strip->setPixelColor(pos    , 0xFF3000); // Center pixel is brightest
  strip->setPixelColor(pos + 1, 0x100000); // Dark red
 
  strip->show();
  delay(70);
 
  // Rather than being sneaky and erasing just the tail pixel,
  // it's easier to erase it all and draw a new one next time.
  strip->clear();
 
  // Bounce off ends of strip
  pos += dir;
  if(pos < 0) {
    pos = 1;
    dir = -dir;
  } else if(pos >= strip->numPixels()) {
    pos = strip->numPixels() - 2;
    dir = -dir;
  }
}
