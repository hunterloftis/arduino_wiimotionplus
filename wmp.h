#ifndef __WMP_H__
#define __WMP_H__

#include "WProgram.h"
#include <Wire.h>

class WiiMotionPlus {
  public:
  
  static const int MP_MODE = 0x04;  // Motion plus solo  
  static const int NC_MODE = 0x05;  // Nunchuk passthrough
  static const int CC_MODE = 0x07;  // Classic controller passthrough
    
  WiiMotionPlus() {
    
  }
  

  void activateModeNC() {
    
  }
  
  void activate(){
    byte data[6];
    // Activate WM+
    Serial.println("Intitializing MP");
    Wire.beginTransmission(0x52);
    Serial.println("1");
    Wire.send(0xf0);
    Serial.println("2");    
    Wire.send(0x55);
    Serial.println("3");    
    Wire.endTransmission();
    Serial.println("4");    
    delay(250);
    // Activate WM+ in NC passthrough mode    
    Serial.println("Setting mode: passthrough");
    Wire.begin();
    Wire.beginTransmission(0x53);
    Wire.send(0xfe);
    Wire.send(0x05);
    Wire.endTransmission();
    delay(250);
    // Set reading address
    Serial.print ("Setting reading address at 0xFA");
    Wire.beginTransmission(0x52); //0xA4
    Wire.send(0xfa);
    Wire.endTransmission();
    delay(250);
    // Read ID of WMP
    Serial.println("Reading MP ID");
    Wire.requestFrom(0x52, 6);
    int xID = 0;
    for(int i = 0; i < 6; i++) {
      data[i] = Wire.receive(); 
      xID += data[i];
    }
    Serial.print("Extension controller xID = 0x");
    Serial.println(xID, HEX);
    if (xID == 0xcb) Serial.println("MP+ on but not active");
    else if (xID == 0xCE) Serial.println("MP+ on and active");
    else if (xID == 0x00) Serial.println("MP+ not on");
    else Serial.println("MP+ in unknown state");
    delay(500);
    // Point the read address to 0xA40008 where the 6-byte data is stored
    Wire.beginTransmission(0x52);
    Wire.send(0x08);
    Wire.endTransmission();
    delay(250);
  }
  
  void sendZero(){
    Wire.beginTransmission(0x52); // now at address 0x52
    Wire.send(0x00); // send zero to signal we want info
    Wire.endTransmission();
  }
  
  void receiveData(byte data[], int length=6) {
    Wire.requestFrom(0x52, length);
    for (int i = 0; i < length; i++){
      data[i] = Wire.receive();
    }
  }
  
};

#endif
