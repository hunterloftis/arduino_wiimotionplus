#ifndef __WIIMOTIONCHUCK_H__
#define __WIIMOTIONCHUCK_H__

#include "WProgram.h"
#include "wmp.h"
#include "axis.h"

class WiiMotionChuck {
  public:
  
  int updateInterval;
  int passthroughInterval;
  int calibrations;
  boolean debug;

  unsigned long lastUpdate;
  
  WiiMotionPlus wmp;
  byte data[6];
  Axis axis[3];
  static const int axis_length = 3;
    
  WiiMotionChuck() {
    updateInterval = 10;
    passthroughInterval = 3;
    calibrations = 10;
    debug = false;
    lastUpdate = 0;
  }
  
  void invert(int index, boolean inverted) {
    axis[index].inverted = inverted;
  }
  
  void interval(int i) {
    updateInterval = i;  
  }
  
  void start() {
    wmp.activate();
  }
  
  void calibrate() {
    calibrateZeroes();  
  }
  
  void calibrateZeroes(){
    byte data[6];
    delay(500);
    for (int i = 0; i < calibrations; i++){
      wmp.sendZero();
      wmp.receiveData(data);
      axis[0].calibrate( ((data[3] >> 2) << 8) + data[0], data[3] & 2, false); // Yaw
      axis[1].calibrate( ((data[5] >> 2) << 8) + data[2], data[3] & 1, false); // Pitch
      axis[2].calibrate( ((data[4] >> 2) << 8) + data[1], data[4] & 2, false); // Roll
      delay(50);
    }
  }
  
  void update() {
    unsigned long now = millis();
    unsigned long frame = now - lastUpdate;
    if (frame >= updateInterval) {
      update_rotation(frame);
      lastUpdate = now;
    } 
  }
  
  void update_rotation(float frame) {
    
    wmp.sendZero();
    wmp.receiveData(data);
    
    axis[0].update( ((data[3] >> 2) << 8) + data[0], data[3] & 2, frame, true); // Yaw
    axis[1].update( ((data[5] >> 2) << 8) + data[2], data[3] & 1, frame, false); // Pitch
    axis[2].update( ((data[4] >> 2) << 8) + data[1], data[4] & 2, frame, false); // Roll

  }
  
  void getPosition(float pos[]) {
    pos[3] = axis[0].position;
    pos[4] = axis[1].position;
    pos[5] = axis[2].position;
  }
  
  void getVelocity(float vel[]) {
    vel[3] = axis[0].velocity;
    vel[4] = axis[1].velocity;
    vel[5] = axis[2].velocity;
  }
  
  void getValue(int val[]) {
    val[3] = axis[0].value;
    val[4] = axis[1].value;
    val[5] = axis[2].value;
  }

};

#endif
