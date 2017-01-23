#include <Adafruit_NeoPixel.h>

/*
 * Implements a larson scanner, step-by-step (keep calling from loop() as long 
 * as you want it to scan back and forth). Remebers where it got to, so you only
 * need to feed it the details of the strip. Will also auto-scale to the size of
 * the strip automagically
 */
void larsonScanner(Adafruit_NeoPixel *strip);

/*
 * Implements a twinkling effect
 * 
 * Parameters
 * strip        The strip to operate on
 * wait         smaller = shorter twinkle
 * howmany      Number of pixels to simultaneously light up
 */
void twinkle(Adafruit_NeoPixel *strip, int wait, uint8_t howmany);

/*
 * Implements a rainbow cycle across all pixels in the strip.
 * Rainbow 'slides' down the length of the string
 */
void rainbow(Adafruit_NeoPixel *strip);

/*
 * Implements a 'heartbeat'-like fade in and out of the pixels to the specified colour
 */
void heartbeat(Adafruit_NeoPixel *strip, uint8_t red, uint8_t green, uint8_t blue);
