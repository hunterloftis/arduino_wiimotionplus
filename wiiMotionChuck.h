#ifndef __WIIMOTIONCHUCK_H__
#define __WIIMOTIONCHUCK_H__

#include "WProgram.h"
#include <Wire.h>

class WiiMotionChuck {
  public:
  
  float wmpToDegreesPerSec;
  float wmpFastMultiplier;
  float frameToSec;
  int updateInterval;
  int calibrations;
  boolean debug;

  unsigned long lastUpdate;
  
  byte data[6];
  int yaw, pitch, roll;
  
  boolean slowYaw, slowPitch, slowRoll;
  boolean invertYaw, invertPitch, invertRoll;
  
  float yawZero, pitchZero, rollZero;
  float yawVelocity, pitchVelocity, rollVelocity;
  float yawPosition, pitchPosition, rollPosition;

  WiiMotionChuck() {
    wmpToDegreesPerSec = 16.0;
    wmpFastMultiplier = 4.0;
    frameToSec = 0.001;
    updateInterval = 10;
    calibrations = 10;
    debug = false;
    lastUpdate = 0;
    yawZero = pitchZero = rollZero = 0.0;
    yawVelocity = pitchVelocity = rollVelocity = 0.0;
    yawPosition = pitchPosition = rollPosition = 0.0;
    Wire.begin();
  }
  
  void start() {
    wmpOn();
  }
  
  void invert(boolean yaw, boolean pitch, boolean roll) {
    
  }
  
  void interval(int i) {
    updateInterval = i;  
  }
  
  void calibrate() {
    calibrateZeroes();  
  }
  
  void update() {
    unsigned long now = millis();
    unsigned long frame = now - lastUpdate;
    if (frame >= updateInterval) {
      receivePosition(frame);
      lastUpdate = now;
    } 
  }
  
  float* getPosition() {
    float pos[] = {10, 11, 12, yawPosition, pitchPosition, rollPosition};  
    return pos;
  }
  
  void wmpOn(){
    Wire.beginTransmission(0x53); //WM+ starts out deactivated at address 0x53
    Wire.send(0xfe); //send 0x04 to address 0xFE to activate WM+
    Wire.send(0x05);  // pass-through mode
    Wire.endTransmission(); //WM+ jumps to address 0x52 and is now active
  }
  
  void wmpSendZero(){
    Wire.beginTransmission(0x52); //now at address 0x52
    Wire.send(0x00); //send zero to signal we want info
    Wire.endTransmission();
  }
  
  void receiveData() {
    wmpSendZero(); //send zero before each request (same as nunchuck)
    Wire.requestFrom(0x52,6); //request the six bytes from the WM+
    for (int i=0;i<6;i++){
      data[i]=Wire.receive();
    }
  }
  
  void calibrateZeroes(){
    delay(500);
    for (int i=0;i<calibrations;i++){
      receiveVelocity();
      yawZero += yawVelocity / calibrations;
      pitchZero += pitchVelocity / calibrations;
      rollZero += rollVelocity / calibrations;
      delay(50);
    }
  }
  
  void receiveYawPitchRoll(){
    receiveData();
    
    yaw=((data[3]>>2)<<8)+data[0]; 
    pitch=((data[5]>>2)<<8)+data[2]; 
    roll=((data[4]>>2)<<8)+data[1]; 
    
    slowYaw = data[3] & 2;
    slowPitch = data[3] & 1;
    slowRoll = data[4] & 2;
  }
  
  void receiveVelocity() {
    receiveYawPitchRoll();
    
    yawVelocity = yaw / wmpToDegreesPerSec;
    pitchVelocity = pitch / wmpToDegreesPerSec;
    rollVelocity = roll / wmpToDegreesPerSec;
  }
  
  void receiveRelativeVelocity() {
    receiveVelocity();
    
    yawVelocity -= yawZero;
    pitchVelocity -= pitchZero;
    rollVelocity -= rollZero;
    
    if (!slowYaw) yawVelocity *= wmpFastMultiplier;
    if (!slowPitch) pitchVelocity *= wmpFastMultiplier;
    if (!slowRoll) rollVelocity *= wmpFastMultiplier;
  }
  
  void receivePosition(unsigned long frame) {
    receiveRelativeVelocity();
  
    float toSec = frame * frameToSec;
   
    // Integrete velocity into position
    
    yawPosition += yawVelocity * toSec;
    pitchPosition += pitchVelocity * toSec;
    rollPosition += rollVelocity * toSec;  
  }

};

#endif
