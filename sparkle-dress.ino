#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_BLE.h>
#include <Adafruit_BluefruitLE_UART.h>

// project specific includes
#include "BluefruitConfig.h"  // parameters for the BLE module
#include "Motion.h"           // brings in the motion sensing interface
#include "Effects.h"          // defines the interfaces to the different visual effects we can run

// uncomment this line to get debug output on the serial port
//#define DEBUG

/*
 * Here we're going to set the number of NeoPixels attached to the dress
 */
#define NUM_PIXELS  20

/*
 * This value sets which pin we're going to send the pixel data out
 */
#define PIXEL_PIN   6

/*
 * This value sets the pin we're going to blink to show when a new command has been received
 */
#define LED_PIN     7

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// define BLE radio interface; second parameter sets the mode pin, but we're not too worried about that here, hence the -1
Adafruit_BluefruitLE_UART ble(Serial1, -1);

// mess with this number to adjust TWINklitude :)
// lower number = more sensitive
#define MOVE_THRESHOLD 70

extern uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
extern float parsefloat(uint8_t *buffer);
extern void printHex(const uint8_t * data, const uint32_t numBytes);

void setup() 
{
  // set up the LED for indicating failure/new commands
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
#ifdef DEBUG
  while (!Serial);  // required for Flora & Micro
  Serial.begin(9600);
#endif
  
  // Try to initialise and warn if we couldn't detect the chip
  if (!initMotion())
  {
#ifdef DEBUG
    Serial.println("Oops ... unable to initialize the LSM303. Check your wiring!");
#endif

    while (1) {
      digitalWrite(LED_PIN, HIGH);
      delay(750);
      digitalWrite(LED_PIN, LOW);
      delay(250);
    }
  }

  // initialise the Pixel strip
  strip.begin();
  strip.setBrightness(20);
  strip.show(); // Initialize all pixels to 'off'

  // configure and enable the BLE module
  if (!ble.begin(true)) {
#ifdef DEBUG
    Serial.println("Oops ... unable to initialise BLE module!");
#endif

    while (1) {
      digitalWrite(LED_PIN, HIGH);
      delay(250);
      digitalWrite(LED_PIN, LOW);
      delay(750);
    }
  }

  // disable echo
  ble.echo(false);
  ble.info();
  ble.verbose(false);

  ble.setMode(BLUEFRUIT_MODE_DATA);

  /*
  while (!ble.isConnected()) {
    delay(500);
  }
  */
}

enum modes {
  mode_twinkle = 1,   // sparkle
  mode_larson,        // scan back and forth across the dress
  mode_rainbow,       // rainbow cycle on all pixels
  mode_colour,        // solid colour on all pixels
  mode_hearbeat       // fade between colour and off, like 'breathing'
};

// set default mode
enum modes current_mode = mode_twinkle;
enum modes last_mode = mode_twinkle;

extern uint8_t packetbuffer[];

// colours used for the mode_colour and mode_heartbeat settings
uint8_t red = 0,
        green = 0,
        blue = 0,
        old_red = 0,
        old_green = 0,
        old_blue = 0;

void loop() 
{
  //if (ble.isConnected())
  {
    // check and see if we've got a message from the radio to switch mode
    uint8_t len = readPacket(&ble, BLE_READ_PACKET_TIMEOUT);
  
    // did we get something?
    if (len != 0) {
      digitalWrite(LED_PIN, HIGH);
  #ifdef DEBUG    
      Serial.println("Message revceived");
  #endif
      // only button presses will change the mode:
      if ((packetbuffer[1] == 'B') && (packetbuffer[3] == '1')) {
        last_mode = current_mode; // preserve old mode; some modes need to know to update the string
        current_mode = (enum modes)(packetbuffer[2]-'0');
  #ifdef DEBUG
        Serial.print("mode switched to ");
        Serial.println((char)packetbuffer[2]);
  #endif
      }
  
      // if we receive a colour from the picker, then we'll save that for mode_colour and mode_heartbeat
      if (packetbuffer[1] == 'C') {
        red = packetbuffer[2];
        green = packetbuffer[3];
        blue = packetbuffer[4];
  
  #ifdef DEBUG
        Serial.print("new colour recevied: #");
        if (red < 0x10) Serial.print("0");
        Serial.print(red, HEX);
        if (green < 0x10) Serial.print("0");
        Serial.print(green, HEX);
        if (blue < 0x10) Serial.print("0");
        Serial.println(blue, HEX);
  #endif
      }
      digitalWrite(LED_PIN, LOW);
    }
  }

  // do whatever this mode wants:
  switch (current_mode) {
    case mode_twinkle:
      if (checkMotion(MOVE_THRESHOLD)) {
        twinkle(&strip, 5, 1);
        twinkle(&strip, 5, 3);
        twinkle(&strip, 5, 2);
      }
      break;

    case mode_larson:
      larsonScanner(&strip);
      break;

    case mode_rainbow:
      rainbow(&strip);
      break;
      
    case mode_colour: // make all pixels the same colour, but only if it changes, or we've just entered this mode
      if ((old_red != red) || (old_green != green) || (old_blue != blue) || (last_mode != current_mode)) {
        // save the colour
        old_red = red;
        old_green = green;
        old_blue = blue;

        for (int i = 0; i < strip.numPixels(); i++) {
          strip.setPixelColor(i, red, green, blue);
        }

        // update the strip!
        strip.show();
      }
      break;
      
    case mode_hearbeat:
      if ((old_red != red) || (old_green != green) || (old_blue != blue)) {
        // save the colour:
        old_red = red;
        old_green = green;
        old_blue = blue;
      }
      
      heartbeat(&strip, red, green, blue);
      break;
  }
}


