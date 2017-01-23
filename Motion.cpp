/*
 * Implements the motion sensitive parts of the dress
 */

#include <Adafruit_LSM303.h>
#include "Motion.h"

 Adafruit_LSM303 lsm;


bool initMotion() {
  return lsm.begin();  
}

bool checkMotion(uint16_t threshold) {
  bool moving = false; // start off with the assumption we're not moving
  
  // Take a reading of accellerometer data
  lsm.read();

  // Get the magnitude (length) of the 3 axis vector
  // http://en.wikipedia.org/wiki/Euclidean_vector#Length
  double storedVector = lsm.accelData.x*lsm.accelData.x;
  storedVector += lsm.accelData.y*lsm.accelData.y;
  storedVector += lsm.accelData.z*lsm.accelData.z;
  storedVector = sqrt(storedVector);
    
  // wait a bit
  delay(100);
  
  // get new data!
  lsm.read();
  double newVector = lsm.accelData.x*lsm.accelData.x;
  newVector += lsm.accelData.y*lsm.accelData.y;
  newVector += lsm.accelData.z*lsm.accelData.z;
  newVector = sqrt(newVector);
    
  // are we moving 
  if (abs(newVector - storedVector) > threshold) {
    moving = true; // yes we are
  }

  return moving;
}

