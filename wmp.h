#ifndef __WMP_H__
#define __WMP_H__

#include "WProgram.h"
#include <Wire.h>

class WiiMotionPlus {
  public:
  
  WiiMotionPlus() {
    Wire.begin();
  }
  
  void activate(){
    Wire.beginTransmission(0x53); // WM+ starts out deactivated at address 0x53
    Wire.send(0xfe); // send 0x05 to address 0xFE to activate WM+
    Wire.send(0x05);  // pass-through mode (normal mode is 0x04)
    Wire.endTransmission(); // WM+ jumps to address 0x52 and is now active
  }
  
  void sendZero(){
    Wire.beginTransmission(0x52); // now at address 0x52
    Wire.send(0x00); // send zero to signal we want info
    Wire.endTransmission();
  }
  
  void receiveData(byte data[], int length=6) {
    Wire.requestFrom(0x52, length);
    for (int i = 0; i < length; i++){
      data[i] = (byte)Wire.receive();
    }
  }

};

#endif
