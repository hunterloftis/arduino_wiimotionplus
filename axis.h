#ifndef __AXIS_H__
#define __AXIS_H__

#include "WProgram.h"

class Axis {
  public:
  
  float wmpToDegreesPerSec;
  float wmpFastMultiplier;
  
  boolean inverted;
  int calibrations;
  
  int value;
  
  float zero;
  float velocity;
  float position;
  
  Axis() {
    wmpToDegreesPerSec = 16.0;
    wmpFastMultiplier = 4.0;  
    zero = velocity = position = 0.0;
    calibrations = 0;
  }
  
  void calibrate(int val, boolean slow, boolean log) {
    zero = zero * ((float)calibrations / (calibrations + 1));
    calibrations++;
    float velocity0 = inverted ? -val / wmpToDegreesPerSec : val / wmpToDegreesPerSec;
    if (!slow) velocity0 *= wmpFastMultiplier;
    zero += (float)velocity0 / calibrations;
  }
  
  void update(int val, boolean slow, int frame, boolean log) {
    value = val;
    velocity = inverted ? -val / wmpToDegreesPerSec - zero : val / wmpToDegreesPerSec - zero;
    if (!slow) velocity *= wmpFastMultiplier;
    position += velocity * frame * .001;
    if (log) {
    }
  }
    
};

#endif
